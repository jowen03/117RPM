// --------------------------------------------------------------
//
//                        structs.proxy.cpp
//
//        Author: Sam Testa, Jake Owen       
//   
//       This is a hand-crafted proxy.
//     
// --------------------------------------------------------------

#include "structs.idl"

#include "rpcproxyhelper.h"

#include <cstdio>
#include <cstring>
#include "c150debug.h"
#include <string>
#include <sstream>
#include <iostream>

using namespace C150NETWORK;

using namespace std;

Person findPerson(ThreePeople tp) {
    cout << "findPerson" << endl;
    char readBuffer[100]; // expected int + null OR non-numeric char = error
    
    //
    // Send the Remote Call
    //
    string p1 = tp.p1.firstname + " " + tp.p1.lastname + to_string(tp.p1.age);
    string p2 = tp.p2.firstname + " " + tp.p2.lastname + to_string(tp.p2.age);
    string p3 = tp.p3.firstname + " " + tp.p3.lastname + to_string(tp.p3.age);

    string func = string("findPerson") + " " + p1 + " " + p2 + " " + p3;
    // NEEDSWORK: waste of space to send two strings but its more readable
    char* func_c_str = const_cast<char*>(func.c_str());        

    c150debug->printf(C150RPCDEBUG,"structs.proxy.cpp: findPerson() invoked");
    RPCPROXYSOCKET->write(func_c_str, strlen(func_c_str)+1); // write function name including null
    cout << "findPerson WRITTEN" << endl;

    /*
     * Read the response
     */
    c150debug->printf(C150RPCDEBUG,"structs.proxy.cpp: findPerson() invocation sent, waiting for response");
    RPCPROXYSOCKET->read(readBuffer, sizeof(readBuffer)); // only legal response is number

    /*
     * Check the response
     */
    string response;
    string firstName;
    string lastName;
    string age;
    string s(readBuffer);
    std::stringstream ss(s);
    ss >> response;
    ss >> firstName;
    ss >> lastName;
    ss >> age;

    if (strncmp(response.c_str(), "DONE", sizeof(response.c_str())) != 0)  {
        throw C150Exception("structs: findPerson() received invalid response from the server");
    }
    c150debug->printf(C150RPCDEBUG,"structs.proxy.cpp: findPerson() successful return from remote call");

    Person responsePerson = {firstName, lastName, atoi(age)};

    return responsePerson;

}

int area(reactangle r) {
    cout << "area invoked" << endl;
    char readBuffer[100];

    string func = string("area") + " " + to_string(r.x) + " " + to_string(r.y);

    char* func_c_str = const_cast<char*>(func.c_str());        

    c150debug->printf(C150RPCDEBUG,"structs.proxy.cpp: area() invoked");
    RPCPROXYSOCKET->write(func_c_str, strlen(func_c_str)+1); // write function name including null
    cout << "area WRITTEN" << endl;

    /*
     * Read the response
     */
    c150debug->printf(C150RPCDEBUG,"structs.proxy.cpp: area() invocation sent, waiting for response");
    RPCPROXYSOCKET->read(readBuffer, sizeof(readBuffer)); // only legal response is number

    /*
     * Check the response
     */
    string response;
    string areaResult;
    string s(readBuffer);
    std::stringstream ss(s);
    ss >> response;
    ss >> areaResult;

    if (strncmp(response.c_str(), "DONE", sizeof(response.c_str())) != 0)  {
        throw C150Exception("structs: area() received invalid response from the server");
    }
    c150debug->printf(C150RPCDEBUG,"structs.proxy.cpp: area() successful return from remote call");
    return atoi(areaResult);
}