#!/usr/bin/env python3

"""
Generate RST using Sphinx AutoDoc. See <https://stackoverflow.com/a/31648880>
"""

import argparse
import os
import tempfile
import shutil
import sys

import sphinx.cmd.build
import sphinx.ext.autodoc


def parse_args(args):
    """
    Takes in the command-line arguments list (args), and returns a nice argparse
    result with fields for all the options.
    
    Borrows heavily from the argparse documentation examples:
    <http://docs.python.org/library/argparse.html>
    """

    # Construct the parser (which is stored in parser)
    # Module docstring lives in __doc__
    # See http://python-forum.com/pythonforum/viewtopic.php?f=3&t=36847
    # And a formatter class so our examples in the docstring look good. Isn't it
    # convenient how we already wrapped it to 80 characters?
    # See http://docs.python.org/library/argparse.html#formatter-class
    parser = argparse.ArgumentParser(description=main.__doc__,
        formatter_class=argparse.RawDescriptionHelpFormatter)
        
    parser.add_argument("source_file",
        help="Source file (.rst with autodoc) to read")
    parser.add_argument("out_file", type=argparse.FileType('w'),
        help="Output file (.rst without autodoc) to generate")
        
    return parser.parse_args(args)
    
def patch_sphinx(line_callback):
    """
    Monkey-patch Sphinx so that each generated RST line is passed to the given callback.
    Includes trailing newelines.
    """
    
    orig_add_line = sphinx.ext.autodoc.Documenter.add_line
    
    def add_line(self, line, source, *lineno):
        """Append one line of generated reST to the output."""
        line_callback(line + '\n')
        orig_add_line(self, line, source, *lineno)
        
    sphinx.ext.autodoc.Documenter.add_line = add_line

def main(argv):
    """
    Generate docs as instructed on the command line.
    """

    options = parse_args(argv[1:])
    
    # Send all Sphinx lines to the output file
    patch_sphinx(lambda line: options.out_file.write(line))
    
    # Create a temporary output directory to force a rebuild.
    tempdir = tempfile.mkdtemp()
    
    # Invoke Sphinx's main, which returns an exit code
    try:
        sphinx_code = sphinx.cmd.build.main(['-b', 'html', '-d', os.path.join(tempdir, 'doctrees'), '.', os.path.join(tempdir, 'html'), options.source_file])
    finally:
        shutil.rmtree(tempdir)
        
    # Close the file properly.
    # Otherwise we somehow lose data.
    options.out_file.close()
        
    return sphinx_code
    
if __name__ == "__main__":
    sys.exit(main(sys.argv))
    
    
