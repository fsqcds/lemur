#ifndef _DOCUMENT_HPP
#define _DOCUMENT_HPP

#include "Term.hpp"
#include "DocumentProps.hpp"

/// Interface of a TokenTerm -- a term in a doc stream

class TokenTerm : public Term {
public:

  /// return the spelling of the term (read-only)
  virtual const char *spelling() = 0;
};


/// Abstract document class
/*! 
  The class represents a document that is composed of a string (external) ID
and a sequence of tokens. Subclasses may add extra information such as title
and date.

*/

class Document {
public:
  virtual ~Document() {};
  
  /// returns the external string ID
  virtual char *getID() const =0;


  /// start term iteration
  virtual void startTermIteration()=0;

  /// returns true iff there's at least one term for iteration
  virtual bool hasMore() = 0;

  /// fetch next term, caller should check hasMore() before calling it
  virtual TokenTerm *nextTerm()=0;

  /// return the DocumentProps handle
  virtual DocumentProps *property() {
    return (props);
  }
private:
  DocumentProps *props;
};


#endif


  


