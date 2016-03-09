// --------------------------------------------------------------
//
//                        idldeclarationtst.cpp
//
//        Author: Noah Mendelsohn
//
//        Syntax:
//
//               idldeclarationtst idlfile1 [idlfile...]*
//  
//        This program reads each file named on the command line
//        and parses the file into a Declarations parse tree.
//        The results are then formatted on standard output for demonstration
//        purposes.
//
//        INTENDED USE:
//
//        In addition to being somewhat useful for exploring IDL files, this
//        code is intended to illustrate use of the data structures comprising
//        the parse tree.
//
//        NEEDSWORK:
//
//        This test program and the parsing framework were written in some haste.
//        There will likely be some problems especially with edge cases.
//
//        It's known that the parsing framework does not free dynamically
//        allocated structures -- valgrind will report leaks.
//
//       Copyright: 2012 Noah Mendelsohn
//     
// --------------------------------------------------------------



#include <fstream>
#include <sstream>
#include <iostream>
#include <cstdio>
#include "c150exceptions.h"
#include "declarations.h"
#include "functiondeclaration.h"
#include "typedeclaration.h"

using namespace std;

void
processIDLFile(const char fileName[]);


// --------------------------------------------------------------
//
//                       main
//
//          Loop through all files calling processIDLFile on each
//
// --------------------------------------------------------------

int 
main(int argc, char *argv[])
{
    int argnum;

    //
    // Check argument count
    //
    if (argc < 2) {
      fprintf(stderr, "Syntax: %s  idlfile1 [idlfile...]*\n", argv[0]);
      exit (8);
    };

    //
    //  Loop through arguments, processing each named file
    //

    for (argnum = 1; argnum < argc; argnum ++) {

      try {
	processIDLFile(argv[argnum]);
      } 

      catch (C150Exception e) {
	//
	// If any of the exceptions that are created using the COMP 150
	// framework are thrown, they come with a detailed explanation.
	// Print it, then give up.
	//
	fprintf(stderr,"%s: caught C150Exception: %s\n", argv[0],  
		e.formattedExplanation().c_str());
	printf("...Giving up on file %s...",argv[argnum]);
	if (argnum< (argc-1)) 
	  printf("trying next file...\n\n"
		 "-----------------------------------------\n\n");
      }
    }
    
    printf("\n\n");   // some diagnostic output may not have ended with linefeed

    return 0;
}

// -----------------------------------------------------
//                  processIDLFile
//
//  Use the IDLTokenizer class to tokenize one file.
//  Then format the results on stdout for diagnostic
//  purposes.
//
//  THIS IS WHERE YOU SHOULD LOOK TO LEARN THE PARSE
//  TREE DATA STRUCTURES, BECAUSE THE PRINTOUT ROUTINES
//  BELOW SHOW YOU HOW TO WALK THROUGH THE WHOLE PARSE
//  TREE. ALSO CHECK .H FILES LIKE functiondeclaration.h.
//
// -----------------------------------------------------

void
processIDLFile(const char fileName[]) {

  unsigned int argnum;


  //
  // Open the file
  //
  ifstream idlFile(fileName);        // open 

  if (!idlFile.is_open()) {
    stringstream ss;
    ss << "Could not open IDL file: " << fileName;
    throw C150Exception(ss.str());
  }

  //
  //   Call the IDL parser to parse the file
  //   into typed tokens
  //

  printf("\n\n********************************************************\n");
  printf(    "              File: %-25s            \n",fileName);
  printf(    "********************************************************\n");


  printf("\nParsing Declarations(%s)\n", fileName);

  // The following line does all the work parsing the file into
  // the variable parseTree

  Declarations parseTree(idlFile);
  printf("Done Parsing Declarations\n");

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
  //         Print out all Types
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

  printf("\n*------------------------------------------------------*\n");
  printf(    "|                    TYPES                             |\n");
  printf(    "*------------------------------------------------------*\n");
  printf("\n\nNumber of types=%d\n", (int)parseTree.types.size());
  
  //
  //  Loop once for each type declaration
  //
  //
  // The C++ iterator stuff over maps is a mess. 
  // The following shows you how to go through every type
  // regardless of name.
  //
  //  You could also reference a particular type with
  //
  //      parseTree.types["yourtypename"]
  //
  //  First, you should see whether the type exists by calling: 
  //
  //      parseTree.typeExists["yourtypename"]
  //

  // This iterator will return pairs of <string, TypeDeclaration*>
  // If this doesn't make sense, look on the Web for information
  // about iterating over C++ maps. 

  std::map<std::string, TypeDeclaration*>::iterator iter;  

  TypeDeclaration *typep;

  stringstream formattedType;

  for (iter = parseTree.types.begin(); iter != parseTree.types.end(); ++iter) {

    //
    // Iter actually is a pair, with members first (the key) and
    // second (the value) from the map. Our keys are the type
    // names, and the values are the actual type declaration structures.
    //
    // iter -> second gets you the value, so that's what we want
    //
    // We can actually get the type name from two places:
    // From the map at iter -> first, or from the TypeDeclaration
    // itself by doing typep->getName()  
    // 
    typep = iter -> second;

    // OK -- HERE IS WHERE YOU HAVE THE TYPE NAME

    formattedType.str("");      // empty the formatting buffer

    typep->to_string_stream(formattedType);   // format the type info into the buffer

    printf("\n%s",formattedType.str().c_str());  // write the output
  } 

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
  //         Print out all Functions
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 


  printf("\n\n*------------------------------------------------------*\n");
  printf(    "|                   FUNCTIONS                          |\n");
  printf(    "*------------------------------------------------------*\n");
  printf("\n\nNumber of functions=%d\n", (int)parseTree.functions.size());
  
  //
  //  Loop once for each function declaration
  //
  //  You could also reference a particular function with
  //
  //      parseTree.functions["yourfunctionname"]
  //
  //  First, you should see whether the type exists by calling: 
  //
  //      parseTree.functionExists["yourfunctionname"]
  //



  // This iterator will return pairs of <string, FunctionDeclaration*>
  std::map<std::string, FunctionDeclaration*>::iterator fiter;  

  FunctionDeclaration *functionp;    // Declare FunctionDelcaration pointer

  //  ArgumentVector args;               // We'll hold a copy of the parameter
                                     // list here

  for (fiter = parseTree.functions.begin(); fiter != parseTree.functions.end(); 
       ++fiter) {

    //
    // Fiter actually is a pair, with members first (the key) and
    // second (the value) from the map. Our keys are the function
    // names, and the values are the actual function declaration structures.
    // 
    functionp = fiter -> second;

    printf("\nFunction name: %s\n", functionp->getName().c_str());


    //
    // Get a C++ reference to the vector with the argument list
    //
    ArgumentVector& args = functionp -> getArgumentVector();

    printf("  Return type: %s\n\n",functionp->getReturnType()->getName().c_str());
    printf("  Number of arguments: %d\n\n",(int)args.size());

    //
    // The arguments are in a vector, not a map, so we
    // access them by position, not name
    //
    for(argnum=0; argnum<args.size();argnum++) {
      Arg_or_Member_Declaration* argp = args[argnum];
      printf("  ARGUMENT: name=%s type=%s\n",
	     argp->getName().c_str(),
	     argp->getType()->getName().c_str());
    }

    printf("\n\n");                    // separate functions w/ space

  } 




}
