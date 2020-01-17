#!/usr/bin/env python3

import glob
import os
import sys
import shutil
import subprocess
import git
import re
from distutils.sysconfig import get_python_inc


# Overall script settings
bindings_dir = 'cmake_bindings'
this_project_source = f'{os.getcwd()}/src'
this_project_include = f'{os.getcwd()}/include' 
this_project_namespace_to_bind = 'bdsg'
python_module_name = 'bdsg'

def clone_repos():
    if not glob.glob("binder"):
        print("Binder not found, cloning repo...")
        os.mkdir("binder")
        git.Git(".").clone("https://github.com/RosettaCommons/binder.git")

def build_binder():
    if not glob.glob("./build/*/*/bin/*"):
        print("Binder not compiled, using packaged build.py...")
        os.system(f'{get_python_inc().split("/")[-1]} build.py')
    pybind_source = f'binder/build/pybind11/include'
    return "binder/" + glob.glob('./build/*/*/bin/')[0] + "binder"

def clean_includes():
    changes_made = dict()
    matcher = re.compile('^\s*#include "')
    # find instances of includes we need to change
    for filename in (glob.glob(f'{this_project_source}/**/*.hpp', recursive=True) + 
                     glob.glob(f'{this_project_source}/**/*.cpp', recursive=True) +
                     glob.glob(f'{this_project_source}/**/*.h', recursive=True) +
                     glob.glob(f'{this_project_source}/**/*.cc', recursive=True) + 
                     glob.glob(f'{this_project_source}/**/*.c', recursive=True) + 
                     glob.glob(f'{this_project_source}/../build/*/src/*/*.hpp', recursive=True) + 
                     glob.glob(f'{this_project_source}/../build/*/src/*/*.cpp', recursive=True) + 
                     glob.glob(f'{this_project_source}/../build/*/src/*/*.h', recursive=True) + 
                     glob.glob(f'{this_project_source}/../build/*/src/*/*.cc', recursive=True) + 
                     glob.glob(f'{this_project_source}/../build/*/src/*/*.c', recursive=True)):
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
    return changes_made

def make_all_includes():
    all_includes = []
    all_include_filename = 'all_cmake_includes.hpp'
    for filename in (glob.glob(f'{this_project_source}/**/*.hpp', recursive=True) +
                     glob.glob(f'{this_project_source}/**/*.cpp', recursive=True) +
                     glob.glob(f'{this_project_source}/**/*.h', recursive=True) +
                     glob.glob(f'{this_project_source}/**/*.cc', recursive=True) +
                     glob.glob(f'{this_project_source}/**/*.c', recursive=True)):
#                     glob.glob(f'{this_project_source}/../build/*/src/*/*.hpp', recursive=True) +  
#                     glob.glob(f'{this_project_source}/../build/*/src/*/*.cpp', recursive=True) +  
#                     glob.glob(f'{this_project_source}/../build/*/src/*/*.h', recursive=True) +
#                     glob.glob(f'{this_project_source}/../build/*/src/*/*.cc', recursive=True) + 
#                     glob.glob(f'{this_project_source}/../build/*/src/*/*.c', recursive=True)):
        with open(filename, 'r') as fh:
            for line in fh:
                if line.startswith('#include'):
                    all_includes.append(line.strip())
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
    shutil.rmtree(bindings_dir, ignore_errors=True)
    os.mkdir(bindings_dir)
    proj_include = glob.glob("build/*/src/*/include")
    proj_include = " -I".join(proj_include)
    command = (f'{binder_executable} --root-module {python_module_name} '
               f'--prefix {os.getcwd()}/{bindings_dir}/ '
               f'--bind {this_project_namespace_to_bind} '
               + ('--config config.cfg ') +
               f' {all_includes_fn} -- -std=c++14 '
               f'-I{this_project_include} {proj_include} -DNDEBUG -v').split()
    print('BINDER COMMAND:', ' '.join(command))
    subprocess.check_call(command)

def revert_include_changes(changes_made):
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

def main():
    clone_repos()
    os.chdir("binder")
    binder_executable = build_binder()
    os.chdir("..")
    changes_made = clean_includes()
    print(f"made the following changes to source: {changes_made}")
    all_includes_fn = make_all_includes()
    make_bindings_code(all_includes_fn, binder_executable)
    revert_include_changes(changes_made)

if __name__ == '__main__':
    main()
