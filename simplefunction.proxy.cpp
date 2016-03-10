// --------------------------------------------------------------
//
//                        simplefunction.proxy.cpp
//
//        Author: Noah Mendelsohn         
//   
//       This is a hand-crafted demonstration proxy.
//
//       For your project, your "rpcgen" program will generate
//       proxies like this one automatically from the idl
//       file. Note that this proxy also #includes the 
//       simplefunctions.idl file. Of course, your rpcgen
//       program will change that, as well as the number
//       of functions generated. More importantly, it will
//       generate code to handle function arguments and
//       return values.
//
//
//
//       Copyright: 2012 Noah Mendelsohn
//     
// --------------------------------------------------------------

// IMPORTANT! WE INCLUDE THE IDL FILE AS IT DEFINES THE INTERFACES
// TO THE FUNCTIONS WE'RE IMPLEMENTING. THIS MAKES SURE THE
// CODE HERE ACTUALLY MATCHES THE REMOTED INTERFACE

#include "simplefunction.idl"

#include "rpcproxyhelper.h"

#include <cstdio>
#include <cstring>
#include "c150debug.h"

using namespace C150NETWORK;  // for all the comp150 utilities 


void func1() {
  char readBuffer[5];  // to read magic value DONE + null
  //
  // Send the Remote Call
  //
  c150debug->printf(C150RPCDEBUG,"simplefunction.proxy.cpp: func1() invoked");
  RPCPROXYSOCKET->write("func1", strlen("func1")+1); // write function name including null
  //
  // Read the response
  //
  c150debug->printf(C150RPCDEBUG,"simplefunction.proxy.cpp: func1() invocation sent, waiting for response");
  RPCPROXYSOCKET->read(readBuffer, sizeof(readBuffer)); // only legal response is DONE

  //
  // Check the response
  //
  if (strncmp(readBuffer,"DONE", sizeof(readBuffer))!=0) {
    throw C150Exception("simplefunction.proxy: func1() received invalid response from the server");
  }
  c150debug->printf(C150RPCDEBUG,"simplefunction.proxy.cpp: func1() successful return from remote call");

}


void func2() {
  char readBuffer[5];  // to read magic value DONE + null
  //
  // Send the Remote Call
  //
  c150debug->printf(C150RPCDEBUG,"simplefunction.proxy.cpp: func2() invoked");
  RPCPROXYSOCKET->write("func2", strlen("func2")+1); // write function name including null
  //
  // Read the response
  //
  c150debug->printf(C150RPCDEBUG,"simplefunction.proxy.cpp: func2() invocation sent, waiting for response");
  RPCPROXYSOCKET->read(readBuffer, sizeof(readBuffer)); // only legal response is DONE

  //
  // Check the response
  //
  if (strncmp(readBuffer,"DONE", sizeof(readBuffer))!=0) {
    throw C150Exception("simplefunction.proxy: func2() received invalid response from the server");
  }
  c150debug->printf(C150RPCDEBUG,"simplefunction.proxy.cpp: func2() successful return from remote call");

}


void func3() {
  char readBuffer[5];  // to read magic value DONE + null
  //
  // Send the Remote Call
  //
  c150debug->printf(C150RPCDEBUG,"simplefunction.proxy.cpp: func3() invoked");
  RPCPROXYSOCKET->write("func3", strlen("func3")+1); // write function name including null
  //
  // Read the response
  //
  c150debug->printf(C150RPCDEBUG,"simplefunction.proxy.cpp: func3() invocation sent, waiting for response");
  RPCPROXYSOCKET->read(readBuffer, sizeof(readBuffer)); // only legal response is DONE

  //
  // Check the response
  //
  if (strncmp(readBuffer,"DONE", sizeof(readBuffer))!=0) {
    throw C150Exception("simplefunction.proxy: func3() received invalid response from the server");
  }
  c150debug->printf(C150RPCDEBUG,"simplefunction.proxy.cpp: func3() successful return from remote call");

}

