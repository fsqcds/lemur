/*==========================================================================
 * Copyright (c) 2001 Carnegie Mellon University.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.cs.cmu.edu/~lemur/license.html
 *
 *==========================================================================
*/


#ifndef _EXCEPTION_HPP
#define _EXCEPTION_HPP
#include <iostream.h>

/// Default Exception class
/*!
  This is the base Exception class that all other exceptions should
inherit from. 
  Some guidelines for using Exception:

  
  (1) Define your main function as "int AppMain(int argc, char *argv[])"
     rather than the normal "main" function. 
  (2) When throwing an exception from a class (e.g., MyClass), do

        " throw Exception("MyClass", "description of exception");"
  (3) If you want to process the exception, do

     <PRE> 
             try { 
	     ... ... 
	        throw Exception("MyClass", "an exception");
              }
	      catch  (Exception &ex) {
	         process exception...
             }

    </PRE>

  (4) If the exception is not caught in the application, it will be
   caught be the main function in the lemur toolkit. The default exception handler prints the following
   message on stderr and terminates the program. 
   <PRE>
     Exception [ by MyClass]: an exception
     Program aborted due to exception
 </PRE>

 (5) You can define more specific exceptions by subclassing Exception.
 All exceptions will be caught by the default main function if not caught by an application.


*/

class Exception {
public:
  Exception(char *throwerString=" unknown thrower", char *whatString="unknown exception"):
    thrower(throwerString), what(whatString) {}
  ~Exception() {}
  void writeMessage(ostream &os = cerr);
private:
  char *thrower;
  char *what;
};


inline void Exception::writeMessage(ostream &os)
{
  os << "Exception [by "<< thrower << "]:"<< what << endl;
}
#endif
