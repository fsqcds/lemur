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


#ifndef _LEMUR_KEYFILE_INCINDEX_HPP
#define _LEMUR_KEYFILE_INCINDEX_HPP

/*
 * NAME DATE - COMMENTS
 * tnt 01/02 - created
 * dmf 07/03 - converted to incremental berkeley db btree indexer with
 * integrated document manager.
 * tds 09/03 - modified from BTIncIndex to use keyfile
 * dmf 12/03 - update to 2.1 API and remove parser/docmgr components.
*/
#include "common_headers.hpp"
#include "Index.hpp"
#include "InvFPDocList.hpp"
#include "InvFPTermList.hpp"
#include "InvFPTypes.hpp"
#include "Param.hpp"
#include "PushIndex.hpp"
#include "MemCache.hpp"
#include "Keyfile.hpp"
#include "KeyfileDocMgr.hpp"
#include "ReadBuffer.hpp"
#include "WriteBuffer.hpp"
#include "TermCache.hpp"
#include <cstring>
#include <queue>

// for counts array
#define UNIQUE_TERMS 0
#define TOTAL_TERMS  1
#define DOCS         2
#define DT_FILES     3
#define INV_FILES    4
#define MAX_DOCID_LENGTH 256
#define MAX_TERM_LENGTH 256

#define KEYFILE_MAX_SEGMENTS (16)

// we love multiple inheritance

/*!

  KeyfileIncIndex builds an index assigning termids, docids, tracking
  locations of term within documents, and tracking terms within
  documents.  It also expects a DocumentProp to have the total number of
  terms that were in a document.  It expects that all stopping and
  stemming (if any) occurs before the term is passed in. If used with an
  existing index, new documents are added incrementally. Records are stored
  in keyfile b-trees. KeyfileIncIndex also provides the Index API for using
  the index.

*/
class KeyfileIncIndex : public PushIndex, public Index {
public:
  /// principle record
  class record {
  public:
    /// file offset
    File::offset_type offset;
    /// length of list
    int len;
    /// token length of doc
    int totalLen;
    /// mgrid for terminfolist, df for docinfolist
    int num;     
  };
  /// offset within an individual file segment
  struct SegmentOffset {
    /// segment number
    unsigned int segment;
    /// length of data
    unsigned int length;
    /// file offset
    File::offset_type offset;
  };
  /// individual term data
  struct TermData {
    /// total number of times this term occurs in the corpus
    unsigned int totalCount;
    /// total number of documents this term occurs in
    unsigned int documentCount;
    /// segments containing the data associated with the the term
    SegmentOffset segments[ KEYFILE_MAX_SEGMENTS ];
  };
  /// Instantiate with an existing index name, including extension.
  KeyfileIncIndex(const char* indexName = 0); 
  /// Instantiate with index name without extension. Optionally pass in
  /// cachesize and starting document id number.
  KeyfileIncIndex(char* prefix, int cachesize=128000000, 
		  DOCID_T startdocid=1);
  /// Clean up.
  ~KeyfileIncIndex();

  /// sets the name for this index
  void setName(char* prefix);

  /// the beginning of a new document
  bool beginDoc(DocumentProps* dp);

  /// adding a term to the current document
  bool addTerm(Term& t);

  /// signify the end of current document
  void endDoc(DocumentProps* dp);

  /// signify the end of current document
  virtual void endDoc(DocumentProps* dp, const char* mgr);

  /// signify the end of this collection.
  void endCollection(CollectionProps* cp);

  /// set the document manager to use for succeeding documents
  void setDocManager(const char* mgrID);
    
protected:
  /// try to open an existing index
  bool tryOpen();
  /// write out the table of contents file.
  void writeTOC();
  /// write out the cache
  void writeCache( bool lastRun = false );
  /// final run write out of cache
  void lastWriteCache();

  /// out-of-tree cache management combine segments into single segment
  void mergeCacheSegments();
  /// write out segments
  void writeCacheSegment();
  /// write out document manager ids
  void writeDocMgrIDs();
  /// returns the internal id of given docmgr
  /// if not already registered, mgr will be added
  int docMgrID(const char* mgr);
  /// handle end of document token.
  virtual void doendDoc(DocumentProps* dp, int mgrid);
  /// how long all the lists are
  int listlengths;
  
public:
  /// @name Open index 
  //@{

  /// Open previously created Index with given prefix
  bool open(const char* indexName);
  //@}

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

  /// The document manager for this document
  DocumentManager *docManager(int docID);

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

  /// Total counts of terms in a document, including stop words maybe
  int docLength(DOCID_T docID) const; // should use DOCID_T everywhere...

  /// Total counts of terms in a document including stopwords for sure.
  virtual int totaldocLength (int docID) const;

  /// Total count of terms in given document, not including stop words
  int docLengthCounted(int docID);

  //@}

  /// @name Index entry access
  //@{
  /// doc entries in a term index, @see DocList @see InvFPDocList
  DocInfoList* docInfoList(int termID);

  /// word entries in a document index (bag of words), @see TermList
  TermInfoList* termInfoList(int docID);
  /// word entries in a document index (sequence of words), @see TermList
  TermInfoList* termInfoListSeq(int docID);

  //@}

  /// set the mesg stream
  void setMesgStream(ostream * lemStream);
  /// update data for an already seen term
  void addKnownTerm( int termID, int position );
  /// initialize data for a previously unseen term.
  int addUnknownTerm( InvFPTerm* term );
  /// update data for a term that is not cached in the term cache.
  int addUncachedTerm( InvFPTerm* term );

protected:
  /// open the database files
  void openDBs();
  /// open the segment files
  void openSegments();
  /// create the database files
  void createDBs();

  /// readin all toc
  void fullToc();
  /// read in document manager internal and external ids map
  bool docMgrIDs();
  /// retrieve a document record.
  record fetchDocumentRecord( int key ) const;
  /// store a document record
  void addDocumentLookup( int documentKey, const char* documentName );
  /// store a term record
  void addTermLookup( int termKey, const char* termSpelling );
  /// store a record
  void addGeneralLookup( Keyfile& numberNameIndex, Keyfile& nameNumberIndex, 
			 int number, const char* name );
  /// retrieve and construct the DocInfoList for a term.
  InvFPDocList* internalDocInfoList(int termID);
  /// add a position to a DocInfoList
  void _updateTermlist( InvFPDocList* curlist, int position );
  /// total memory used by cache
  int _cacheSize();
  /// cache size limits based on cachesize parameter to constructor
  void _computeMemoryBounds( int memorySize );
  /// Approximate how many updates to collect before flushing the cache.
  void _resetEstimatePoint();
  /// array to hold all the overall count stats of this db
  int* counts;    
  /// array to hold all the names for files we need for this db
  std::vector<std::string> names;
  /// the average document length in this index
  float aveDocLen; 
  /// list of document managers
  vector<std::string> docmgrs;
  /// Lemur code messages stream		
  ostream* msgstream;

  // All database handles are marked mutable since they sometimes
  // must be used to fetch values during const methods
  /// termID -> TermData (term statistics and inverted list segment offsets)
  mutable Keyfile invlookup;
  
  // int <-> string mappings for documents and terms
  /// documentName -> documentID 
  mutable Keyfile dIDs;
  /// documentID -> documentName
  mutable Keyfile dSTRs;
  /// termName -> termID
  mutable Keyfile tIDs;
  /// termID -> termName
  mutable Keyfile tSTRs;
  /// document statistics (document length, etc.)
  mutable File dtlookup; 
  /// read buffer for dtlookup
  ReadBuffer* dtlookupReadBuffer; 
  /// filestream for writing the list of located terms
  File writetlist; 

  /// buffers for term() lookup functions
  char termKey[MAX_TERM_LENGTH];
  /// buffers for document() lookup functions
  char docKey[MAX_DOCID_LENGTH];
  /// memory for use by inverted list buffers
  int _listsSize;
  /// upper bound for memory use
  int _memorySize;
  /// the prefix name
  std::string name;
  /// array of pointers to doclists
  vector<InvFPDocList*> invertlists; 
  /// list of terms and their locations in this document
  vector<LocatedTerm> termlist; 
  /// the current docmanager to use
  int curdocmgr; 
  /// list of document manager objects
  vector<DocumentManager*> docMgrs; 
  /// cache of term entries
  TermCache _cache;

  /// out-of-tree segments for data
  std::vector<File*> _segments;
  /// highest term id flushed to disk.
  int _largestFlushedTermID;
  /// invertlists point where we should next check on the cache size
  int _estimatePoint; 
};


#endif //_LEMUR_KEYFILE_INCINDEX_HPP
