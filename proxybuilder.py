#!/bin/env python3
#
#          print signatures of all the functions named in supplied IDL file
#

import profile
import subprocess
import json
import sys
import os

IDL_TO_JSON_EXECUTABLE = './idl_to_json'

def addFunctionToProxy(proxyFile, name, sig):
    returnType = sig["return_type"]
    argString = "("
    argstring = ', '.join([a["type"] + ' ' + a["name"] for a in sig["arguments"]])
    functionDecl = f"{returnType} {name} ({argstring}) " + "{" 
    proxyFile.write(functionDecl)
    proxyFile.write('\n')

    # write the body of the proxy function
    # NEEDSWORK: dynamically calculating buffer size would allow for more general cases
    proxyFile.write('char readBuffer[7];\n')
    proxyFile.write(f'string func = string("{name}") + " " + to_string(x) + " " + to_string(y);\n')
    proxyFile.write('char* func_c_str = const_cast<char*>(func.c_str());\n')
    proxyFile.write(f'c150debug->printf(C150RPCDEBUG,"arithmetic.proxy.cpp: {name}() invoked");\n')
    proxyFile.write('RPCPROXYSOCKET->write(func_c_str, strlen(func_c_str)+1);\n')
    proxyFile.write(f'cout << "{name} WRITTEN" << endl;\n')

    ## read the response in proxy
    proxyFile.write(f'c150debug->printf(C150RPCDEBUG,"arithmetic.proxy.cpp: {name}() invocation sent, waiting for response");\n')
    proxyFile.write('RPCPROXYSOCKET->read(readBuffer, sizeof(readBuffer));\n')

    ## check the response
    proxyFile.write('string response;\n')
    proxyFile.write('string num;\n')
    proxyFile.write('string s(readBuffer);\n')
    proxyFile.write('std::stringstream ss(s);\n')
    proxyFile.write('ss >> response;\n')
    proxyFile.write('ss >> num;\n')

    proxyFile.write('if (strncmp(response.c_str(), "DONE", sizeof(response.c_str())) != 0)  {\n')
    proxyFile.write(f'throw C150Exception("arithmetic: {name}() received invalid response from the server");\n')
    proxyFile.write('}\n')
    proxyFile.write(f'c150debug->printf(C150RPCDEBUG,"arithmetic.proxy.cpp: {name}() successful return from remote call");\n')
    proxyFile.write('return atoi(num.c_str());\n')
    proxyFile.write('}\n\n\n')




def buildProxy(proxyName, funcList):

    includes = [
        '#include \"arithmetic.idl\"',
        '#include \"rpcproxyhelper.h\"',
        '#include <cstdio>',
        '#include <cstring>',
        '#include \"c150debug.h\"',
        '#include <string>',
        '#include <sstream>',
        '#include <iostream>',
        'using namespace std;',
        'using namespace C150NETWORK;'
    ]

    # simple includes and setup
    with open(proxyName, 'a') as proxy:
        for line in includes:
            proxy.write(line)
            proxy.write('\n')
        
        for name, sig in funcList.items():
            addFunctionToProxy(proxy, name, sig)

    # Add header information (includes)
    # For each function, call addFunctionToProxy


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
    
    proxyName = filename.split('.')[0] + '.proxy.cpp'
    buildProxy(proxyName, decls["functions"])

    # #
    # # Loop printing each function signature
    # #
    # for  name, sig in decls["functions"].items():

    #     # Python List of all args (each is a dictionary with keys "name" and "type")
    #     args = sig["arguments"]

    #     # Make a string of form:  "type1 arg1, type2 arg2" for use in function sig
    #     argstring = ', '.join([a["type"] + ' ' + a["name"] for a in args])

    #     # print the function signature
    #     print(f"{sig['return_type']} {name}({argstring})")

except Exception as e:
    print(str(e), file=sys.stderr)
    print(f"Usage: {sys.argv[0]} <idlfilename>", file=sys.stderr)
