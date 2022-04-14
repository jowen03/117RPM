#include <string>
using namespace std;

#include "structs.idl"

#include "rpcstubhelper.h"

#include <cstdio>
#include <cstring>
#include "c150debug.h"
#include <sstream>
#include <iostream>

using namespace C150NETWORK;  // for all the comp150 utilities 


void getFunctionNameFromStream(char *buffer, unsigned int bufSize);
void __badFunction(char * functionName);

void __findPerson(ThreePeople tp) {
    c150debug->printf(C150RPCDEBUG,"structs.stub.cpp: invoking findPerson()");
    Person p = findPerson(tp);
    string doneBuffer = "DONE " + p.firstname + " " + p.lastname + " " + to_string(p.age);
    c150debug->printf(C150RPCDEBUG,"structs.stub.cpp: returned from findPerson() -- responding to client");
    RPCSTUBSOCKET->write(doneBuffer.c_str(), strlen(doneBuffer.c_str()));
}

void __area(rectangle r) {
    c150debug->printf(C150RPCDEBUG,"structs.stub.cpp: invoking area()");
    int product = area(r);
    string doneBuffer = "DONE " + to_string(product);
    c150debug->printf(C150RPCDEBUG,"structs.stub.cpp: returned from area() -- responding to client");
    RPCSTUBSOCKET->write(doneBuffer.c_str(), strlen(doneBuffer.c_str())); 
}

void __badFunction(char * functionName) {
    char doneBuffer[4] = "BAD";

    c150debug->printf(C150RPCDEBUG,"arithmetic.stub.cpp: received call for nonexistent function %s()",functionName);
    RPCSTUBSOCKET->write(doneBuffer, strlen(doneBuffer)+1);
}

void dispatchFunction() {
    char functionNameBuffer[100];
    getFunctionNameFromStream(functionNameBuffer,sizeof(functionNameBuffer));
    
    string s(functionNameBuffer);
    stringstream ss(s);
  
    string func_name;
    ss >> func_name;

    const char* func_name_cstr = func_name.c_str();
    if (!RPCSTUBSOCKET-> eof()) {
        if (strcmp(func_name_cstr,"findPerson") == 0) {
            string p1fn, p1ln, p1a, p2fn, p2ln, p2a, p3fn, p3ln, p3a;
            ss >> p1fn;
            ss >> p1ln;
            ss >> p1a;
            ss >> p2fn;
            ss >> p2ln;
            ss >> p2a;
            ss >> p3fn;
            ss >> p3ln;
            ss >> p3a;
            Person p1 = {p1fn, p1ln, stoi(p1a)};
            Person p2 = {p2fn, p2ln, stoi(p2a)};
            Person p3 = {p3fn, p3ln, stoi(p3a)};
            ThreePeople tp = {p1, p2, p3};
            __findPerson(tp);
        }
        else   if (strcmp(func_name_cstr,"area") == 0) {
            string x, y;
            ss >> x;
            ss >> y;
            rectangle r = {stoi(x), stoi(y)};
            __area(r);
        }
        else
        __badFunction(functionNameBuffer);
    }
}


//
//                   getFunctionNamefromStream
//
//   Helper routine to read function name from the stream. 
//   Note that this code is the same for all stubs, so can be generated
//   as boilerplate.
//
//   Important: this routine must leave the sock open but at EOF
//   when eof is read from client.
//
void getFunctionNameFromStream(char *buffer, unsigned int bufSize) {
  unsigned int i;
  char *bufp;    // next char to read
  bool readnull;
  ssize_t readlen;             // amount of data read from socket
  
  //
  // Read a message from the stream
  // -1 in size below is to leave room for null
  //
  readnull = false;
  bufp = buffer;
  for (i=0; i< bufSize; i++) {
    readlen = RPCSTUBSOCKET-> read(bufp, 1);  // read a byte
    // check for eof or error
    if (readlen == 0) {
      break;
    }
    // check for null and bump buffer pointer
    if (*bufp++ == '\0') {
      readnull = true;
      break;
    }
  }

  
  //
  // With TCP streams, we should never get a 0 length read
  // except with timeouts (which we're not setting in pingstreamserver)
  // or EOF
  //
  if (readlen == 0) {
    c150debug->printf(C150RPCDEBUG,"arithmetic.stub: read zero length message, checking EOF");
    if (RPCSTUBSOCKET-> eof()) {
      c150debug->printf(C150RPCDEBUG,"arithmetic.stub: EOF signaled on input");

    } else {
      throw C150Exception("arithmetic.stub: unexpected zero length read without eof");
    }
  }

  //
  // If we didn't get a null, input message was poorly formatted
  //
  else if(!readnull) 
    throw C150Exception("arithmetic.stub: method name not null terminated or too long");

  
  //
  // Note that eof may be set here for our caller to check
  //

}