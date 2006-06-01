#ifndef _DBINTERFACE_H
#define _DBINTERFACE_H

#include <string>
using std::string;

#include "CGIOutput.h"
#include "Index.hpp"
#include "IndexManager.hpp"
#include "Stemmer.hpp"
#include "IndexManager.hpp"
#include "InQueryRetMethod.hpp"
#include "InQueryOpParser.hpp"
#include "Stopper.hpp"
#include "StringQuery.hpp"
#include "ElemDocMgr.hpp"
#include "MatchInfo.hpp"
#include "TextHandlerManager.hpp"
#include "Param.hpp"
#include "LemurIndriIndex.hpp"
#include "indri/QueryEnvironment.hpp"

#define MIN(a,b) (((a) < (b)) ? (a) : (b))

/**
 * A class that represents a single result item for the results
 *
 * @author Mark J. Hoy [http://www.cs.cmu.edu/~mhoy/]
 * @version 4/14/06 
 */
class DBInterface {

private:
  string    pathToIndex;
  string    dataRoot;
  CGIOutput *output;

  lemur::api::Stemmer* getDbStemmer(const lemur::api::Index* ind);
  
  lemur::api::Index *openIndex();

  lemur::api::IndexedRealVector removeDuplicateResults(lemur::api::IndexedRealVector results);

  /**
  * @param datasourceID the index of the database used for this search
  * @param results pointer to the vector of results
  * @param listLength the max. number of results to show on this page
  * @param rankStart the starting number of the first result of the page
  */
  void displaySearchResults(lemur::api::Index *db, int datasourceID, lemur::parse::StringQuery* q, indri::api::QueryEnvironment *indriEnvironment,
                            lemur::api::IndexedRealVector *results, int listLength, int rankStart);

  /**
   * Creates a summary string for the result
   *
   */
  std::string getSummaryString(const lemur::api::DocumentManager* dm, lemur::api::Index *db, lemur::parse::StringQuery* q, int resultID, string docext);

  /**
   * Retrieves the original HTTP link of the document for trecweb data
   *
   * @param index the opened index
   * @param docID the document internal ID
   * @return the original URI
   */
  string getDocHTTPLink(lemur::api::Index *index, long docID);

  /**
   * Returns the Indri index (if any) of where a document is
   *
   * @param indexes collection of all the indexes
   * @param documentID the document internal ID
   * @return the index (or NULL if not found)
   */
  indri::index::Index* _indexWithDocument( indri::collection::Repository::index_state& indexes, int documentID );


  /**
   * Gets the field extent tags for a specified field for a document
   *
   * @param index the indri index
   * @param docid the document internal id
   * @param fieldID the field ordinal to get the extents for
   * @return a greedy_vector of FieldExtents
   */
  indri::utility::greedy_vector<indri::index::FieldExtent> *getFieldTags(indri::index::Index *index, long docid, int fieldID);

  /**
   * Retrieves a count for a specfic term of how many fields the term occurs
   * (Note: this is not a term-count - it's a field count)
   *
   * @param index the indri index
   * @param docid the internal document ID
   * @param termid the internal term ID
   * @param anchorFields the field vector to look in
   * @return count of the numebr of fields where the term occurs
   */
  int getFieldCountOfTerm(indri::index::Index *index, long docid, long termid, indri::utility::greedy_vector<indri::index::FieldExtent> *anchorFields);

  /**
   * Retrieves a term count for a specfic term in a set of fields
   *
   * @param index the indri index
   * @param docid the internal document ID
   * @param termid the internal term ID
   * @param anchorFields the field vector to look in
   * @return termcount of how many times the term occurs total
   */
  int getTFAnchorTagCount(indri::index::Index *index, long docid, long termid, indri::utility::greedy_vector<indri::index::FieldExtent> *anchorFields);

  /**
   * Reformulates a free-text query
   * @param origQuery the original query
   * @return the (possibly) modified query string
   */
  string indriDefaultQueryExpansion(string origQuery);


public:
  enum QUERY_INTERFACE_TYPE {
    QUERY_INTERFACE_INDRI=1,
    QUERY_INTERFACE_INQUERY
  };
  /**
   * Basic constructor.
   */
  DBInterface();

  /**
   * Constructor w/ specific CGI output
   * @param outputInterface the output object
   */
  DBInterface(CGIOutput *outputInterface);

  /**
   * Destructor
   */
  ~DBInterface();

  /**
   * Sets the base root path
   * @param rootPath the root path
   */
  void setDataRoot(string rootPath);

  /**
   * Sets the path to the index
   * @param indexPath the path to the index
   */
  void setIndexPath(string indexPath);

  /**
   * Sets the output interface
   * @param outputInterface the output object
   */
  void setOutput(CGIOutput *outputInterface);

  /**
   * Displays the index listing
   */
  void displayIndexStatistics();

  /**
   * Returns the current data root path (if set)
   * @return the data root path
   */
  string getDataRoot();

  /**
   * Returns the current index path
   * @return the current index path
   */
  string getIndexPath();

  /**
   * Performs a search and outputs to the output object
   * @param datasourceID which index to use
   * @param query the actual raw query
   * @param listLength the maximum number of results per page
   * @param rankStart the starting number to use for our results
   * @param queryType the query language type
   */
  void search(int datasourceID, string *query, long listLength, long rankStart, QUERY_INTERFACE_TYPE queryType=QUERY_INTERFACE_INDRI);

  /**
   * Retrieves a parsed document and outputs it
   * @param docID the internal Lemur ID of the document
   */
  void getParsedDoc(long docID);

  /**
   * Retrieves a raw document and outputs it
   * @param docID the internal Lemur ID of the document
   */
  void getDocIID(long docID);

  /**
   * Retrieves a document from the original document's ID and outputs it
   * @param docID the external document ID
   */
  void getDocXID(string *docID);

  /**
   * Outputs the corpus statistics for a given term
   * @param term the term string
   */
  void getTermCorpusStats(string *term);

  /**
   * Outputs the basic inverted list for a given term
   * @param term the term string
   */
  void getTermInvList (string *term);

  /**
   * Outputs the inverted list with text positions for a given term
   * @param term the term string
   */
  void getTermInvPosList (string *term);

  /**
   * Outputs the inverted list for a given term with potential anchor text
   * and related statistics
   *
   * @param term the term string
   */
  void getTermInvListWithAnchor (string *term);

  /**
   * Outputs the inverted list for a given term with position information
   * and also potential anchor text, related statistics, and anchor
   * position information
   *
   * @param term the term string
   */
  void getTermInvPosListWithAnchor(string *term);


  /**
   * Outputs the stemmed version (if any) of the given word
   * @param term the term string
   */
  void getWordStem(string *term);

}; // class DBInterface

#endif // _DBINTERFACE_H

