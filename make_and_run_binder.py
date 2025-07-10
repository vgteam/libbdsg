#!/usr/bin/env python3

import glob
import os
import platform
import sys
import shutil
import subprocess
import re
import multiprocessing
import itertools
from contextlib import contextmanager



# Overall script settings
this_project_package = f'{os.getcwd()}/bdsg'
this_project_source = f'{this_project_package}/src'
this_project_include = f'{this_project_package}/include'
this_project_deps = f'{this_project_package}/deps' # Now deps come from submodules.
bindings_dir = f'{this_project_package}/cmake_bindings'
this_project_namespace_to_bind = 'bdsg'
python_module_name = 'bdsg'

# We have one global notion of what an include looks like
INCLUDE_REGEX = re.compile(r'^\s*#include\s+(["<])(.*)([">])')
# We have one master list of source code extensions
SOURCE_EXTENSIONS = ['hpp', 'cpp', 'h', 'cc', 'c']

def clone_repos():
    ''' download the most correct binder and pybind11 from git '''
    if not glob.glob("binder"):
        print("Binder not found, cloning repo...")
        subprocess.check_call(['git', 'clone', 'https://github.com/RosettaCommons/binder.git', 'binder'])
        parent = os.getcwd()
        os.chdir('binder')
        # See also: Binder commit defined in CMakeLists.txt for header files.
        subprocess.check_call(['git', 'checkout', '46ec0e88137d368eeedafecfa123004f8ad028d1'])
        os.chdir(parent)
    if not glob.glob("binder/build/pybind11"):
        print("pybind11 not found, cloning repo...")
        parent = os.getcwd()
        os.chdir('binder')
        os.makedirs('build', exist_ok=True)
        subprocess.check_call(['git', 'clone', 'https://github.com/RosettaCommons/pybind11.git', 'build/pybind11'])
        os.chdir('build/pybind11')
        # See also: pybind11 commit defined in CMakeLists.txt
        subprocess.check_call(['git', 'checkout', '5b0a6fc2017fcc176545afe3e09c9f9885283242'])
        os.chdir(parent)


def build_binder():
    '''
    Check for binder executable in the location we expect it.
    If it's not there, build binder with the included script.
    Expects to run in the binder directory.
    :return: location of executable, relative to project directory
    '''
    if not glob.glob("./build/*/*/bin/*"):
        print("Binder not compiled, using packaged build.py...")
        # TODO: Use CPU counting that accounts for container quotas?
        build_command = [
            sys.executable,
            'build.py',
            '--compiler',
            'clang' if platform.system() == 'Darwin' else 'gcc',
            '--jobs',
            str(multiprocessing.cpu_count()),
            '--pybind11',
            os.path.join(os.getcwd(), 'build/pybind11'),
            '--llvm-version',
            '14.0.5'
        ]
        subprocess.check_call(build_command)
    return "binder/" + glob.glob('./build/*/*/bin/')[0] + "binder"

def all_sources_and_headers(include_deps=False):
    '''
    Find all source or include files relevant to the project.
    Yields their paths.
    
    Note that we count the libhandlegraph sources as part of this project's
    sources. We include them even if include_deps is false and we aren't
    including the other dependencies.
    '''
    
    # And the paths we want to look in.
    # Always include libhandlegraph.
    paths = [f'{this_project_source}/**/*', f'{this_project_include}/**/*', f'{this_project_deps}/libhandlegraph/src/**/*']
    if include_deps:
        # Include all dependencies if asked
        paths.append(f'{this_project_deps}/**/*')
    # Get an iterable of glob iterables that search all combinations
    all_globs = (glob.glob(f'{f}.{e}', recursive=True) for f, e in itertools.product(paths, SOURCE_EXTENSIONS))
    # Deduplicate overlapping globs
    seen = set()
    for filename in itertools.chain.from_iterable(all_globs):
        if filename not in seen:
            yield filename
            seen.add(filename)
        
    #    files = list()
    #    searchroot = os.path.abspath(f'{this_project_source}/../')
    #    for (root,dirs,fils) in os.walk(searchroot):
    #        for fl in fils:
    #            if(fl.endswith(("hpp","cpp","h","cc","c")) and ("src" in root or "include" in root)):
    #                files.append(root+"/"+fl)
    #    print(f'found source files {files}')
    #    for filename in files:
    
    

@contextmanager
def clean_includes():
    '''
    Goes through source code and replaces all quote-format includes with carrot-style includes on entry.

    Reverts changes on exit.
    '''
    changes_made = dict()
    # find instances of includes we need to change
    for filename in all_sources_and_headers():
        changes_made[filename] = list()
        with open(filename, 'r') as fh:
            for line in fh:
                match = INCLUDE_REGEX.match(line)
                if match:
                    replacement = line[:match.start()] + f'#include <{match.group(2)}>' + line[match.end():]
                    changes_made[filename].append((line, replacement))
        if not changes_made[filename]:
            del changes_made[filename]
    # edit files we need to alter and then resave them
    for filename in changes_made.keys():
        filedata = ""
        listInd = 0
        with open(filename, 'r') as fh:
            for line in fh:
                if listInd < len(changes_made[filename]) and line == changes_made[filename][listInd][0]:
                    filedata += changes_made[filename][listInd][1]
                    listInd += 1
                else:
                    filedata += line
        with open(filename, 'w') as fh:
            fh.write(filedata)
    try:
        yield
    finally:
        for filename in changes_made.keys():
            filedata = ""
            listInd = 0 
            with open(filename, 'r') as fh:
                for line in fh:
                    if listInd < len(changes_made[filename]) and line == changes_made[filename][listInd][1]:
                        filedata += changes_made[filename][listInd][0]
                        listInd += 1
                    else:
                        filedata += line
            with open(filename, 'w') as fh:
                fh.write(filedata)
        

def make_all_includes():
    '''
    Generates an .hpp file with all includes in this project that need to be bound.
    We collect all the include directives from this project's sources.
    '''
    
    all_includes = []
    all_include_filename = 'all_cmake_includes.hpp'
    
    for filename in all_sources_and_headers(include_deps=False):
        # Then for each file found by any search
        with open(filename, 'r') as fh:
            for line in fh:
                if 'BINDER_IGNORE' in line:
                    # Skip includes that are maybe not available on all systems
                    continue
                # Look at each line
                match = INCLUDE_REGEX.match(line)
                if match:
                    # This is an include directive that makes sense to include here. Parse it
                    is_relative = match.group(1) == '"'
                    included_path = match.group(2)
                    assert (match.group(1) == '"') == (match.group(3) == '"'), "Mismatched include delimiters in " + filename + " for " + included_path
                    
                    # Relative includes arent really relative paths so we can't really resolve them.
                    
                    # Just collect all the includes as <>
                    all_includes.append(f'#include <{included_path}>')
    all_includes = list(set(all_includes))
    # This is to ensure that the list is always the same and doesn't
    # depend on the filesystem state.  Not technically necessary, but
    # will cause inconsistent errors without it.
    all_includes.sort()
    with open(all_include_filename, 'w') as fh:
        # Start by always including the binding-generation-time hook file, with
        # things Binder needs to see to generate good bindings.
        fh.write('#include <bdsg/internal/binder_hook_bind.hpp>\n')
        for include in all_includes:
            fh.write(f'{include}\n')
    return all_include_filename
    
    
def postprocess_bindings():
    '''
    Modify generated bindings files to correct Binder's STL-version-dependent code to portable code.
    '''
    
    # We apply each of these to all source files with sed.
    transformations = ['s/class std::__cxx11::basic_string<char>/std::string/g', # We can't leave "class" in front of a non-template
        's/std::__cxx11::basic_string<char>/std::string/g']
    # TODO: Add transformations to catch problems from libc++ STL
    
    for (directory, subdirectories, files) in os.walk(bindings_dir):
        for filename in files:
            if os.path.splitext(filename)[1].lstrip('.') in SOURCE_EXTENSIONS:
                # For each source file, get its full path from where our process is
                full_path = os.path.join(directory, filename)
                for transformation in transformations:
                    # Apply all the transformations
                    subprocess.check_call(['sed', "-i.bak", transformation, full_path])
                    os.unlink(full_path + '.bak')


def make_bindings_code(all_includes_fn, binder_executable):
    ''' runs the binder executable with required parameters '''
    # Find all the include directories for dependencies.
    # Some dependency repos have an include and some have an src/include.
    # BBHash and sparsepp have weird project structures and needs to be handled specially.
    proj_include = (glob.glob(f'{this_project_deps}/*/include') +
                    glob.glob(f'{this_project_deps}/*/src/include') +
                    [f'{this_project_deps}/sparsepp',
                     f'{this_project_deps}/BBHash'])
    # proj_include = " -I".join(proj_include)
    proj_include = [f'-I{i}' for i in proj_include]
    
    command = [binder_executable,
        "--root-module", python_module_name,
        "--prefix", f'{bindings_dir}/',
        '--bind', this_project_namespace_to_bind,
        "--config", "config.cfg",
        all_includes_fn,
        "--",
        "-std=c++14",
        f'-I{this_project_include}']
    if platform.system() == 'Darwin':
        # We need the MacOS SDK, which provides the C standard library and also a C++ STL, at least as of Apple Clang 14.
        sdk_path=subprocess.check_output(['xcrun', '-sdk', 'macosx', '--show-sdk-path']).decode('utf8').strip()
        command.append('-isysroot' + sdk_path)
        # Also make sure to look for libomp from macports or homebrew, like CMakeLists.txt does
        homebrew_prefix = os.environ.get('HOMEBREW_PREFIX', '/opt/homebrew')
        try:
            homebrew_prefix = subprocess.check_output(['brew', '--prefix']).decode('utf-8').strip('\n')
        except subprocess.CalledProcessError:
            pass
        except FileNotFoundError:
            pass
        command.append(f'-I{homebrew_prefix}/opt/libomp/include')
        command.append('-I/opt/local/include/libomp')
        command.append('-I/usr/local/include')
        print(command)
    else:
        # With current GCC, Clang can't find the multiarch-specific *and*
        # GCC-version-specific include path where the OpenMP headers live.
        # So help it out.
        # TODO: We're assuming we're using GCC.
        compiler_version = int(subprocess.check_output(["gcc", "-dumpversion"]).decode('utf-8').split('.')[0])
        compiler_triple = subprocess.check_output(["gcc", "-dumpmachine"]).decode('utf-8').strip()
        command.append('-I' + f"/usr/lib/gcc/{compiler_triple}/{compiler_version}/include")
        # We rely on macro hacks in binder_hook_bind.hpp to translate the file
        # into something Binder can understand.

    # Find Jansson
    jansson_flags = subprocess.check_output(['pkg-config', '--cflags', 'jansson']).decode('utf-8').strip().split(' ')
    command += jansson_flags

    command = command + proj_include
    command.append("-DNDEBUG")
    command.append("-v")
    print('BINDER COMMAND:', ' '.join(command))
    
    shutil.rmtree(bindings_dir, ignore_errors=True)
    os.mkdir(bindings_dir)
    subprocess.check_call(command)
    
    # Do some post-processing on the bindings
    postprocess_bindings()
    

                
def main():
    clone_repos()
    parent = os.getcwd()
    os.chdir("binder")
    binder_executable = build_binder()
    os.chdir(parent)
    with clean_includes():
        all_includes_fn = make_all_includes()
        make_bindings_code(all_includes_fn, binder_executable)

if __name__ == '__main__':
    main()
