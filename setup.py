import os
from setuptools import setup, Extension
from Cython.Build import cythonize # pylint: disable=import-error

with open('README.md') as fh:
    LONG_DESCRIPTION = fh.read()

def find_all_c_files(given_dir, exclude=[]):
    c_file_list = []
    for i in os.listdir(given_dir):
        if i.find('c') > 0 and exclude.count(i) == 0:
            c_file_list.append(os.path.join(given_dir, i))
    return c_file_list
    
def set_up_cython_extension():
    extra_include_path = []
    extra_include_path.append(os.path.join(os.getcwd()))


    # collect library
    sourcefiles = ['pygncd.pyx']
    sourcefiles.extend(find_all_c_files(os.path.join(os.getcwd(), 'gn'), exclude='readgml.c'))
    extensions = [
        Extension('pygncd', sourcefiles,
                  include_dirs=extra_include_path
                 )
    ]
    return cythonize(extensions)

EXT_MODULE_CLASS = set_up_cython_extension()

setup(
    name='pygncd',
    version='0.1', # different with C++ lib version
    ext_modules=EXT_MODULE_CLASS,
    author="zhaofeng-shu33",
    install_requires=['networkx'],
    author_email="616545598@qq.com",
    description="a hierachical community detection algorithm by Girvan Newman",
    url="https://github.com/zhaofeng-shu33/Girvan-Newman",
    long_description=LONG_DESCRIPTION,
    long_description_content_type="text/markdown",
    classifiers=[
        "Programming Language :: Python :: 3",
    ],
    license="Apache License Version 2.0",
)