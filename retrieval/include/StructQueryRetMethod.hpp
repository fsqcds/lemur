/*==========================================================================
 * Copyright (c) 2002 University of Massachusetts.  All Rights Reserved.
 *
 * Use of the Lemur Toolkit for Language Modeling and Information Retrieval
 * is subject to the terms of the software license set forth in the LICENSE
 * file included with this software, and also available at
 * http://www.cs.cmu.edu/~lemur/license.html
 *
 *==========================================================================
*/
/*
  Author: fff
 */


#ifndef _STRUCTQUERYRETMETHOD_HPP
#define _STRUCTQUERYRETMETHOD_HPP

#include "RetrievalMethod.hpp"
#include "StructQueryRep.hpp"
#include "DocumentRep.hpp"
#include "ScoreFunction.hpp"
#include "ScoreAccumulator.hpp"
#include "FreqVector.hpp"


//--------------------------------------------------------------------------
//      Abstract Interface for A Retrieval Method/Model for Structured Query
//--------------------------------------------------------------------------

/*!
Given a query <tt>q = op(q<sub>1</sub>,q<sub>2</sub>,...,q<sub>N</sub>)</tt> 
where q<sub>i</sub> can be an structured query node or a term node,
and a document <tt>d</tt>, <tt>StructQueryRetMethod</tt> assumes the 
following general scoring function:

<blockquote>
<pre>
s(q,d) = g(s(q<sub>1</sub>,d), ..., s(q<sub>N</sub>,d))
</pre>
</blockquote>
That is, the score of a document <tt>d</tt> against a query <tt>q</tt> is a 
function <tt>g</tt> of the scores of each child query node in 
<tt>q<sub>i</sub></tt>, where <tt>g</tt> is the composition function for 
<tt>q</tt>, such as average.
 
*/

class StructQueryRetMethod : public RetrievalMethod {
public:
  StructQueryRetMethod(Index &ind) : RetrievalMethod(ind) {}
  virtual ~StructQueryRetMethod() {}
  /// compute the query representation for a text query 
  //(caller responsible for deleting the memory of the generated new instance)
  virtual StructQueryRep *computeStructQueryRep(StructQuery &qry) = 0;

  /// overriding abstract class method
  virtual QueryRep *computeQueryRep(Query &qry); 
  /// score the query against the given document id.
  virtual double scoreDoc(QueryRep &qry, int docID);
  /// score the query against the collection.
  virtual void scoreCollection(QueryRep &qry, IndexedRealVector &results);

  /// compute the doc representation (caller responsible for deleting 
  /// the memory of the generated new instance)
  virtual DocumentRep *computeDocRep(int docID) = 0;
  /// return the scoring function pointer
  virtual ScoreFunction *scoreFunc() = 0;
  /// update the query
  virtual void updateQuery(QueryRep &qryRep, DocIDSet &relDocs) {
    updateStructQuery(*((StructQueryRep *)(&qryRep)), relDocs);
  }

  /// Modify/update the query representation based on a set (presumably)
  /// relevant documents
  virtual void updateStructQuery(StructQueryRep &qryRep, 
				 DocIDSet &relDocs) = 0;

  /// Scoring with the inverted index
  virtual void scoreInvertedIndex(QueryRep &qryRep, 
				  IndexedRealVector &scores, 
				  bool scoreAll=false);
};

/// ScoreFunction for structured querys. Provides evalQuery.
class StructQueryScoreFunc : public ScoreFunction {
public:
  /// Have the QueryNode eval itself on the DocumentRep. 
  virtual double evalQuery(QueryNode *qn, DocumentRep *dRep) {
    return qn->eval(dRep);
  }
  /// No adjustment, return the original score.
  virtual double adjustedScore(double w, QueryNode *qn) {
    return w;
  }  
};

//=============== inlines ========================================

inline QueryRep *StructQueryRetMethod::computeQueryRep(Query &qry) { 
  StructQuery *q = static_cast<StructQuery *>(&qry);
  return (computeStructQueryRep(*q));
}

#endif /* _STRUCTQUERYRETMETHOD_HPP */




