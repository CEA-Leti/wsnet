/**
 *  \file   redblack_interval_tree.h
 *  \brief  RedBlackIntervalTree Concrete Class definition
 *  \author Luiz Henrique Suraty Filho
 *  \date   July 2017
 *  \version 1.0
 **/

#ifndef WSNET_CORE_DATA_STRUCTURE_INTERVAL_TREE_REDBLACK_INTERVAL_TREE_H_
#define WSNET_CORE_DATA_STRUCTURE_INTERVAL_TREE_REDBLACK_INTERVAL_TREE_H_

#include <list>
#include <vector>
#include <map>

#include <kernel/include/definitions/types/interval/interval.h>
#include <kernel/include/data_structures/interval_tree/interval_tree.h>
#include <kernel/include/data_structures/interval_tree/redblack_interval_tree_element.h>
/** \brief The Concrete Class : RedBlackIntervalTree Class
 *
 * It is the implementation of the IntervalTree using an Augmented RedBlack Tree.
 **/
class RedBlackIntervalTree : public IntervalTree{
  public:
    RedBlackIntervalTree();
    ~RedBlackIntervalTree();
    void CheckRedBlackTreeProperties() const;
    void Print() const;
  private:
    //  A sentinel is used for root and for nil.
    RedBlackIntervalTreeElement * root_;
    RedBlackIntervalTreeElement * nil_;
    RedBlackIntervalTreeElement * GetPredecessorOf(RedBlackIntervalTreeElement *) const;
    RedBlackIntervalTreeElement * GetSuccessorOf(RedBlackIntervalTreeElement *) const;
    std::list<std::weak_ptr<Interval>> FindAllIntersectionsRecursive(RedBlackIntervalTreeElement *, IntervalBoundary, IntervalBoundary);
    std::map<IntervalUid, RedBlackIntervalTreeElement*> FindAllElementsIntersectionRecursive(RedBlackIntervalTreeElement *, IntervalBoundary, IntervalBoundary);
    RedBlackIntervalTreeElement * FindInterval(std::weak_ptr<Interval>);
    std::list<std::weak_ptr<Interval>> FindAllIntersectionsImpl(std::weak_ptr<Interval>);
    void DeleteImpl(std::weak_ptr<Interval>);
    void InsertImpl(std::weak_ptr<Interval>);
    void DeleteElement(RedBlackIntervalTreeElement *);
    void LeftRotate(RedBlackIntervalTreeElement *);
    void RightRotate(RedBlackIntervalTreeElement *);
    void InsertElement(RedBlackIntervalTreeElement *);
    void InsertFixUp(RedBlackIntervalTreeElement *);
    void TreePrintHelper(RedBlackIntervalTreeElement *) const;
    void FixUpMaxHigh(RedBlackIntervalTreeElement *);
    void DeleteFixUp(RedBlackIntervalTreeElement *);
    void VerifyMaxHighFields(RedBlackIntervalTreeElement *) const;
    int  VerifyMaxHighFieldsHelper(RedBlackIntervalTreeElement *, const IntervalBoundary, int ) const;
};

#endif // WSNET_CORE_DATA_STRUCTURE_INTERVAL_TREE_REDBLACK_INTERVAL_TREE_H_
