# setup.py: setup script to install `bdsg` Python module
# Adapted from https://stackoverflow.com/a/48015772

import collections
import os
import pathlib
import platform
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
            '--', f'-j{self.cpu_count()}'
        ]

        os.chdir(str(build_temp))
        self.spawn(['cmake', str(cwd)] + cmake_args)
        if not self.dry_run:
            self.spawn(['cmake', '--build', '.'] + build_args)
        # Troubleshooting: if fail on line above then delete all possible 
        # temporary CMake files including "CMakeCache.txt" in top level dir.
        os.chdir(str(cwd))
        
        if platform.system() == 'Darwin':
            # Rewrite Mac library links to be portable, and eliminate library symlinks
            self.portablize_mac_libraries(extdir.absolute())
            
        # TODO: also trim down extra symlink-to-full-library-copies in Linux wheels
            
    def cpu_count(self):
        """
        Get the rounded-up integer number of whole CPUs available.
        Counts hyperthreads as CPUs.
        Uses the system's actual CPU count, or the current v1 cgroup's quota per
        period, if the quota is set.
        Ignores the cgroup's cpu shares value, because it's extremely difficult to
        interpret. See https://github.com/kubernetes/kubernetes/issues/81021.
        
        Adapted from Toil: https://github.com/DataBiosphere/toil/blob/79c4c43c1f76484c99ec550aee609c6579a6a77c/src/toil/lib/threading.py#L118
        
        :return: Integer count of available CPUs, minimum 1.
        :rtype: int
        """

        # Get the fallback answer of all cores (on system or available by affinity
        total_machine_size = len(os.sched_getaffinity(0)) if hasattr(os, 'sched_getaffinity') else os.cpu_count()

        try:
            with open('/sys/fs/cgroup/cpu/cpu.cfs_quota_us', 'r') as stream:
                # Read the quota
                quota = int(stream.read())

            log.debug('CPU quota: %d', quota)

            if quota == -1:
                # Assume we can use the whole machine
                return total_machine_size

            with open('/sys/fs/cgroup/cpu/cpu.cfs_period_us', 'r') as stream:
                # Read the period in which we are allowed to burn the quota
                period = int(stream.read())

            # The thread count is how many multiples of a wall clcok period we can burn in that period.
            cgroup_size = int(math.ceil(float(quota)/float(period)))

        except:
            # We can't actually read these cgroup fields. Maybe we are a mac or something.
            cgroup_size = float('inf')

        # Return the smaller of the actual thread count and the cgroup's limit, minimum 1.
        result = max(1, min(cgroup_size, total_machine_size))
        return result
       
    def eliminate_symlink_chains(self, root_path):
        """
        Given a directory, delete all symlinks in that directory to files in the same directory.
        
        Return a mapping from each symlink's base name to the base name of the file it linked to.
        
        :param pathlib.Path root_path: Absolute path to directory to process
        """
        
        link_to_real_file = {}
        
        for entry in os.scandir(root_path):
            if entry.is_symlink():
                # For each a symlink, get its path as a pathlib Path
                path = pathlib.Path(entry.path)
                # Get the ultimate destination path, and fail if it doesn't exist
                destination = path.resolve(strict=True).absolute()
                   
                if destination.parent == root_path:
                    # If the destination is also in this directory
                    # Remember the link destination
                    link_to_real_file[entry.name] = destination.name
        
        for link in link_to_real_file.keys():
            # Delete the symlink
            fullpath = os.path.join(root_path, link)
            assert os.path.islink(fullpath)
            os.unlink(fullpath)
        
        return link_to_real_file
                
    def portablize_mac_libraries(self, root_path):
        """
        On Mac, CMake builds all our libraries into root_path as *.dylib files,
        plus the main Python module as a .so file. They all name themselves
        (via install_name) and reference each other with paths based on
        "@rpath". This works for loading the module from the current directory,
        but to load from the Python module installation directory, we need to
        change all the linker references to use "@loader_path" instead, so
        they're relative to the file directing the load. We also rewrite the
        install names for good measure.
        
        A complication is that some dylibs are symlinks, which are used to load
        other dylibs. So we map all symlinks to their real destinations when we
        rewrite, and eliminate the symlinks so that we don't ship multiple
        copies of the libraries in the wheel.
        
        :param pathlib.Path root_path:
        """
        
        # Drop all the symlinks
        link_to_real_file = self.eliminate_symlink_chains(root_path)
        
        # Invert that mapping
        real_file_to_links = collections.defaultdict(list)
        for link, real_file in link_to_real_file.items():
            real_file_to_links[real_file].append(link)
        
        # Find all the .so and .dylib files that are not symlinks
        lib_names = [entry.name for entry in os.scandir(root_path)
            if (entry.name.endswith('.dylib') or entry.name.endswith('.so')) and not entry.is_symlink()]
        
        change_options = []
        for lib_name in lib_names:
             # Make all the "-change" options to change references to the libraries, if found
            change_options.append('-change')
            change_options.append(f'@rpath/{lib_name}')
            change_options.append(f'@loader_path/{lib_name}')
            for link in real_file_to_links[lib_name]:
                # Also make change options to rewrite references to the symlink to be to the base file
                change_options.append('-change')
                change_options.append(f'@rpath/{link}')
                change_options.append(f'@loader_path/{lib_name}')
        
        for lib_name in lib_names:
            # For each library
        
            # Apply all changes and set the install name
            self.spawn(['install_name_tool'] + change_options + ['-id', f'@loader_path/{lib_name}', str(os.path.join(root_path, lib_name))])        
        


setup(
    name='bdsg',
    url='https://github.com/vgteam/libbdsg',
    author="vgteam",
    author_email="anovak@soe.ucsc.edu",
    description="Library for BiDirected Sequence Graphs",
    use_scm_version=True, # Just get the version and non-artifact file list from Git
    setup_requires=['setuptools_scm', 'setuptools_git_ls_files'], # Make sure to get everything in Git *and* everything in submodules
    packages=['bdsg'],
    ext_modules=[CMakeExtension('bdsg')],
    cmdclass={
        'build_ext': build_ext,
    }
)
