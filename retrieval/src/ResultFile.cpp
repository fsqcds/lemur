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

#include "ResultFile.hpp"


void ResultFile::openForRead(istream &is, Index &index)
{
  inStr=&is;
  ind = &index;
  eof = !readLine();
}

void ResultFile::load(istream &is, Index &index)
{
  int i;
  static ResultEntry entry;

  // delete any old data
  for (i=0; i<resTable->size(); i++) {
    entry = (*resTable)[i];
    delete entry.res;
  }

  resTable->clear();
 
  inStr=&is;
  ind = &index;
  while (readLine()) {
    entry.key = curQID;
    int qi = (*resTable)[entry];
    if (qi<0) {
      entry.res = new IndexedRealVector();
      qi = resTable->add(entry);
    }
    entry = (*resTable)[qi];
    int did = ind->document(curDID);
    if (did>0) {
      entry.res->PushValue(did, curSC); 
    } else {
      cerr << "Warning: doc id "<< curDID << " not found in database\n";
    }
  }
}

bool ResultFile::findResult(char *queryID, IndexedRealVector *&res)
{
  static ResultEntry entry;
  entry.key = queryID;
  int qi = (*resTable)[entry];
  if (qi>=0) {
    entry = (*resTable)[qi];
    res = entry.res;
    return true;
  } else {
    res = NULL;
    return false;
  }
}

void ResultFile::getResult(char *expectedQID, IndexedRealVector &res)
{
  res.clear();
  if (eof || strcmp(curQID, expectedQID)) {
    if (!eof) {
      cerr << "expected query ID: "<< expectedQID << " actual ID: "<<curQID <<endl;
    } else {
      cerr << "Unexpected end of the result file\n";
    }
    cerr.flush();
    
    throw Exception("ResultFile", "query id mismatch between the original query and the result file");
  }
  do {

    int did = ind->document(curDID);
    if (did>0) {
      res.PushValue(did, curSC); 
    } else {
      cerr << "Warning: doc id "<< curDID << " not found in database\n";
    }
    if (! readLine()) {
      // end of file
      eof = true;
      break;
    }
  } while (!strcmp(curQID, expectedQID));
}

void ResultFile::writeResults(char *queryID, IndexedRealVector *results, int maxCountOfResult)
{
  IndexedRealVector::iterator j;
  int count=0;
  for (j= results->begin();j!=results->end();j++) {
    if (count >= maxCountOfResult) {
      break;
    } else {
      count++;
      *outStr << queryID;
      if (trecFmt)
	*outStr << " Q0";
      *outStr << " "  << ind->document((*j).ind) << " " ;
      if (trecFmt)
	*outStr << count << " ";
      *outStr << (*j).val;
      if (trecFmt)
	*outStr << " Exp";
      //      *outStr << endl; //flushes buffer each call.
      *outStr << "\n";
    }
  }  
  outStr->flush();
}



bool ResultFile::readLine()
{
    char dummy1[100];
    char dummy2[100];
    char dummy3[100];

    if (trecFmt) {
      return (*inStr >> curQID >> dummy1 >> curDID >> dummy2 >> curSC >> dummy3);
    } else {
      return (*inStr >> curQID >> curDID >> curSC);
    }
}


