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


#include "InvIndex.hpp"

/*
 * NAME DATE - COMMENTS
 * tnt 04/2002 - created
 * dmf 10/22/2002 -- Change to open all of the inverted list files 
 * (dtfiles, invfiles) once at initialization and store the open streams 
 * in the arrays dtfstreams and invfstreams.
 *
 *========================================================================*/

InvIndex::InvIndex() {
  lookup = NULL;  
  dtlookup = NULL;
  dtfiles = NULL;
  dtfstreams = NULL;
  invfiles = NULL;
  invfstreams = NULL;
  docnames = NULL;
  terms = NULL;
  aveDocLen = 0;
}

InvIndex::InvIndex(const string &indexName) {
  lookup = NULL;  
  dtlookup = NULL;
  dtfiles = NULL;
  dtfstreams = NULL;
  invfiles = NULL;
  invfstreams = NULL;
  docnames = NULL;
  terms = NULL;
  aveDocLen = 0;
  open(indexName);
}

InvIndex::~InvIndex() {
  int i;
  delete[](lookup);
  delete[](dtlookup);
  delete[](names);
  delete[](dtfiles);

  if (dtfstreams != NULL) {
    for (i=0;i<counts[DT_FILES];i++) {
      dtfstreams[i].close();
    }
    delete[](dtfstreams);
  }

  delete[](invfiles);

  if (invfstreams != NULL) {
    for (i=0;i<counts[INV_FILES];i++) {
      invfstreams[i].close();
    }
    delete[](invfstreams);
  }

  for (i = 0; i < docmgrs.size(); i++)
    delete(docmgrs[i]);

  delete[](terms);
  delete[](docnames);
  delete[](counts);
}

bool InvIndex::open(const string &indexName){
  counts = new COUNT_T[5];
  names = new string[NAMES_SIZE];  
  names[VERSION_NUM] = "";
  names[DOCMGR_IDS] = "";

  String streamSelect = ParamGetString("stream", "cerr");
  if (streamSelect == "cout") {
    setMesgStream(&cout);
  } else {
    setMesgStream(&cerr);
  }

  if (!fullToc(indexName)) {
    cerr << "Couldn't not properly parse param file. Index was not loaded." << endl;
    return false;
  }

  dtloaded = ParamGetInt("loadDT", 1);

  if (!dtloaded) {
    *msgstream << "Parameter set to dtlist lookup not being loaded" << endl;
  } else {

    // older versions didn't specify version name and will be blank
    // if it's new we call new load functions
    if (!names[VERSION_NUM].empty()) {
      if (!dtLookup())
	return false;
    if (!docMgrIDs())
      return false;
    } else {
      if (!dtLookup_ver1())
	return false;
    }

    if (!dtFileIDs())
      return false;
  }

  if (!indexLookup())
    return false;

  if (!invFileIDs())
    return false;

  if (!termIDs())
    return false;

  if (!docIDs())
    return false;

  *msgstream << "Load index complete." << endl;
  return true;
}

TERMID_T InvIndex::term(const TERM_T &word) const{
  map<TERM_T, TERMID_T, less<TERM_T> >::iterator point = termtable.find(word);
  if (point != termtable.end()) 
    return point->second;

  return 0;
}

const TERM_T InvIndex::term(TERMID_T termID) const {
  if ((termID < 0) || (termID > counts[UNIQUE_TERMS]))
    return "";
  return terms[termID];
}

DOCID_T InvIndex::document(const EXDOCID_T &docIDStr) const{
  map<EXDOCID_T, DOCID_T, less<EXDOCID_T> >::iterator point = doctable.find(docIDStr);
  if (point != doctable.end()) 
    return point->second;

  return 0;
}

const EXDOCID_T InvIndex::document(DOCID_T docID) const { 
  if ((docID < 0) || (docID > counts[DOCS]))
    return "";
  return docnames[docID]; 
}

//const char* InvIndex::docManager(int docID) {
const DocumentManager* InvIndex::docManager(DOCID_T docID) const{
  // no such thing previous to version 1.9
  if (names[VERSION_NUM].empty()) 
    return NULL;

  if ((docID < 0) || (docID > counts[DOCS])) {
    fprintf(stderr, "Error trying to get docManager for invalid docID.\n");
    return NULL;
  }

  // there is no such document
  if (docID == 0)
    return NULL;

  // if it never had one registered.
  if (dtlookup[docID].docmgr == -1)
    return NULL;

  return docmgrs[dtlookup[docID].docmgr]; 
}

COUNT_T InvIndex::termCount(TERMID_T termID) const{
  // the termCount is equal to the position counts
  // this is calculated by the length of the inverted list -(df*2) for each doc, 
  // there is the docid and the tf
  if ((termID <0) || (termID > counts[UNIQUE_TERMS])) {
    *msgstream << "Error:  Trying to get termCount for invalid termID" << endl;
    return 0;
  }

  // this is OOV
  if (termID == 0)
    return 0;

  return lookup[termID].ctf;
}

float InvIndex::docLengthAvg() const{
  return aveDocLen;
}

COUNT_T InvIndex::docCount(TERMID_T termID)  const{
  if ((termID <0) || (termID > counts[UNIQUE_TERMS])) {
    *msgstream << "Error:  Trying to get docCount for invalid termID" << endl;
    return 0;
  }

  // this is OOV
  if (termID == 0)
    return 0;

  return lookup[termID].df;
}

COUNT_T InvIndex::docLength(DOCID_T docID) const{
  if ((docID < 0) || (docID > counts[DOCS])) {
    *msgstream << "Error trying to get docLength for invalid docID." << endl;
    return 0;
  }
    
  if (docID == 0)
    return 0;

  if (!dtloaded) {
    *msgstream << "DT index must be loaded to obtain docLength" << endl;
    return 0;
  }

  return dtlookup[docID].length;
}

COUNT_T InvIndex::docLengthCounted(DOCID_T docID) const{
  if ((docID < 0) || (docID > counts[DOCS])) {
    *msgstream << "Error trying to get docLengthCounted for invalid docID." << endl;
    return 0;
  }
    
  if (docID == 0)
    return 0;

  if (!dtloaded) {
    *msgstream << "DT index must be loaded to obtain docLength" << endl;
    return 0;
  }

  COUNT_T dl;
  DOCID_T id;
  ifstream *look = &(dtfstreams[dtlookup[docID].fileid]);
  if (!look) {
    *msgstream << "Could not open term indexfile for reading." << endl;
    return 0;
  }
  look->seekg(dtlookup[docID].offset, ios::beg);
  look->read((char*)&id, sizeof(DOCID_T));
  look->read((char*)&dl, sizeof(COUNT_T));
  look->read((char*)&dl, sizeof(COUNT_T));
  if ( !(look->gcount() == sizeof(COUNT_T)) ) 
    return 0;
  
  return dl;
}

DocInfoList* InvIndex::docInfoList(TERMID_T termID) const{
  if ((termID < 0) || (termID > counts[UNIQUE_TERMS])) {
    *msgstream << "Error:  Trying to get docInfoList for invalid termID" << endl;
    return NULL;
  }

  if (termID == 0)
    return NULL;
  ifstream *indexin = &(invfstreams[lookup[termID].fileid]);
  indexin->seekg(lookup[termID].offset, ios::beg);
  DocInfoList* doclist;
  InvDocList* dlist = new InvDocList();
  bool success;

  if (!names[VERSION_NUM].empty()) {
    // version 1.9 is compressed and must be decompressed
    success = dlist->binReadC(*indexin);
  } else {
    success = dlist->binRead(*indexin);
  }
  if (!success) {
    return NULL;
  } else {
    doclist = dlist;
    return doclist;
  }
}

TermInfoList* InvIndex::termInfoList(DOCID_T docID) const{
  if ((docID < 0) || (docID > counts[DOCS])) {
    *msgstream << "Error trying to get termInfoList for invalid docID." << endl;
    return NULL;
  }

  if (docID == 0)
    return NULL;

  if (!dtloaded) {
    *msgstream << "DT index must be loaded to obtain termInfoList" << endl;
    return NULL;
  }
  ifstream *indexin = &(dtfstreams[dtlookup[docID].fileid]);
  indexin->seekg(dtlookup[docID].offset, ios::beg);
  TermInfoList* termlist;
  InvTermList* tlist = new InvTermList();
  if (!tlist->binRead(*indexin)) {
    return NULL;
  } 
  termlist = tlist;
  return termlist;
}

/*=======================================================================
 * PRIVATE METHODS 
 =======================================================================*/
bool InvIndex::fullToc(const string &fileName) {
  FILE* in = fopen(fileName.c_str(), "rb");
  *msgstream << "Trying to open toc: " << fileName << endl;
  if (in == NULL) {
     *msgstream << "Couldn't open toc file for reading" << endl;
    return false;
  }

  char key[128];
  char val[128];
  while (!feof(in)) {
    if (fscanf(in, "%s %s", key, val) != 2) continue;
    cerr << key << ":" << val << endl;
    if (strcmp(key, NUMDOCS_PAR) == 0) {
      counts[DOCS] = atoi(val);
    } else if (strcmp(key, NUMTERMS_PAR) == 0) {
      counts[TOTAL_TERMS] = atoi(val);
    } else if (strcmp(key, NUMUTERMS_PAR) == 0) {
      counts[UNIQUE_TERMS] = atoi(val);
    } else if (strcmp(key, NUMDT_PAR) == 0) {
      counts[DT_FILES] = atoi(val);
    } else if (strcmp(key, NUMINV_PAR) == 0) {
      counts[INV_FILES] = atoi(val);
    } else if (strcmp(key, INVINDEX_PAR) == 0) {
      names[DOC_INDEX] = val;
    } else if (strcmp(key, INVLOOKUP_PAR) == 0) {
      names[DOC_LOOKUP] = val;
    } else if (strcmp(key, DTINDEX_PAR) == 0) {
      names[TERM_INDEX] = val;
    } else if (strcmp(key, DTLOOKUP_PAR) == 0) {
      names[TERM_LOOKUP] = val;
    } else if (strcmp(key, TERMIDMAP_PAR) == 0) {
      names[TERM_IDS] = val;
    } else if (strcmp(key, DOCIDMAP_PAR) == 0) {
      names[DOC_IDS] = val;
    } else if (strcmp(key, DOCMGR_PAR) == 0) {
      names[DOCMGR_IDS] = val;
    } else if (strcmp(key, VERSION_PAR) == 0) {
      names[VERSION_NUM] = val;
    }
  }    

  aveDocLen = counts[TOTAL_TERMS] / (float) counts[DOCS];

  fclose(in);
  return true;
}

bool InvIndex::indexLookup() {
  FILE* in = fopen(names[DOC_LOOKUP].c_str(), "rb");
  *msgstream << "Trying to open invlist lookup: " << names[DOC_LOOKUP] << endl;
  if (in == NULL) {
    *msgstream << "Couldn't open invlist lookup table for reading" << endl;
    return false;
  }

  lookup = new inv_entry[counts[UNIQUE_TERMS]+1];
  inv_entry* e;
  TERMID_T tid =0;
  FILEID_T fid =0;
  long off =0;
  COUNT_T ctf = 0;
  COUNT_T df = 0;
  // if COUNT_T changes, this needs to be changed...
  while (fscanf(in, "%d %d %d %d %d", &tid, &fid, &off, &ctf, &df) == 5) {
    e = &lookup[tid];
    e->fileid = fid;
    e->offset = off;
    e->ctf = ctf;
    e->df = df;
  }
  fclose(in);
 
  return true;
}

bool InvIndex::dtLookup_ver1() {
  FILE* in = fopen(names[TERM_LOOKUP].c_str(), "rb");
  *msgstream << "Trying to open dtlist lookup: " 
	     << names[TERM_LOOKUP] << endl;
  if (in == NULL) {
    *msgstream <<  "Couldn't open dt lookup table for reading" << endl;
    return false;
  }

  dtlookup = new dt_entry[counts[DOCS]+1];
  dt_entry* e;
  TERMID_T tid =0;
  FILEID_T fid =0;
  long off =0;
  int len =0;
  while (fscanf(in, "%d %d %d %d", &tid, &fid, &off, &len) == 4) {
    e = &dtlookup[tid];
    e->fileid = fid;
    e->offset = off;
    e->length = len;
  }
  fclose(in);
  return true;
}

bool InvIndex::dtLookup() {
  *msgstream << "Trying to open dtlist lookup: " << names[TERM_LOOKUP] << endl;

  FILE* in = fopen(names[TERM_LOOKUP].c_str(), "rb");

  if (in == NULL) {
    *msgstream << "Couldn't open dt lookup table for reading" << endl;
    return false;
  }

  dtlookup = new dt_entry[counts[DOCS]+1];
  dt_entry* e;
  TERMID_T tid =0;
  FILEID_T fid =0;
  long off =0;
  int len =0;
  int mgr =0;
  while (fscanf(in, "%d %d %d %d %d", &tid, &fid, &off, &len, &mgr) == 5) {
    e = &dtlookup[tid];
    e->fileid = fid;
    e->offset = off;
    e->length = len;
    e->docmgr = mgr;
  }
  fclose(in);
  return true;
}

bool InvIndex::dtFileIDs() {
  FILE* in = fopen(names[TERM_INDEX].c_str(), "rb");
  *msgstream << "Trying to open term index filenames: " << names[TERM_INDEX] << endl;
  if (in == NULL) {
    *msgstream << "Error opening term index filenames file" << endl;
    return false;
  }


  dtfiles = new string [counts[DT_FILES]];
  dtfstreams = new ifstream[counts[DT_FILES]];
  int index;
  int len;
  char* str;

  while (!feof(in)) {
    if (fscanf(in, "%d %d", &index, &len) != 2) 
        continue;

    str = new char[len + 1];  // fixed PATH_MAX buffer here.
    if (fscanf(in, "%s", str) != 1) {
      delete[](str);
      continue;
    }
    str[len] = '\0';
    dtfiles[index] = str;
    delete[](str);
    dtfstreams[index].open(dtfiles[index].c_str(), ios::in | ios::binary);
    if (dtfstreams[index] == NULL)
      cerr << "error opening: " << dtfiles[index] << endl;
  }

  fclose(in);
  return true;
}

bool InvIndex::docMgrIDs() {
  FILE* in = fopen(names[DOCMGR_IDS].c_str(), "r");
  *msgstream << "Trying to open doc manager ids file: " << names[DOCMGR_IDS] << endl;
  if (in == NULL) {
    fprintf(stderr, "Error opening doc manager ids \n");
    return false;
  }

  int ind, len;
  char* str;

  while (!feof(in)) {
    if (fscanf(in, "%d %d", &ind, &len) != 2)
      continue;

    str = new char[len + 1]; 
    if (fscanf(in, "%s", str) != 1) {
      delete[](str);
      continue;
    }
    str[len] = '\0';
    DocumentManager* dm = DocMgrManager::openDocMgr(str);
    docmgrs.push_back(dm);
    delete[]str;
  }
  fclose(in);
  return true;
}

bool InvIndex::invFileIDs() {
  FILE* in = fopen(names[DOC_INDEX].c_str(), "rb");
  *msgstream << "Trying to open inverted index filenames: " << names[DOC_INDEX] << endl;
  if (in == NULL) {
    *msgstream <<  "Error opening inverted index filenames file"<< endl;
    return false;
  }

  invfiles = new string[counts[INV_FILES]];
  invfstreams = new ifstream[counts[INV_FILES]];
  int index;
  int len;
  char* str;

  while (!feof(in)) {
    if (fscanf(in, "%d %d", &index, &len) != 2) 
        continue;

    str = new char[len + 1];  // fixed PATH_MAX buffer here
    if (fscanf(in, "%s", str) != 1) {
      delete[](str); 
      continue;
    }
    str[len] = '\0';
    invfiles[index] = str;
    delete[](str);
    invfstreams[index].open(invfiles[index].c_str(), ios::in | ios::binary);
  }

  fclose(in);
  return true;
}

bool InvIndex::termIDs() {
  FILE* in = fopen(names[TERM_IDS].c_str(), "rb");
  *msgstream << "Trying to open term ids file: " << names[TERM_IDS] << endl;
  if (in == NULL) {
    *msgstream <<  "Error opening termfile" << endl;
    return false;
  }
  terms = new TERM_T[counts[UNIQUE_TERMS]+1];

  int index;
  int len;
  char* str;

  while (!feof(in)) {
    if (fscanf(in, "%d %d", &index, &len) != 2) 
        continue;

    str = new char[len + 1]; // fixed size MAX_TERM_LENGTH buffer here.
    if (fscanf(in, "%s", str) != 1) {
      delete[](str);
      continue;
    }
    str[len] = '\0';
    terms[index] = str;
    delete[](str);
    termtable[terms[index]] = index;
  }

  fclose(in);
  if (termtable.size() != counts[UNIQUE_TERMS]+1) {
    *msgstream << "Warning:Didn't read in as many terms as we were expecting." << endl;
    *msgstream << "could be corrupted file or duplicate terms in file" << endl;
  }

  return true;
}

bool InvIndex::docIDs() {
  FILE* in = fopen(names[DOC_IDS].c_str(), "rb");
   *msgstream<< "Trying to open doc ids file: " << names[DOC_IDS] << endl;
  if (in == NULL) {
    *msgstream << "Error opening docfile" << endl;
    return false;
  }
  docnames = new EXDOCID_T[counts[DOCS]+1];

  int index;
  int len;
  char* str;

  while (!feof(in)) {
    if (fscanf(in, "%d %d", &index, &len) != 2)
      continue;

    str = new char[len + 1]; // fixed size buffer here
    if (fscanf(in, "%s", str) != 1) {
      delete[](str);
      continue;
    }
    str[len] = '\0';
    docnames[index] = str;
    doctable[docnames[index]] = index;
    delete[](str);
  }
  fclose(in);

  if (doctable.size() != counts[DOCS]+1) {
    *msgstream << "Warning:Didn't read in as many docids as we were expecting" << endl;
    *msgstream << "could be corrupted file or duplicate docids in file" << endl;
  }
  return true;
}

void InvIndex::setMesgStream(ostream * lemStream) {
  msgstream = lemStream;
}
