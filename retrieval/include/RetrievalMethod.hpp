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


#ifndef _RETRIEVALMETHOD_HPP
#define _RETRIEVALMETHOD_HPP


#include "Index.hpp"
#include "WeightedIDSet.hpp"
#include "IndexedReal.hpp"


//------------------------------------------------------------
//      Abstract Interface for A Query
//------------------------------------------------------------

/// Abstract query
class Query {
public:
  virtual char *id() = 0;
};

/// Abstract query representation

class QueryRep {
};


//------------------------------------------------------------
//      Abstract Interface for Feedback Document Subset
//------------------------------------------------------------

/// Reuse a WeightedIDSet to represent A feedback doc set, with the weight interpreted as the corresponding "prob. of relevance" which could be any score that may be useful for feedback
typedef WeightedIDSet DocIDSet;


//------------------------------------------------------------
//      Abstract Interface for A Retrieval Method/Model
//------------------------------------------------------------



/*!
  A retrieval method is determined by specifying the following elements <BR>
  <UL>
  <LI> A method to compute the query representation
  <LI> A method to score a document w.r.t. a query representation
  <LI> A method to score all the documents in the collection w.r.t. a query representation
  <LI> A method to update the query representation based on a set of (relevant) documents
  </UL>
*/

class RetrievalMethod {
public:
  RetrievalMethod(Index &collectionIndex) : ind(collectionIndex) {}
  virtual ~RetrievalMethod() {}

  /// compute the representation for a query, semantics defined by subclass
  virtual QueryRep *computeQueryRep(Query &qry)=0;

  /// Score a document identified by the id w.r.t. a query rep
  virtual double scoreDoc(QueryRep &qry, int docID)=0;
  
  /// Score all documents in the collection 
  virtual void scoreCollection(QueryRep &qry, IndexedRealVector &results);

  /// update the query, feedback support
  virtual void updateQuery(QueryRep &qryRep, DocIDSet &relDocs) = 0;

protected:
  Index &ind;
};

#endif /* _RETRIEVALMETHOD_HPP */









