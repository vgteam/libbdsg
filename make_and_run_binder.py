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
bindings_dir = 'cmake_bindings'
this_project_source = f'{os.getcwd()}/src'
this_project_include = f'{os.getcwd()}/include'
this_project_deps = f'{os.getcwd()}/build' # This is the CMake build directory where depoendencies must have already been fetched by cmake.
this_project_namespace_to_bind = 'bdsg'
python_module_name = 'bdsg'

def clone_repos():
    ''' download the most recent copy of binder from git '''
    if not glob.glob("binder"):
        print("Binder not found, cloning repo...")
        # TODO: Change back to https://github.com/RosettaCommons/binder.git
        # master when https://github.com/RosettaCommons/binder/pull/99 is
        # fixed.
        subprocess.check_call(['git', 'clone', '--branch', 'patch-1', 'https://github.com/adamnovak/binder.git', 'binder'])

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
        subprocess.check_call([sys.executable, 'build.py', '--jobs', str(multiprocessing.cpu_count())])
    return "binder/" + glob.glob('./build/*/*/bin/')[0] + "binder"

def all_sources_and_headers(include_deps=False):
    '''
    Find all source or include files relevant to the project.
    Yields their paths.
    Assumes the CMake build has run in ./build.
    
    Note that we count the libhandlegraph sources as part of this project's
    sources. We include them even if include_deps is false and we aren't
    including the other dependencies.
    '''
    
    # Define the extensions we are interested in
    extensions = ['hpp', 'cpp', 'h', 'cc', 'c']
    # And the paths we want to look in.
    # Always include libhandlegraph.
    paths = [f'{this_project_source}/**/*', f'{this_project_include}/**/*', f'{this_project_deps}/handlegraph*/**/*']
    if include_deps:
        # Include all dependencies if asked
        paths.append(f'{this_project_deps}/*/src/*/*')
    # Get an iterable of glob iterables that search all combinations
    all_globs = (glob.glob(f'{f}.{e}', recursive=True) for f, e in itertools.product(paths, extensions))
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
    matcher = re.compile('^\s*#include "')
    # find instances of includes we need to change
    for filename in all_sources_and_headers():
        changes_made[filename] = list()
        with open(filename, 'r') as fh:
            for line in fh:
                if matcher.match(line):
                    spl = line.split('"')
                    replacement = f'{spl[0]}<{spl[1]}>\n'
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
    
    include_regex = re.compile('^\s*#include\s+(["<])(.*)([">])')
    
    for filename in all_sources_and_headers(include_deps=False):
        # Then for each file found by any search
        with open(filename, 'r') as fh:
            for line in fh:
                # Look at each line
                match = include_regex.match(line)
                if match:
                    # This is an include directive. Parse it
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
        for include in all_includes:
            fh.write(f'{include}\n')
    return all_include_filename


def make_bindings_code(all_includes_fn, binder_executable):
    ''' runs the binder executable with required parameters '''
    # Find all the include directories for dependencies.
    # Some dependency repos have an include and some have an src/include.
    # TODO: This (and a lot of this script) relies on a cmake build having been done out of "build" beforehand!
    # BBHash and sparsepp have weird project structures and needs to be handled specially.
    proj_include = (glob.glob("build/*/src/*/include") +
                    glob.glob("build/*/src/*/src/include") +
                    ["build/sparsepp-prefix/src/sparsepp",
                     "build/bbhash-prefix/src/bbhash"])
    # proj_include = " -I".join(proj_include)
    proj_include = [f'-I{i}' for i in proj_include]
    
    command = [binder_executable,
        "--root-module", python_module_name,
        "--prefix", f'{os.getcwd()}/{bindings_dir}/',
        '--bind', this_project_namespace_to_bind,
        "--config", "config.cfg",
        all_includes_fn,
        "--",
        "-std=c++14",
        f'-I{this_project_include}']
    if platform.system() == 'Darwin':
        # On (newer) Macs, Binder can't find the C++ STL because it is not in
        # /usr/include but under a weird path returned by xcode-select -p and
        # then /usr/include.  See
        # https://github.com/RosettaCommons/binder/issues/26#issuecomment-322538385
        # and
        # https://developer.apple.com/documentation/xcode_release_notes/xcode_10_release_notes#3035624
        stl_path = os.path.join(subprocess.check_output(['xcode-select', '-p']).decode('utf8').strip(), 'usr', 'include', 'c++', 'v1')
        command.append('-isystem' + stl_path)
        # But we also need the MacOS SDK, which provides e.g. the "real" string.h that this STL depends on
        sdk_path=subprocess.check_output(['xcrun', '-sdk', 'macosx', '--show-sdk-path']).decode('utf8').strip()
        command.append('-isysroot' + sdk_path)
        # Also make sure to look for libomp from macports or homebrew, like CMakeLists.txt does
        command.append('-I/opt/local/include/libomp')
        command.append('-I/usr/local/include')
    command = command + proj_include
    command.append("-DNDEBUG")
    command.append("-v")
    print('BINDER COMMAND:', ' '.join(command))
    
    shutil.rmtree(bindings_dir, ignore_errors=True)
    os.mkdir(bindings_dir)
    subprocess.check_call(command)

def main():
    clone_repos()
    os.chdir("binder")
    binder_executable = build_binder()
    os.chdir("..")
    with clean_includes():
        all_includes_fn = make_all_includes()
        make_bindings_code(all_includes_fn, binder_executable)

if __name__ == '__main__':
    main()
