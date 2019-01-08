/**
 *  \file   interval_tree.h
 *  \brief  IntervalTree Abstract Class definition
 *  \author Luiz Henrique Suraty Filho
 *  \date   July 2017
 *  \version 1.0
 **/

#ifndef WSNET_CORE_DATA_STRUCTURES_INTERVAL_TREE_INTERVAL_TREE_H_
#define WSNET_CORE_DATA_STRUCTURES_INTERVAL_TREE_INTERVAL_TREE_H_

#include <list>
#include <memory>
#include <kernel/include/definitions/types/interval/interval.h>

/** \brief The Abstract Base Class : IntervalTree Class
 *
 * \fn Delete - deletes an interval from the tree
 * \fn Insert - inserts an interval in the tree
 * \fn GetSize - return the current size of the interval tree
 * \fn FindAllIntersections - return the intervals that intersect the given interval
 **/
class IntervalTree {
  public:
    IntervalTree(){ size_ = 0;};
    virtual ~IntervalTree(){};
    void Delete(std::weak_ptr<Interval> interval) {DeleteImpl(interval);};
    void Insert(std::weak_ptr<Interval> interval) {InsertImpl(interval);};
    uint GetSize() {return size_;};
    std::list<std::weak_ptr<Interval>> FindAllIntersections(std::weak_ptr<Interval> interval) {return FindAllIntersectionsImpl(interval);} ;
  private:
    virtual void DeleteImpl(std::weak_ptr<Interval>) = 0;
    virtual void InsertImpl(std::weak_ptr<Interval>) = 0;
    virtual std::list<std::weak_ptr<Interval>> FindAllIntersectionsImpl(std::weak_ptr<Interval>) = 0;
  protected:
    uint size_;
};

#endif // WSNET_CORE_DATA_STRUCTURES_INTERVAL_TREE_INTERVAL_TREE_H_
