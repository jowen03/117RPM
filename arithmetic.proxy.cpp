// --------------------------------------------------------------
//
//                        arithmetic.proxy.cpp
//
//        Author: Sam Testa, Jake Owen       
//   
//       This is a hand-crafted proxy.
//     
// --------------------------------------------------------------

#include "arithmetic.idl"

#include "rpcproxyhelper.h"

#include <cstdio>
#include <cstring>
#include "c150debug.h"

using namespace C150NETWORK;

int add(int x, int y) {
    char readBuffer[7]; // expected int + null OR non-numeric char = error
    
    //
    // Send the Remote Call
    //
    string func = "add" + " " + to_string(x) + " " + to_string(y);
        

    c150debug->printf(C150RPCDEBUG,"arithmetic.proxy.cpp: add() invoked");
    RPCPROXYSOCKET->write(func.c_str(), strlen(func)+1); // write function name including null

    /*
     * Read the response
     */
    c150debug->printf(C150RPCDEBUG,"arithmetic.proxy.cpp: add() invocation sent, waiting for response");
    RPCPROXYSOCKET->read(readBuffer, sizeof(readBuffer)); // only legal response is number

    /*
     * Check the response
     */
    string response, num;
    stringstream ss << readBuffer;
    ss >> response;
    ss >> num;

    if (strncmp(response, "DONE", sizeof(response)) != 0)  {
        throw C150Exception("arithmetic: add() received invalid response from the server");
    }
    c150debug->printf(C150RPCDEBUG,"arithmetic.proxy.cpp: add() successful return from remote call");

}