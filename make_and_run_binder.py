#!/usr/bin/env python3

import glob
import os
import sys
import shutil
import subprocess
import git
from distutils.sysconfig import get_python_inc


# Overall script settings
binder_executable = f''
bindings_dir = 'cmake_bindings'
binder_source = f''
pybind_source = f''
use_pybind_stl = True
#use_pybind_stl = False
this_project_source = f'{os.getcwd()}/src'
this_project_include = f'{os.getcwd()}/include' 
this_project_namespace_to_bind = 'bdsg'
python_module_name = 'bdsg'

def clone_repos():
    if not os.path.exists(f'binder/pybind11'):
        os.mkdir("binder")
        git.Git("binder").clone("https://github.com/RosettaCommons/pybind11.git")
    if not os.path.exists(f'binder/binder'):
        git.Git("binder").clone("https://github.com/RosettaCommons/binder.git")
    pybind_source = f'binder/pybind11/include'
    binder_source = f'binder/binder/source'

def build_binder():
    if not os.path.exists(f'build/*/*/bin/binder'):
        os.system(f'{get_python_inc().split("/")[-1]} binder/binder/build.py')
    binder_executable = glob.glob(f'build/*/*/bin/binder')[0]

def make_all_includes():
    all_includes = []
    all_include_filename = 'all_cmake_includes.hpp'
    for filename in (glob.glob(f'{this_project_source}/**/*.hpp', recursive=True) +
                     glob.glob(f'{this_project_source}/**/*.cpp', recursive=True) +
                     glob.glob(f'{this_project_source}/**/*.h', recursive=True) +
                     glob.glob(f'{this_project_source}/**/*.cc', recursive=True) +
                     glob.glob(f'{this_project_source}/**/*.c', recursive=True)):
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


def make_bindings_code(all_includes_fn):
    shutil.rmtree(bindings_dir, ignore_errors=True)
    os.mkdir(bindings_dir)
    command = (f'{binder_executable} --root-module {python_module_name} '
               f'--prefix {os.getcwd()}/{bindings_dir}/ '
               f'--bind {this_project_namespace_to_bind} '
               + ('--config config.cfg ' if use_pybind_stl else '') +
               f' {all_includes_fn} -- -std=c++14 '
               f'-I{this_project_include} -DNDEBUG -v').split()
    print('BINDER COMMAND:', ' '.join(command))
    subprocess.check_call(command)
    sources_to_compile = []
    with open(f'{bindings_dir}/{python_module_name}.sources', 'r') as fh:
        for line in fh:
            sources_to_compile.append(line.strip())
    return sources_to_compile

def main():
    clone_repos()
    build_binder()
    all_includes_fn = make_all_includes()
    make_bindings_code(all_includes_fn)


if __name__ == '__main__':
    main()
