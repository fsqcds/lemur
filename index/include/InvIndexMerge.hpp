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


#ifndef _INVINDEXMERGE_HPP
#define _INVINDEXMERGE_HPP

#include "common_headers.hpp"
#include "InvDocList.hpp"
#include "InvFPTypes.hpp"
#include "Exception.hpp"

#define READBUFSIZE 2000000
#define NUM_FH_OPEN 32

struct IndexReader {
  InvDocList* list;
  ifstream* reader;
};

// this class could actually be static
class InvIndexMerge {
public:
  /// constructor.  
  /// vector : a list of files to merge
  /// char* : name prefix of created files
  /// long  : the maximum size the index can be
  InvIndexMerge(char* buffer, long size, long maxfilesize=2100000000);
  InvIndexMerge(long buffersize=64000000, long maxfilesize=2100000000);
  ~InvIndexMerge();

  /// output of this merge operation and the lookup table for the merged index
  /// returns the number of index files this merge created
  int merge(vector<char*>* tf, char* prefix);

  void setMaxFileSize(long size);
  char* setBuffer(char* buffer, long size);

  /// recursive hierarchical merge
  /// calls mergeFiles() when intermediate files are necessary
  /// base case if finalMerge()
  int hierMerge(vector<char*>* files, int level);

  /// merge these files and put the results into the intmed list
  virtual int mergeFiles(vector<char*>* files, vector<char*>* intmed, int level);

  /// do the final merge and write the lookup table
  virtual int finalMerge(vector<char*>* files);

protected:
  /// write file ids for indexes created
  virtual void writeInvFIDs();
  /// figure out which readers point to the lowest termids
  virtual void least(vector<IndexReader*>* r, vector<int>* ret);
  /// setbuffer for ifstream.  trying to keep ugly os specific code out of main code
  void setbuf(ifstream* fs, char* bp, int bytes);

  char* name;
  vector<char*> invfiles; // list of files that we've written to
  long maxfile; // maximum file size for each index
  long bufsize;
  char* readbuffer;
};

#endif
