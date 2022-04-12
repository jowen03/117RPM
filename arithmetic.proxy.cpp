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
#include <string>
#include <sstream>
#include <iostream>

using namespace C150NETWORK;

using namespace std;

int add(int x, int y) {
    cout << "ADD CALLED" << endl;
    char readBuffer[7]; // expected int + null OR non-numeric char = error
    
    //
    // Send the Remote Call
    //
    string func = string("add") + " " + to_string(x) + " " + to_string(y);
    // NEEDSWORK: waste of space to send two strings but its more readable
    char* func_c_str = const_cast<char*>(func.c_str());

    cout << "add size: " << sizeof(func_c_str) << endl;
        

    c150debug->printf(C150RPCDEBUG,"arithmetic.proxy.cpp: add() invoked");
    RPCPROXYSOCKET->write(func_c_str, strlen(func_c_str)+1); // write function name including null
    cout << "ADD WRITTEN" << endl;

    /*
     * Read the response
     */
    c150debug->printf(C150RPCDEBUG,"arithmetic.proxy.cpp: add() invocation sent, waiting for response");
    RPCPROXYSOCKET->read(readBuffer, sizeof(readBuffer)); // only legal response is number

    /*
     * Check the response
     */
    string response;
    string num;
    string s(readBuffer);
    std::stringstream ss(s);
    ss >> response;
    ss >> num;

    if (strncmp(response.c_str(), "DONE", sizeof(response.c_str())) != 0)  {
        throw C150Exception("arithmetic: add() received invalid response from the server");
    }
    c150debug->printf(C150RPCDEBUG,"arithmetic.proxy.cpp: add() successful return from remote call");

    return atoi(num.c_str());

}

int subtract(int x, int y) {


    return x - y;

}

int multiply(int x, int y) {
    return x * y;
}

int divide(int x, int y) {
    return x / y;
}