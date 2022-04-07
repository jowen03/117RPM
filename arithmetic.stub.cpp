#include "arithmetic.idl"

#include "rpcstubhelper.h"

#include <cstdio>
#include <cstring>
#include "c150debug.h"

using namespace C150NETWORK;  // for all the comp150 utilities 

void getFunctionNamefromStream();

void __add(int x, int y) {
    //
    // Time to actually call the function 
    //
    c150debug->printf(C150RPCDEBUG,"arithmetic.stub.cpp: invoking add()");
    int sum = add(x, y)
    string doneBuffer = "DONE " + to_string(sum);

    c150debug->printf(C150RPCDEBUG,"arithmetic.stub.cpp: returned from add() -- responding to client");
    RPCSTUBSOCKET->write(doneBuffer.c_str(), strlen(doneBuffer.c_str()));
}


void dispatchFunction() {


  char functionNameBuffer[80];

  //
  // Read the function name from the stream -- note
  // REPLACE THIS WITH YOUR OWN LOGIC DEPENDING ON THE 
  // WIRE FORMAT YOU USE
  //
  getFunctionNameFromStream(functionNameBuffer,sizeof(functionNameBuffer));

  //
  // We've read the function name, call the stub for the right one
  // The stub will invoke the function and send response.
  //
  stringstream ss(string(functionNameBuffer));
  string func_name, arg1, arg2;

  ss >> func_name;
  ss >> arg1_str;
  ss >> arg2_str;

  int arg1 = stoi(arg1_str);
  int arg2 = stoi(arg2_str);


  if (!RPCSTUBSOCKET-> eof()) {
    if (strcmp(func_name,"add") == 0)
      __add(arg1, arg2);
    else   if (strcmp(func_name,"subtract") == 0)
      __subtract(arg1, arg2);
    else   if (strcmp(func_name,"multiply") == 0)
      __multiply(arg1, arg2);
    else   if (strcmp(func_name,"divide") == 0)
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