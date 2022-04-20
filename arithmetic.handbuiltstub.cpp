#include "arithmetic.idl"

#include "rpcstubhelper.h"

#include <cstdio>
#include <cstring>
#include "c150debug.h"
#include <sstream>
#include <string>
#include <iostream>

using namespace C150NETWORK;  // for all the comp150 utilities 

using namespace std;

void getFunctionNameFromStream(char *buffer, unsigned int bufSize);
void __badFunction(char * functionName);

void __add(int x, int y) {
    //
    // Time to actually call the function 
    //
    c150debug->printf(C150RPCDEBUG,"arithmetic.stub.cpp: invoking add()");
    int sum = add(x, y);
    string doneBuffer = "DONE " + to_string(sum);

    c150debug->printf(C150RPCDEBUG,"arithmetic.stub.cpp: returned from add() -- responding to client");
    RPCSTUBSOCKET->write(doneBuffer.c_str(), strlen(doneBuffer.c_str()));
}

void __subtract(int x, int y) {
    c150debug->printf(C150RPCDEBUG,"arithmetic.stub.cpp: invoking subtract()");
    int diff = subtract(x, y);
    string doneBuffer = "DONE " + to_string(diff);

    c150debug->printf(C150RPCDEBUG,"arithmetic.stub.cpp: returned from subtract() -- responding to client");
    RPCSTUBSOCKET->write(doneBuffer.c_str(), strlen(doneBuffer.c_str()));
}

void __multiply(int x, int y) {
    c150debug->printf(C150RPCDEBUG,"arithmetic.stub.cpp: invoking multiply()");
    int product = multiply(x, y);
    string doneBuffer = "DONE " + to_string(product);

    c150debug->printf(C150RPCDEBUG,"arithmetic.stub.cpp: returned from multiply() -- responding to client");
    RPCSTUBSOCKET->write(doneBuffer.c_str(), strlen(doneBuffer.c_str()));
}

void __divide(int x, int y) {
    c150debug->printf(C150RPCDEBUG,"arithmetic.stub.cpp: invoking divide()");
    int quotient = divide(x, y);
    string doneBuffer = "DONE " + to_string(quotient);

    c150debug->printf(C150RPCDEBUG,"arithmetic.stub.cpp: returned from divide() -- responding to client");
    RPCSTUBSOCKET->write(doneBuffer.c_str(), strlen(doneBuffer.c_str()));
}

void __badFunction(char * functionName) {
  char doneBuffer[4] = "BAD";

  c150debug->printf(C150RPCDEBUG,"arithmetic.stub.cpp: received call for nonexistent function %s()",functionName);
  RPCSTUBSOCKET->write(doneBuffer, strlen(doneBuffer)+1);
}


void dispatchFunction() {
  cout << "In dispatch" << endl;


  char functionNameBuffer[20];

  //
  // Read the function name from the stream -- note
  // REPLACE THIS WITH YOUR OWN LOGIC DEPENDING ON THE 
  // WIRE FORMAT YOU USE
  //
  getFunctionNameFromStream(functionNameBuffer,sizeof(functionNameBuffer));

  cout << "got function name" << endl;

  //
  // We've read the function name, call the stub for the right one
  // The stub will invoke the function and send response.
  //
  string s(functionNameBuffer);
  stringstream ss(s);
  
  string func_name, arg1_str, arg2_str;

  ss >> func_name;
  ss >> arg1_str;
  ss >> arg2_str;

  int arg1 = stoi(arg1_str);
  int arg2 = stoi(arg2_str);

  const char* func_name_cstr = func_name.c_str();

  cout << "FUNCTION INCOMING " << func_name << endl;

  if (!RPCSTUBSOCKET-> eof()) {
    if (strcmp(func_name_cstr,"add") == 0)
        
      __add(arg1, arg2);
    else   if (strcmp(func_name_cstr,"subtract") == 0)
      __subtract(arg1, arg2);
    else   if (strcmp(func_name_cstr,"multiply") == 0)
      __multiply(arg1, arg2);
    else   if (strcmp(func_name_cstr,"divide") == 0)
      __divide(arg1, arg2);
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
  cout << "getFunctionNameFromStream called " << " bufsize " << bufSize << endl;
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
    cout << bufp << endl;
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

  cout << "bufp" << bufp << endl;
  
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