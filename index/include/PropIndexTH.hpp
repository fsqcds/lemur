/*==========================================================================
 *
 *  Original source copyright (c) 2003, Carnegie Mellon University.
 *  See copyright.cmu for details.
 *
 *==========================================================================
*/

#include "TextHandler.hpp"
#include "InvFPPushIndex.hpp"
#include "KeyfileIncIndex.hpp"

#ifndef _PROPINDEXTH_HPP
#define _PROPINDEXTH_HPP

#define MAX_WORD_LENGTH 25

/**
   PropIndexTH builds a positional indexer. At this time only InvFPIndex
   is supported. This class is a destination TextHandler. It expects the 
   position of the token to be given (not counted) and will index
   certain properties if passed in the PropertyList. PropertyList *must*
   have one property called "position" with the token position. 
   Any and ALL other properties of type STRING will be indexed at the 
   same position as the token, but added after the token. This is usually 
   the last object in TextHandler chain. If a property should NOT be indexed, 
   remove it before it gets here.
*/

class PropIndexTH : public TextHandler {

public:
  // want to subclass this class for incremental.
  PropIndexTH() { }
  
  /// Create a PropIndexTH with index name filename 
  /// (minus the .ifp extension) and specified buffer size.
  PropIndexTH(char * filename, int bufferSize, bool countStopWds = false, int ind = 1);
  PropIndexTH(char * filename, int bufferSize, bool countStopWds, string ind);
  ~PropIndexTH();

  /// Pushes the doc into the InvFPPushIndex.
  char * handleDoc(char * docno);
  /// Pushes the word into the InvFPPushIndex.
  char * handleWord(char * word, char* original, PropertyList* list);

  /// passes mgrID onto index
  void setDocManager(const char* mgrID);
  // want to subclass this class for incremental.
protected:
  /// Ends a document in the collection
  void endDoc();

  /// The indexer.
  PushIndex * index;
  /// Document properties object to be reused over repeated calls.
  DocumentProps * dp;
  /// Term object to be reused over repeated calls.
  InvFPTerm * term;
  /// Keeps track of document length.
  int docLength;

  /// Indicates whether or not to count stop words in doc length
  bool countStopWds;

  /// Indicates whether object is before first doc
  /// true = yes, false = foundDoc has been called once.
  bool first;
};

#endif

