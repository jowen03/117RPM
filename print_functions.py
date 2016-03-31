#!/bin/env python
#
#          print signatures of all the functions named in supplied IDL file
#

import subprocess
import json
import sys
import os


try:
    #
    #     Make sure invoked properly
    #
    if len(sys.argv) != 2:
        raise "Wrong number of arguments"

    #
    #     Make sure file exists and is readable
    #
    filename = sys.argv[1]
    if (not os.path.isfile(filename)):
        print >> sys.stderr, "Path %s does not designate a file" % filename
        raise "No file named " + filename
    if (not os.access(filename, os.R_OK)):
        print >> sys.stderr, "File %s is not readable" % filename
        raise "File " + filename + " not readable"

    #
    #     Parse declarations into a Python dictionary
    #
    decls = json.loads(subprocess.check_output(["idl_to_json", filename]))

    #
    # Loop printing each function signature
    #
    for  name, sig in decls["functions"].iteritems():

        # Python Array of all args (each is a hash with keys "name" and "type")
        args = sig["arguments"]

        # Make a string of form:  "type1 arg1, type2 arg2" for use in function sig
        argstring = ', '.join([a["type"] + ' ' + a["name"] for a in args])

        # print the function signature
        print "%s %s(%s)" % (sig["return_type"], name, argstring)

except Exception as e:
    print >> sys.stderr, "Usage: %s <idlfilename>" % sys.argv[0]

