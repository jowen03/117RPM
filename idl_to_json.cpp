// --------------------------------------------------------------
//
//                        idl_to_json.cpp
//
//        Author: Noah Mendelsohn
//
//        Syntax:
//
//               idldeclarationtst idlfile1
//  
//        This program reads the file named on the command line
//        and parses the file into a Declarations parse tree.
//        The results are then formatted on standard output as JSON.
//
//        NEEDSWORK:
//
//        This test program and the parsing framework were written in some haste.
//        There will likely be some problems especially with edge cases.
//
//        It's known that the parsing framework does not free dynamically
//        allocated structures -- valgrind will report leaks.
//
//       Copyright: 2012 & 2016 Noah Mendelsohn
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

const int INDENT = 4;
void indent_it(int ind);
void print_arglist_or_members(vector<Arg_or_Member_Declaration *>& members, 
			      int indent);
void print_function(FunctionDeclaration *functionp, int indent, 
		    bool writeCommaFunction);
void print_type_of_type(const char * type_of_type);

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
//    int argnum;

    //
    // Check argument count
    //
    if (argc != 2) {
      fprintf(stderr, "Syntax: %s  idlfile1\n", argv[0]);
      exit (8);
    };

    //
    //  Process the named file
    //


    try {
	    processIDLFile(argv[1]);
    } 

    catch (C150Exception e) {
	    //
	    // If any of the exceptions that are created using the COMP 150
	    // framework are thrown, they come with a detailed explanation.
	    // Print it, then give up.
	    //
	    fprintf(stderr,"%s: caught C150Exception: %s\n", argv[0],  
		    e.formattedExplanation().c_str());
	    printf("...Giving up on file %s...",argv[1]);
    }
    

    return 0;
}

// -----------------------------------------------------
//                  processIDLFile
//
//  Use the IDLTokenizer class to tokenize one file.
//  Then format the JSON results on stdout.
//
// -----------------------------------------------------

void
processIDLFile(const char fileName[]) {

//  unsigned int argnum;
  int indent = 0;


  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
  //         Print outer { for whole JSON file
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

  printf("{\n");
  indent += INDENT;


  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
  //         Parse the IDL file
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

  //
  // Open the file
  //
  ifstream idlFile(fileName);        // open 

  if (!idlFile.is_open()) {
    stringstream ss;
    ss << "Could not open IDL file: " << fileName;
    throw C150Exception(ss.str());
  }
  // The following line does all the work parsing the file into
  // the variable parseTree

  Declarations parseTree(idlFile);

  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
  //         Print out all Types
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

  // This iterator will return pairs of <string, TypeDeclaration*>
  // If this doesn't make sense, look on the Web for information
  // about iterating over C++ maps. 

  std::map<std::string, TypeDeclaration*>::iterator iter;  
  TypeDeclaration *typep;

  //
  //  Opening { and indent
  //
  indent_it(indent);
  indent += INDENT;
  printf("\"types\" : {");               // start types json key
  bool writeCommaType = false;
  bool writeCommaFunction = false;

  //
  //  Loop printing each type
  //
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


    printf("%s\n", 
	   writeCommaType ? "," : "");  // write the output
    writeCommaType = true;
    indent_it(indent);
    printf("\"%s\" : ", 
	   typep -> getName().c_str());  // write the output

    //
    // It's a struct
    //
    if (typep -> isStruct()) {
	    //
	    // Write start of dict for type (note might close on same line for builtin)
	    //
	    
	    indent += INDENT;
	    putchar('{');

	    print_type_of_type("struct");  // write the output    
	    //
	    //set up for member list
	    printf(",\n");
	    //
	    indent_it(indent);
	    printf("\"members\" : ");
	    //
	    // print members of struct
	    //
	    print_arglist_or_members(typep -> getStructMembers(), indent);
	    //
	    // close struct type
	    //
	    putchar('\n');
	    indent -= INDENT;
	    indent_it(indent);
	    putchar('}');
    }
    //
    // It's an array
    //
    else if (typep -> isArray()) { 
	    //
	    // Write start of dict for type (note might close on same line for builtin)
	    //
	    
	    indent += INDENT;
	    putchar('{');

	    print_type_of_type("array");  // write the output    
	    //
	    //  member_type
	    //
	    printf(",\n");
	    indent_it(indent);
	    printf("\"member_type\" : \"%s\"", 
		   (typep -> getArrayMemberType())->getName().c_str());
	    //
	    //  element_count
	    //
	    printf(",\n");
	    indent_it(indent);
	    printf("\"element_count\" : %u", typep -> getArrayBound());
	    //
	    // close array type
	    //
	    putchar('\n');
	    indent -= INDENT;
	    indent_it(indent);
	    putchar('}');

    } else {
	    //
	    // Builtin type
	    //
	    putchar('{');
	    print_type_of_type("builtin");  // write the output    
	    putchar('}');
    }

  } 

  //
  //  Write }, to close dict for all types
  //

  putchar('\n');
  indent -= INDENT;
  indent_it(indent);
  printf("},\n");


  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
  //         Print out all Functions
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

  //
  //  Loop once for each function declaration
  //

  // This iterator will return pairs of <string, FunctionDeclaration*>
  std::map<std::string, FunctionDeclaration*>::iterator fiter;  

  FunctionDeclaration *functionp;    // Declare FunctionDelcaration pointer

  //
  //  Opening { and indent
  //
  indent_it(indent);
  indent += INDENT;
  printf("\"functions\" : {");               // start types json key

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
    print_function(functionp, indent, writeCommaFunction);
    writeCommaFunction = true;
  }

  //
  //  Write } to close dict for all functions
  //

  putchar('\n');
  indent -= INDENT;
  indent_it(indent);
  printf("}\n");





  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
  //         Print outer } for whole JSON file
  // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
  indent -= INDENT;
  indent_it(indent);
  printf("}\n");
    
} 




// ----------------------------------------------------------
//
//                   print_function
//
//     Prints the json for a function (quite similar to a struct)
//
// ----------------------------------------------------------

void print_function(FunctionDeclaration *functionp, int indent, 
		    bool writeCommaFunction)
{

    //
    //  "functionname" : {
    //
    printf("%s\n", 
	   writeCommaFunction ? "," : "");  // write the output
    writeCommaFunction = true;
    indent_it(indent);
    printf("\"%s\" : {\n", 
	   functionp -> getName().c_str());  // write the output
    indent += INDENT;

    //
    //  "return_type" : "type_name",
    //
    indent_it(indent);
    printf("\"return_type\" : \"%s\",\n", functionp->getReturnType()->getName().c_str());

    //
    //  "arguments" " : {
    //
    indent_it(indent);
    printf("\"arguments\" : ");

    //
    // Get a C++ reference to the vector with the argument list
    //
    ArgumentVector& args = functionp -> getArgumentVector();


    //
    // print members of struct
    //
    print_arglist_or_members(args, indent);
    putchar('\n');

    indent -= INDENT;
    indent_it(indent);
    putchar('}');

}

// ----------------------------------------------------------
//
//                   print_arglist_or_members
//
//     The same internal structures are used to track argument lists
//     and structure members. Either way, print the list
//     bracketed by [ ] with each item being a two entry dict.
//
//     The opening { is printed is the next character...the }
//     is indented on its own line.
//
// ----------------------------------------------------------

void print_arglist_or_members(vector<Arg_or_Member_Declaration *>& members, int indent)
{
  bool writeCommaStruct = false;
  unsigned memberNum;

  //
  //  Opening { and indent
  //
  printf("[");               // start types json key
  indent_it(indent);
  indent += INDENT;
	    
  //
  // The members are in a vector, not a map, so we
  // access them by position, not name
  //
  for(memberNum=0; memberNum < members.size();memberNum++) {
	  Arg_or_Member_Declaration* memp = members[memberNum];
	  printf("%s\n",
		 writeCommaStruct ? "," : "") ;
	  indent_it(indent);
	  printf("{\"name\" : \"%s\", \"type\" : \"%s\"}",
		 memp->getName().c_str(), 
		 memp->getType()->getName().c_str()) ;
	  writeCommaStruct = true;
  }

  //
  // Write closing '}'
  //
  putchar('\n');
  indent -= INDENT;
  indent_it(indent);
  printf("]");
}


// ----------------------------------------------------------
//
//                   void print_type_of_type
//
//     Write "type_of_type" : "ARG"
//
// ----------------------------------------------------------

void print_type_of_type(const char * type_of_type)
{
	printf("\"type_of_type\" : \"%s\"", type_of_type);
}


// ----------------------------------------------------------
//
//                   indent
//
//     Indent stdout by amount given
//
// ----------------------------------------------------------

void indent_it(int ind)
{
  while (ind--) {
	  putc(' ', stdout);
  }
}
