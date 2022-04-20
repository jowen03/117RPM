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

def writeBadFunction(stubFile):
    stubFile.write('void __badFunction(char * functionName) {\n')
    stubFile.write('char doneBuffer[4] = "BAD";\n')
    stubFile.write('c150debug->printf(C150RPCDEBUG,"arithmetic.stub.cpp: received call for nonexistent function %s()",functionName);\n')
    stubFile.write('RPCSTUBSOCKET->write(doneBuffer, strlen(doneBuffer)+1);\n')
    stubFile.write('}\n')
    

def writeGetFunctionNameFromStream(stubFile):
    stubFile.write('void getFunctionNameFromStream(char *buffer, unsigned int bufSize) {\n')
    stubFile.write('unsigned int i; char *bufp; bool readnull; ssize_t readlen;\n')
    stubFile.write('readnull = false; bufp = buffer;\n')
    stubFile.write('for (i=0; i< bufSize; i++) {\n')
    stubFile.write('readlen = RPCSTUBSOCKET-> read(bufp, 1);\n')
    stubFile.write('if (readlen == 0) { \nbreak;\n }\n')
    stubFile.write("if (*bufp++ == '\\0') {\nreadnull = true;\nbreak;\n }\n")
    stubFile.write('}\n')
    stubFile.write('if (readlen == 0) {c150debug->printf(C150RPCDEBUG,"stub: read zero length message, checking EOF");\n')
    stubFile.write('if (RPCSTUBSOCKET-> eof()) {c150debug->printf(C150RPCDEBUG,"stub: EOF signaled on input");}\n')
    stubFile.write('else { throw C150Exception("arithmetic.stub: unexpected zero length read without eof");}\n')
    stubFile.write('}\n')
    stubFile.write('else if(!readnull) throw C150Exception("stub: method name not null terminated or too long");\n')
    stubFile.write('}\n')
    
    

def addFunctionToStub(fileBase, stubFile, name, sig):
    returnType = sig["return_type"]
    argString = ', '.join([a["type"] + ' ' + a["name"] for a in sig["arguments"]])
    functionDec = f"void __{name} ({argString}) " + "{"
    stubFile.write(f'{functionDec}\n')
    stubFile.write(f'c150debug->printf(C150RPCDEBUG,"{fileBase}.stub.cpp: invoking {name}()");\n')
    stubFile.write(f'{returnType} ret = {name}(' + ', '.join([a["name"] for a in sig["arguments"]]) + ');\n')
    stubFile.write('string doneBuffer = "DONE " + to_string(ret);\n')
    stubFile.write(f'c150debug->printf(C150RPCDEBUG,"{fileBase}.stub.cpp: returned from {name}() -- responding to client");\n')
    stubFile.write('RPCSTUBSOCKET->write(doneBuffer.c_str(), strlen(doneBuffer.c_str()));\n')
    stubFile.write('}\n')
    stubFile.write('\n')

def addDispatchFunctionToStub(fileBase, stubFile, funcList):
    stubFile.write('void dispatchFunction() {')
    stubFile.write('char functionNameBuffer[20];\n')
    stubFile.write('getFunctionNameFromStream(functionNameBuffer,sizeof(functionNameBuffer));\n')
    stubFile.write('string s(functionNameBuffer); stringstream ss(s); string func_name;\n')
    stubFile.write('ss >> func_name;\n')
    

    # match the function name to the function in the json object
    
    stubFile.write('const char* func_name_cstr = func_name.c_str();')
    stubFile.write('if (!RPCSTUBSOCKET-> eof()) {')
    for name, sig in funcList.items():
        # for each function, calculate num arguments
        # for each argument we'll need a variable for stringstream
        stubFile.write(f'if (strcmp(func_name_cstr,"{name}") == 0)' + '{\n')
        # now make as many string stream inputs as necessary for this function
        for elem in sig["arguments"]:
            # read in each argument then convert it to proper type
            stubFile.write(f'string {elem["name"]}_str;\n')
            stubFile.write(f'ss >> {elem["name"]}_str;\n')
            # naming: {arg_name}_arg for each name in arguments list
            stubFile.write(f'{elem["type"]} {elem["name"]}_arg = stoi({elem["name"]}_str);\n')
        # now call proper function
        stubFile.write(f'__{name}(' + ', '.join(f'{elem["name"]}_arg' for elem in sig["arguments"]) + ');\n')
        stubFile.write('}\n')
    stubFile.write('}\n')
    stubFile.write('}\n')


def buildStub(nameBase, stubName, funcList):

    # NEEDSWORK: include shouldnt be arithmetic should be general
    includes = [
        f'#include \"{nameBase}.idl\"',
        '#include \"rpcstubhelper.h\"',
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
    with open(stubName, 'a') as stub:
        for line in includes:
            stub.write(line)
            stub.write('\n')

        writeBadFunction(stub)
        writeGetFunctionNameFromStream(stub)
        
        for name, sig in funcList.items():
            addFunctionToStub(nameBase, stub, name, sig)
        addDispatchFunctionToStub(nameBase, stub, funcList)
        

    # Add header information (includes)
    # For each function, call addFunctionToStub


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

    nameBase = filename.split('.')[0]
    
    stubName = nameBase + '.stub.cpp'
    buildStub(nameBase, stubName, decls["functions"])

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
