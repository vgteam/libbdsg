# setup.py: setup script to install `bdsg` Python module
# Adapted from https://stackoverflow.com/a/48015772

import os
import pathlib
import sys

from setuptools import setup, Extension
from setuptools.command.build_ext import build_ext as build_ext_orig


class CMakeExtension(Extension):

    def __init__(self, name):
        # don't invoke the original build_ext for this special extension
        super().__init__(name, sources=[])


class build_ext(build_ext_orig):

    def run(self):
        for ext in self.extensions:
            self.build_cmake(ext)
        super().run()

    def build_cmake(self, ext):
        cwd = pathlib.Path().absolute()

        # these dirs will be created in build_py, so if you don't have
        # any python sources to bundle, the dirs will be missing
        build_temp = pathlib.Path(self.build_temp)
        build_temp.mkdir(parents=True, exist_ok=True)
        # Our extension is a module path and not a directory
        extlib = pathlib.Path(self.get_ext_fullpath(ext.name))
        extdir = extlib.parent
        extdir.mkdir(parents=True, exist_ok=True)

        config = 'Debug' if self.debug else 'Release'
        cmake_args = [
            '-DRUN_DOXYGEN=OFF',
            f'-DPYTHON_EXECUTABLE={sys.executable}',
            '-DCMAKE_LIBRARY_OUTPUT_DIRECTORY=' + str(extdir.absolute()),
            '-DCMAKE_BUILD_TYPE=' + config
        ]

        build_args = [
            '--config', config,
            '--', '-j16'
        ]

        os.chdir(str(build_temp))
        self.spawn(['cmake', str(cwd)] + cmake_args)
        if not self.dry_run:
            self.spawn(['cmake', '--build', '.'] + build_args)
        # Troubleshooting: if fail on line above then delete all possible 
        # temporary CMake files including "CMakeCache.txt" in top level dir.
        os.chdir(str(cwd))


setup(
    name='bdsg',
    version='0.1',
    packages=['bdsg'],
    ext_modules=[CMakeExtension('bdsg')],
    cmdclass={
        'build_ext': build_ext,
    }
)
