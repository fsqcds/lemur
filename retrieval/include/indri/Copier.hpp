// 
// Warning: This file is automatically generated
// 

#ifndef INDRI_COPIER_HPP
#define INDRI_COPIER_HPP

namespace indri { 
  namespace lang { 
    class Copier { 
    public:
      virtual ~Copier();
      virtual void defaultBefore( class Node* oldNode );
      virtual class Node* defaultAfter( class Node* oldNode, class Node* newNode );

      virtual void before( class IndexTerm* oldNode );
      virtual class Node* after( class IndexTerm* oldNode, class IndexTerm* newNode );
      virtual void before( class Field* oldNode );
      virtual class Node* after( class Field* oldNode, class Field* newNode );
      virtual void before( class ExtentInside* oldNode );
      virtual class Node* after( class ExtentInside* oldNode, class ExtentInside* newNode );
      virtual void before( class ExtentOr* oldNode );
      virtual class Node* after( class ExtentOr* oldNode, class ExtentOr* newNode );
      virtual void before( class ExtentAnd* oldNode );
      virtual class Node* after( class ExtentAnd* oldNode, class ExtentAnd* newNode );
      virtual void before( class UWNode* oldNode );
      virtual class Node* after( class UWNode* oldNode, class UWNode* newNode );
      virtual void before( class ODNode* oldNode );
      virtual class Node* after( class ODNode* oldNode, class ODNode* newNode );
      virtual void before( class FilReqNode* oldNode );
      virtual class Node* after( class FilReqNode* oldNode, class FilReqNode* newNode );
      virtual void before( class FilRejNode* oldNode );
      virtual class Node* after( class FilRejNode* oldNode, class FilRejNode* newNode );
      virtual void before( class FieldLessNode* oldNode );
      virtual class Node* after( class FieldLessNode* oldNode, class FieldLessNode* newNode );
      virtual void before( class FieldGreaterNode* oldNode );
      virtual class Node* after( class FieldGreaterNode* oldNode, class FieldGreaterNode* newNode );
      virtual void before( class FieldBetweenNode* oldNode );
      virtual class Node* after( class FieldBetweenNode* oldNode, class FieldBetweenNode* newNode );
      virtual void before( class RawScorerNode* oldNode );
      virtual class Node* after( class RawScorerNode* oldNode, class RawScorerNode* newNode );
      virtual void before( class TermFrequencyScorerNode* oldNode );
      virtual class Node* after( class TermFrequencyScorerNode* oldNode, class TermFrequencyScorerNode* newNode );
      virtual void before( class CachedFrequencyScorerNode* oldNode );
      virtual class Node* after( class CachedFrequencyScorerNode* oldNode, class CachedFrequencyScorerNode* newNode );
      virtual void before( class PriorNode* oldNode );
      virtual class Node* after( class PriorNode* oldNode, class PriorNode* newNode );
      virtual void before( class OrNode* oldNode );
      virtual class Node* after( class OrNode* oldNode, class OrNode* newNode );
      virtual void before( class NotNode* oldNode );
      virtual class Node* after( class NotNode* oldNode, class NotNode* newNode );
      virtual void before( class MaxNode* oldNode );
      virtual class Node* after( class MaxNode* oldNode, class MaxNode* newNode );
      virtual void before( class CombineNode* oldNode );
      virtual class Node* after( class CombineNode* oldNode, class CombineNode* newNode );
      virtual void before( class WAndNode* oldNode );
      virtual class Node* after( class WAndNode* oldNode, class WAndNode* newNode );
      virtual void before( class WSumNode* oldNode );
      virtual class Node* after( class WSumNode* oldNode, class WSumNode* newNode );
      virtual void before( class WeightNode* oldNode );
      virtual class Node* after( class WeightNode* oldNode, class WeightNode* newNode );
      virtual void before( class ExtentRestriction* oldNode );
      virtual class Node* after( class ExtentRestriction* oldNode, class ExtentRestriction* newNode );
      virtual void before( class FilterNode* oldNode );
      virtual class Node* after( class FilterNode* oldNode, class FilterNode* newNode );
      virtual void before( class ContextCounterNode* oldNode );
      virtual class Node* after( class ContextCounterNode* oldNode, class ContextCounterNode* newNode );
      virtual void before( class ScoreAccumulatorNode* oldNode );
      virtual class Node* after( class ScoreAccumulatorNode* oldNode, class ScoreAccumulatorNode* newNode );
      virtual void before( class AnnotatorNode* oldNode );
      virtual class Node* after( class AnnotatorNode* oldNode, class AnnotatorNode* newNode );
   };
 };
};

#endif // INDRI_COPER_HPP
