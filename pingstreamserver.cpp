// --------------------------------------------------------------
//
//                        pingstreamserver.cpp
//
//        Author: Noah Mendelsohn         
//   
//
//        This is a simple server, designed to illustrate use of:
//
//            * The C150StreamSocket class, which provides 
//              a convenient wrapper for sending and receiving
//              UDP streams in a client/server model
//
//            * The c150debug interface, which provides a framework for
//              generating a timestamped log of debugging messages.
//              Note that the socket classes described above will
//              write to these same logs, providing information
//              about things like when UDP streams are sent and received.
//              See comments section below for more information on 
//              these logging classes and what they can do.
//
//
//        COMMAND LINE
//
//              pingstreamserver 
//
//
//        OPERATION
//
//              pingstreamserver will loop receiving UDP streams from
//              any client. The data in each stream should be a null-
//              terminated string. If it is then the server
//              responds with a text message of its own.
//
//              Note that the C150StreamSocket class will select a UDP
//              port automatically based on the users login, so this
//              will (typically) work only on the test machines at Tufts
//              and for COMP 150-IDS who are registered. See documention
//              for getUserPort.
//
//
//       Copyright: 2012 Noah Mendelsohn
//     
// --------------------------------------------------------------

#include "c150streamsocket.h"
#include "c150debug.h"
#include <fstream>
#include <cstdlib> 


using namespace C150NETWORK;  // for all the comp150 utilities 

void setUpDebugLogging(const char *logname, int argc, char *argv[]);


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
//
//                           main program
//
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
 
int
main(int argc, char *argv[])
{

     //
     // Variable declarations
     //
     ssize_t readlen;             // amount of data read from socket
     char incomingMessage[512];   // received message data

     //
     // Check command line and parse arguments
     //
     if (argc != 1)  {
       fprintf(stderr,"Correct syntxt is: %s\n", argv[0]);
          exit(1);
     }
     //
     //  Set up debug message logging
     //
     setUpDebugLogging("pingstreamserverdebug.txt",argc, argv);

     //
     // We set a debug output indent in the server only, not the client.
     // That way, if we run both programs and merge the logs this way:
     //
     //    cat pingstreamserverdebug.txt pingstreamserverclient.txt | sort
     //
     // it will be easy to tell the server and client entries apart.
     //
     // Note that the above trick works because at the start of each
     // log entry is a timestamp that sort will indeed arrange in 
     // timestamp order, thus merging the logs by time across 
     // server and client.
     //
     c150debug->setIndent("    ");              // if we merge client and server
                                                // logs, server stuff will be indented

     //
     // Create socket and set it to listen, establishing us as a server
     //
     //  Note: the comp150 framework is a little tricky. Under the covers,
     //  there are multiple sockets...one to listen, and one created dynamically
     //  on each accept. The framework hides that, dealing with just
     //  one client at a time, all through the same C150StreamSocket object.
     //  This isn't as flexible as it might be, but it's simple, and it
     //  maximizes compatibility with the UDP (dgm) framework
     //
     try {
       c150debug->printf(C150APPLICATION,"Creating C150StreamSocket");
       C150StreamSocket *sock = new C150StreamSocket();

       c150debug->printf(C150APPLICATION,"Calling C150StreamSocket::listen");
       sock -> listen();


       c150debug->printf(C150APPLICATION,"Ready to accept messages");
       //
       // Infinite loop accepting connections
       //
       while (1) {

	 //
	 // We'll hang here until another client asks to connect
	 // When we drop through, sock will talk to that one
	 //
         c150debug->printf(C150APPLICATION,"Calling C150StreamSocket::accept");
         sock -> accept();
	 
       
         //
         // infinite loop processing messages
         //
         while(1)	{
  
  	  //
            // Read a message from the stream
  	  // -1 in size below is to leave room for null
  	  //
  	  readlen = sock -> read(incomingMessage, sizeof(incomingMessage)-1);
  
  	  //
  	  // With TCP streams, we should never get a 0 length read
  	  // except with timeouts (which we're not setting in pingstreamserver)
  	  // or EOF
  	  //
  	  if (readlen == 0) {
  	    c150debug->printf(C150APPLICATION,"Read zero length message, checking EOF");
  	    if (sock -> eof()) {
  	       c150debug->printf(C150APPLICATION,"EOF signaled on input");
  	       break;
  	    } else {
  	      cerr << "pingstreamserver: unexpected 0 length read without EOF. Giving up!" << endl;
  	      return 8;
  	    }
  	  }
  
  	  //
  	  // Clean up the message in case it contained junk
  	  //
  	  incomingMessage[readlen] = '\0'; // make sure null terminated
  	  string incoming(incomingMessage); // Convert to C++ string ...it's slightly
  	                                    // easier to work with, and cleanString
  	                                    // expects it
  	  cleanString(incoming);            // c150ids-supplied utility: changes
  	                                    // non-printing characters to .
            c150debug->printf(C150APPLICATION,"Successfully read %d bytes. Message=\"%s\"",
  			    readlen, incoming.c_str());
  
  	  //
  	  //  create the message to return
  	  // 
  	  string response = "You said " + incoming;
  	  if (incoming=="ping" || incoming == "Ping" || incoming == "PING") 
  	    response += " and I say PONG. Thank you for playing!";
  	  else
  	    response += ". Don't you know how to play ping pong?";
  
  	  //
  	  // write the return message
  	  //
  	  c150debug->printf(C150APPLICATION,"Responding with message=\"%s\"",
  			    response.c_str());
  	  sock -> write(response.c_str(), response.length()+1);
	 }

	 //
	 // Done looping on this connection, close and wait for another
	 //
         c150debug->printf(C150APPLICATION,"Calling C150StreamSocket::close");
	 sock -> close();
       }
     }

     catch (C150NetworkException& e) {
       // Write to debug log
       c150debug->printf(C150ALWAYSLOG,"Caught C150NetworkException: %s\n",
			 e.formattedExplanation().c_str());
       // In case we're logging to a file, write to the console too
       cerr << argv[0] << ": caught C150NetworkException: " << e.formattedExplanation() << endl;
     }

     // This only executes if there was an error caught above
     return 4;
}



// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
//
//                     setUpDebugLogging
//
//        For COMP 150-IDS, a set of standards utilities
//        are provided for logging timestamped debug messages.
//        You can use them to write your own messages, but 
//        more importantly, the communication libraries provided
//        to you will write into the same logs.
//
//        As shown below, you can use the enableLogging
//        method to choose which classes of messages will show up:
//        You may want to turn on a lot for some debugging, then
//        turn off some when it gets too noisy and your core code is
//        working. You can also make up and use your own flags
//        to create different classes of debug output within your
//        application code
//
//        NEEDSWORK: should be factored and shared w/pingstreamclient
//        NEEDSWORK: document arguments
//
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
 
void setUpDebugLogging(const char *logname, int argc, char *argv[]) {

     //   
     //           Choose where debug output should go
     //
     // The default is that debug output goes to cerr.
     //
     // Uncomment the following three lines to direct
     // debug output to a file. Comment them to 
     // default to the console
     //  
     // Note: the new DebugStream and ofstream MUST live after we return
     // from setUpDebugLogging, so we have to allocate
     // them dynamically.
     //
     //
     // Explanation: 
     // 
     //     The first line is ordinary C++ to open a file
     //     as an output stream.
     //
     //     The second line wraps that will all the services
     //     of a comp 150-IDS debug stream, and names that filestreamp.
     //
     //     The third line replaces the global variable c150debug
     //     and sets it to point to the new debugstream. Since c150debug
     //     is what all the c150 debug routines use to find the debug stream,
     //     you've now effectively overridden the default.
     //
     ofstream *outstreamp = new ofstream(logname);
     DebugStream *filestreamp = new DebugStream(outstreamp);
     DebugStream::setDefaultLogger(filestreamp);


     //
     //  Put the program name and a timestamp on each line of the debug log.
     //
     c150debug->setPrefix(argv[0]);
     c150debug->enableTimestamp(); 

     //
     // Ask to receive all classes of debug message
     //
     // See c150debug.h for other classes you can enable. To get more than
     // one class, you can or (|) the flags together and pass the combined
     // mask to c150debug -> enableLogging 
     //
     // By the way, the default is to disable all output except for
     // messages written with the C150ALWAYSLOG flag. Those are typically
     // used only for things like fatal errors. So, the default is
     // for the system to run quietly without producing debug output.
     //
     c150debug->enableLogging(C150APPLICATION | C150NETWORKTRAFFIC | 
			      C150NETWORKDELIVERY); 

}

