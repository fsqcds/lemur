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


#ifndef _INVINDEX_HPP
#define _INVINDEX_HPP

///  Class for retrieving indexed document collection based on the abstract class Index.hpp
#include "common_headers.hpp"
#include "Index.hpp"
#include "InvDocList.hpp"
#include "InvTermList.hpp"
#include "InvFPTypes.hpp"


// for counts array
#define UNIQUE_TERMS 0
#define TOTAL_TERMS  1
#define DOCS         2
#define DT_FILES     3
#define INV_FILES    4

// for names array
#define DOC_INDEX    0
#define DOC_LOOKUP   1
#define TERM_INDEX   2
#define TERM_LOOKUP  3
#define TERM_IDS     4
#define DOC_IDS      5

class InvIndex : public Index {
public:
   InvIndex();
   InvIndex(const char* indexName);
  ~InvIndex(); 

  /// @name Open index 
  //@{

  /// Open previously created Index with given prefix, return true if opened successfully
  bool open(const char* indexName);
  //@}

  /// Open previously created Index using names in given param file, return true if opened successfully
  bool openName(char* filename);

  /// @name Spelling and index conversion
  //@{

  /// Convert a term spelling to a termID
  int term(const char* word);

  /// Convert a termID to its spelling
  const char* term(int termID);

  /// Convert a spelling to docID
  int document(const char* docIDStr);

  /// Convert a docID to its spelling
  const char* document(int docID); 

  //@}

  /// @name Summary counts
  //@{

  /// Total count (i.e., number) of documents in collection
  int docCount() { return counts[DOCS]; };

  /// Total count of unique terms in collection
  int termCountUnique() { return counts[UNIQUE_TERMS]; };

  /// Total counts of a term in collection
  int termCount(int termID) const;

  /// Total counts of all terms in collection
  int termCount() const { return counts[TOTAL_TERMS]; };

  /// Average document length 
  float docLengthAvg();

  /// Total counts of doc with a given term
  int docCount(int termID);

  /// Total counts of terms in a document, including stop words
  int docLength(DOCID_T docID) const;

  /// Total count of terms in given document, not including stop words
  int docLengthCounted(int docID);

  //@}

  /// @name Index entry access
  //@{
  /// doc entries in a term index, @see DocList @see InvFPDocList
  DocInfoList* docInfoList(int termID);

  /// word entries in a document index (bag of words), @see TermList
  TermInfoList* termInfoList(int docID);

  //@}
protected:
  /// readin all toc
  bool fullToc(const char* fileName);
  /// readin main stats
  bool mainToc(char* fileName);
  /// readin index lookup table
  bool indexLookup();
  /// readin inverted index filenames map
  bool invFileIDs();
  /// read in dt index lookup table
  bool dtLookup();
  /// read in dt index filenames map
  bool dtFileIDs();
  /// read in termIDs to term spelling map
  bool termIDs();
  /// read in docIDs to doc spelling map
  bool docIDs();

  int* counts;    // array to hold all the overall count stats of this db
  char** names;   // array to hold all the names for files we need for this db
  float aveDocLen; // the average document length in this index
  inv_entry* lookup;  // the array holding entries (index is termid)
  dt_entry* dtlookup; // the array holding entries to dt index (index of array is docid)
  TERM_T* terms;   // array of the term spellings (index is termid)
  EXDOCID_T* docnames; // array of the external docids (index is docid)
  char** dtfiles; // array of dt index filenames
  char** invfiles; // array of inv index filenames
  map<TERM_T, TERMID_T, ltstr> termtable; // table of terms to termid
  map<EXDOCID_T, DOCID_T, ltstr> doctable; // table of exdocids to docid
};

#endif