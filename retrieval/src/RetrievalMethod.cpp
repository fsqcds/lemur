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


#include "RetrievalMethod.hpp"


// the most general, but probably also most inefficient implementation
void RetrievalMethod::scoreCollection(QueryRep &qry, IndexedRealVector &results)
{
  for (int id=1; id<=ind.docCount();id++) {
    results.PushValue(id, scoreDoc(qry, id));
  }
}


