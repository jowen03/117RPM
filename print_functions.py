#!/bin/env python3
#
#          print signatures of all the functions named in supplied IDL file
#

import subprocess
import json
import sys
import os

IDL_TO_JSON_EXECUTABLE = './idl_to_json'

try:
    #
    #     Make sure invoked properly
    #
    assert len(sys.argv) == 2, "Wrong number of arguments"

    #
    #     Make sure IDL file exists and is readable
    #
    filename = sys.argv[1]
    assert os.path.isfile(filename), f"Path {filename} does not designate a file"
    assert os.access(filename, os.R_OK), f"File {filename} is not readable" 

    #
    #     Make sure idl_to_json exists and is executable
    #
    assert os.path.isfile(IDL_TO_JSON_EXECUTABLE), f"Path {IDL_TO_JSON_EXECUTABLE} does not designate a file...run \"make\" to create it" 
    assert os.access(IDL_TO_JSON_EXECUTABLE, os.X_OK), f"File {IDL_TO_JSON_EXECUTABLE} exists but is not executable"

    #
    #     Parse declarations into a Python dictionary
    #
    decls = json.loads(subprocess.check_output([IDL_TO_JSON_EXECUTABLE, filename]))
    print("hi")
    print(json.dumps(decls, indent=4, sort_keys=True))
    #
    # Loop printing each function signature
    #
    for  name, sig in decls["functions"].items():

        # Python List of all args (each is a dictionary with keys "name" and "type")
        args = sig["arguments"]

        # Make a string of form:  "type1 arg1, type2 arg2" for use in function sig
        argstring = ', '.join([a["type"] + ' ' + a["name"] for a in args])

        # print the function signature
        print(f"{sig['return_type']} {name}({argstring})")

except Exception as e:
    print(str(e), file=sys.stderr)
    print(f"Usage: {sys.argv[0]} <idlfilename>", file=sys.stderr)

