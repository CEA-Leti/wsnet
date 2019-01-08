/**
 *  \file   redblack_interval_tree_element.h
 *  \brief  RedBlackIntervalTreeElement Concrete Class definition
 *  \author Luiz Henrique Suraty Filho
 *  \date   July 2017
 *  \version 1.0
 **/

#ifndef WSNET_CORE_DATA_STRUCTURE_INTERVAL_TREE_REDBLACK_INTERVAL_TREE_ELEMENT_H_
#define WSNET_CORE_DATA_STRUCTURE_INTERVAL_TREE_REDBLACK_INTERVAL_TREE_ELEMENT_H_

#include <string.h>
#include <kernel/include/definitions/types/interval/interval.h>

enum RedBlackIntervalTreeElementColor {black = 0, red = 1};

/** \brief The Concrete Class : RedBlackIntervalTreeElement Class
 *
 * It represents an element on the RedBlack Tree.
 **/
class RedBlackIntervalTreeElement {
    friend class RedBlackIntervalTree;
  public:
    RedBlackIntervalTreeElement(){};
    RedBlackIntervalTreeElement(std::weak_ptr<Interval> newInterval):
      interval_(newInterval) ,
      key_(newInterval.lock()->GetLowPoint()),
      high_(newInterval.lock()->GetHighPoint()) ,
      max_high_(high_) {
    };
    ~RedBlackIntervalTreeElement(){};
    void Print(RedBlackIntervalTreeElement * nil,
        RedBlackIntervalTreeElement * root) const{
      if (this==nil){
        std::cout<<"nil node"<<std::endl;
        return;
      }
      std::cout << "id=" << interval_.lock()->GetUID() << " | low(key)=" << key_ << " | high=" << high_ << " | maxHigh=" << max_high_;
      std::cout << "| l->key=" << ( (left_ == nil) ? "NULL" : std::to_string(left_->key_));
      std::cout << "| l->id=" << ( (left_ == nil) ? "NULL" : std::to_string(left_->interval_.lock()->GetUID()));
      std::cout << "| r->key=" << ( (right_ == nil) ? "NULL" : std::to_string(right_->key_));
      std::cout << "| r->id=" << ( (right_ == nil) ? "NULL" : std::to_string(right_->interval_.lock()->GetUID()));
      std::cout << "| p->key=" << ( (parent_ == root) ? "NULL" : std::to_string(parent_->key_));
      std::cout << "| p->id=" << ( (parent_ == root) ? "NULL" : std::to_string(parent_->interval_.lock()->GetUID()));
      std::cout << "| color=" << ( (color_ == black) ? "Black" : "Red") << std::endl;
      std::cout << "   Interval Content: ";
      interval_.lock()->Print();
      std::cout << std::endl;
    };
  protected:
    std::weak_ptr<Interval> interval_;
    IntervalBoundary key_;
    IntervalBoundary high_;
    IntervalBoundary max_high_;
    RedBlackIntervalTreeElementColor color_;
    RedBlackIntervalTreeElement * left_;
    RedBlackIntervalTreeElement * right_;
    RedBlackIntervalTreeElement * parent_;

};

#endif //WSNET_CORE_DATA_STRUCTURE_INTERVAL_TREE_REDBLACK_INTERVAL_TREE_ELEMENT_H_
