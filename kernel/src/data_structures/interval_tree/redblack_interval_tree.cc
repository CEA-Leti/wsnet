/**
 *  \file   redblack_interval_tree.cc
 *  \brief  RedBlackIntervalTree Concrete Class implementation
 *  \author Luiz Henrique Suraty Filho
 *  \date   July 2017
 *  \version 1.0
 **/

#include <iostream>
#include <math.h>
#include <limits>
#include <queue>
#include <cassert>

#include <kernel/include/data_structures/interval_tree/interval_tree.h>
#include <kernel/include/data_structures/interval_tree/redblack_interval_tree.h>

/** \brief Constructor of the RedBlackIntervalTree Class
 *  \fn RedBlackIntervalTree::RedBlackIntervalTree()
 **/
RedBlackIntervalTree::RedBlackIntervalTree() : IntervalTree(){

  nil_ = new RedBlackIntervalTreeElement;
  nil_->left_ = nil_->right_ = nil_->parent_ = nil_;
  nil_->color_ = black;
  nil_->key_ = nil_->high_ = nil_->max_high_ = std::numeric_limits<IntervalBoundary>::lowest();
  nil_->interval_ = std::weak_ptr<Interval>();

  root_ = new RedBlackIntervalTreeElement;
  root_->parent_ = root_->left_ = root_->right_ = nil_;
  root_->key_ = root_->high_ = root_->max_high_ = std::numeric_limits<IntervalBoundary>::max();
  root_->color_ = black;
  root_->interval_ = std::weak_ptr<Interval>();
}

/** \brief Destructor of the RedBlackIntervalTree Class
 *  \fn RedBlackIntervalTree::~RedBlackIntervalTree()
 **/
RedBlackIntervalTree::~RedBlackIntervalTree() {

  RedBlackIntervalTreeElement * x = root_->left_;
  std::queue<RedBlackIntervalTreeElement *> elements_to_be_deleted;

  if (x != nil_) {
    if (x->left_ != nil_) {
      elements_to_be_deleted.push(x->left_);
    }
    if (x->right_ != nil_) {
      elements_to_be_deleted.push(x->right_);
    }
    // delete x->storedInterval;
    delete x;
    while( !elements_to_be_deleted.empty() ) {
      x = elements_to_be_deleted.front();
      elements_to_be_deleted.pop();
      if (x->left_ != nil_) {
        elements_to_be_deleted.push(x->left_);
      }
      if (x->right_ != nil_) {
        elements_to_be_deleted.push(x->right_);
      }

      //delete x->interval_;
      delete x;
    }
  }
  delete nil_;
  delete root_;
}

/** \brief Check if two closed intervals overlap
 *  \fn bool OverlapClosed(IntervalBoundary a_interval_low, IntervalBoundary a_interval_high, IntervalBoundary b_interval_low, IntervalBoundary b_interval_high)
 *  \param a_interval_low is the low endpoint of the closed interval a
 *  \param a_interval_high is the high endpoint of the closed interval a
 *  \param b_interval_low is the low endpoint of the closed interval b
 *  \param b_interval_high is the high endpoint of the closed interval b
 *  \return true if the intervals overlap, and false otherwise
 **/
bool OverlapClosed(IntervalBoundary a_interval_low, IntervalBoundary a_interval_high, IntervalBoundary b_interval_low, IntervalBoundary b_interval_high) {
  if (a_interval_low <= b_interval_low) {
    return( (b_interval_low <= a_interval_high) );
  } else {
    return( (a_interval_low <= b_interval_high) );
  }
}

/** \brief Left Rotate the tree on an element
 * 	As described in [1], it basically makes the parent of x be to the left of x,
 * 	x the parent of its parent before the rotation and fixes other pointers accordingly
 * 	while updating the maxHigh field of x and y after the rotation.
 *  \fn void RedBlackIntervalTree::LeftRotate(RedBlackIntervalTreeElement * x)
 *  \param x the element to rotate on
 **/
void RedBlackIntervalTree::LeftRotate(RedBlackIntervalTreeElement *x) {
  RedBlackIntervalTreeElement * y;

  y=x->right_;
  x->right_=y->left_; // turn y's left subtree into x's right subtree

  if (y->left_ != nil_)
    y->left_->parent_=x;

  y->parent_=x->parent_; // link x's parent to y

  // instead of checking if x->parent is the root as in the book, we
  // count on the root sentinel to implicitly take care of this case
  if( x == x->parent_->left_) {
    x->parent_->left_=y;
  } else {
    x->parent_->right_=y;
  }
  y->left_=x; // put x on y's left
  x->parent_=y;

  x->max_high_=std::max(x->left_->max_high_,std::max(x->right_->max_high_,x->high_));
  y->max_high_=std::max(x->max_high_,std::max(y->right_->max_high_,y->high_));
}


/** \brief Right Rotate the tree on an element
 * 	As described in [1], it basically makes the parent of x be to the left of x,
 * 	x the parent of its parent before the rotation and fixes other pointers accordingly
 * 	while updating the maxHigh field of x and y after the rotation.
 *  \fn void RedBlackIntervalTree::RightRotate(RedBlackIntervalTreeElement * y)
 *  \param y the element to rotate on
 **/
void RedBlackIntervalTree::RightRotate(RedBlackIntervalTreeElement *y) {
  RedBlackIntervalTreeElement * x;

  x=y->left_;
  y->left_=x->right_;

  if (nil_ != x->right_)
    x->right_->parent_=y;

  x->parent_=y->parent_;
  if( y == y->parent_->left_) {
    y->parent_->left_=x;
  } else {
    y->parent_->right_=x;
  }
  x->right_=y;
  y->parent_=x;

  y->max_high_=std::max(y->left_->max_high_,std::max(y->right_->max_high_,y->high_));
  x->max_high_=std::max(x->left_->max_high_,std::max(y->max_high_,x->high_));
}

/** \brief Insert an element into the tree
 * 	Inserts an element into the tree as if it were a regular binary tree as described in [1]
 * 	This function is to be called by the Insert function, not by users
 *  \fn void RedBlackIntervalTree::InsertElement(RedBlackIntervalTreeElement * z)
 *  \param z the element to insert
 **/
void RedBlackIntervalTree::InsertElement(RedBlackIntervalTreeElement *z) {
  RedBlackIntervalTreeElement * x = root_->left_;
  RedBlackIntervalTreeElement * y = root_;

  while( x != nil_) {
    y=x;
    if ( z->key_ < x->key_ ) {
      x=x->left_;
    } else { // z->key >= x->key
      x=x->right_;
    }
  }
  z->parent_=y;
  if ( (y == root_) ||
      (y->key_ > z->key_) ) {
    y->left_=z;
  } else {
    y->right_=z;
  }
  z->left_=nil_;
  z->right_=nil_;
  z->color_ = red;

}

/** \brief Fix the max_high_ values
 * 	Travels up to the root fixing the maxHigh fields after an insertion or deletion
 *  \fn void RedBlackIntervalTree::FixUpMaxHigh(RedBlackIntervalTreeElement *x)
 *  \param x the element to start from
 **/
void RedBlackIntervalTree::FixUpMaxHigh(RedBlackIntervalTreeElement *x) {
  while(x != root_) {
    x->max_high_=std::max(x->high_,std::max(x->left_->max_high_,x->right_->max_high_));
    x=x->parent_;
  }
}

/** \brief Fix insertion violations
 * 	Fix the violations that may have occurred while inserting the element
 * 	This function is to be called by the Insert function, not by users
 *  \fn void RedBlackIntervalTree::InsertFixUp(RedBlackIntervalTreeElement * z)
 *  \param z the element to insert
 **/
void RedBlackIntervalTree::InsertFixUp(RedBlackIntervalTreeElement *z){
  RedBlackIntervalTreeElement * y;

  while(z->parent_->color_ == red) {
    if (z->parent_ == z->parent_->parent_->left_) {
      y=z->parent_->parent_->right_;
      if (y->color_) {
        z->parent_->color_=black;
        y->color_=black;
        z->parent_->parent_->color_=red;
        z=z->parent_->parent_;
      } else {
        if (z == z->parent_->right_) {
          z=z->parent_;
          LeftRotate(z);
        }
        z->parent_->color_=black;
        z->parent_->parent_->color_=red;
        RightRotate(z->parent_->parent_);
      }
    } else {
      // this part is essentially the section above with
      // left and right interchanged
      y=z->parent_->parent_->left_;
      if (y->color_) {
        z->parent_->color_=black;
        y->color_=black;
        z->parent_->parent_->color_=red;
        z=z->parent_->parent_;
      } else {
        if (z == z->parent_->left_) {
          z=z->parent_;
          RightRotate(z);
        }
        z->parent_->color_=black;
        z->parent_->parent_->color_=red;
        LeftRotate(z->parent_->parent_);
      }
    }
  }
  root_->left_->color_=black;
}



/** \brief Insert a node
 * 	Creates a node node which contains the appropriate key and info pointers and inserts it into the tree.
 *  \fn void RedBlackIntervalTree::InsertImpl(std::weak_ptr<Interval> new_interval)
 *  \param new_interval the interval to insert
 **/
void RedBlackIntervalTree::InsertImpl(std::weak_ptr<Interval> new_interval){
  RedBlackIntervalTreeElement * z = new RedBlackIntervalTreeElement(new_interval);
  InsertElement(z);
  FixUpMaxHigh(z->parent_);
  InsertFixUp(z);

  ++size_;

  return;
}

/** \brief Get the successor of an element
 * 	It is based on the algorithm described in [1]
 *  \fn RedBlackIntervalTreeElement * RedBlackIntervalTree::GetSuccessorOf(RedBlackIntervalTreeElement * x) const
 *  \param x is the element we want the succesor of
 *  \return the successor of x or nullptr if no successor exists
 **/
RedBlackIntervalTreeElement * RedBlackIntervalTree::GetSuccessorOf(RedBlackIntervalTreeElement * x) const {
  RedBlackIntervalTreeElement * y;

  if (nil_ != (y = x->right_)) { // assignment to y is intentional
    while(y->left_ != nil_) { // returns the minimum of the right subtree of x
      y=y->left_;
    }
    return(y);
  } else {
    y=x->parent_;
    while(x == y->right_) { // sentinel used instead of checking for nil
      x=y;
      y=y->parent_;
    }
    if (y == root_) return(nil_);
    return(y);
  }
}

/** \brief Get the predecessor of an element
 * 	It is based on the algorithm described in [1]
 *  \fn RedBlackIntervalTreeElement * RedBlackIntervalTree::GetPredecessorOf(RedBlackIntervalTreeElement * x) const
 *  \param x is the element we want the succesor of
 *  \return the predecessor of x or nullptr if no successor exists
 **/
RedBlackIntervalTreeElement * RedBlackIntervalTree::GetPredecessorOf(RedBlackIntervalTreeElement * x) const {
  RedBlackIntervalTreeElement * y;

  if (nil_ != (y = x->left_)) { // assignment to y is intentional
    while(y->right_ != nil_) { // returns the maximum of the left subtree of x
      y=y->right_;
    }
    return(y);
  } else {
    y=x->parent_;
    while(x == y->left_) {
      if (y == root_) return(nil_);
      x=y;
      y=y->parent_;
    }
    return(y);
  }
}

/** \brief This function recursively prints the elements of the tree in-order
 *  It can only be used by the class, not by users.
 *  \fn void RedBlackIntervalTree::TreePrintHelper( RedBlackIntervalTreeElement * x) const
 *  \param x is the element to start from
 **/
void RedBlackIntervalTree::TreePrintHelper( RedBlackIntervalTreeElement *x) const {

  if (x != nil_) {
    TreePrintHelper(x->left_);
    x->Print(nil_,root_);
    TreePrintHelper(x->right_);
  }
}

/** \brief This function recursively prints the nodes of the tree in-order
 *  \fn void RedBlackIntervalTree::Print() const
 **/
void RedBlackIntervalTree::Print() const {
  RedBlackIntervalTreeElement * x = root_->left_;

  TreePrintHelper(x);
}

/** \brief Fix the max_high_ values
 * 	Performs rotations and changes colors to restore red-black properties after an element is deleted.
 * 	It is based on the algorithm described in [1]
 *  \fn void RedBlackIntervalTree::DeleteFixUp(RedBlackIntervalTreeElement *x)
 *  \param x is the child of the spliced out element in DeleteElement.
 **/
void RedBlackIntervalTree::DeleteFixUp(RedBlackIntervalTreeElement *x) {
  RedBlackIntervalTreeElement * w;

  while( (x != root_->left_) && (x->color_ == black) ) {
    if (x == x->parent_->left_) {
      w=x->parent_->right_;
      if (w->color_==red) {
        w->color_=black;
        x->parent_->color_=red;
        LeftRotate(x->parent_);
        w=x->parent_->right_;
      }
      if ( (w->right_->color_==black) && (w->left_->color_==black) ) {
        w->color_=red;
        x=x->parent_;
      } else {
        if (w->right_->color_ == black) {
          w->left_->color_=black;
          w->color_=red;
          RightRotate(w);
          w=x->parent_->right_;
        }
        w->color_=x->parent_->color_;
        x->parent_->color_=black;
        w->right_->color_=black;
        LeftRotate(x->parent_);
        x=root_->left_;
      }
    } else { // the code below is has left and right switched from above
      w=x->parent_->left_;
      if (w->color_ == red) {
        w->color_=black;
        x->parent_->color_=red;
        RightRotate(x->parent_);
        w=x->parent_->left_;
      }
      if ( (w->right_->color_ == black) && (w->left_->color_ == black) ) {
        w->color_=red;
        x=x->parent_;
      } else {
        if (w->left_->color_==black) {
          w->right_->color_=black;
          w->color_=red;
          LeftRotate(w);
          w=x->parent_->left_;
        }
        w->color_=x->parent_->color_;
        x->parent_->color_=black;
        w->left_->color_=black;
        RightRotate(x->parent_);
        x=root_->left_;
      }
    }
  }
  x->color_=black;
}

/** \brief Find a specific interval (we look for its UID)
 * 	Performs a search of a specific interval
 * 	It is based on the algorithm described in [1]
 *  \fn RedBlackIntervalTreeElement * RedBlackIntervalTree::FindInterval(std::weak_ptr<Interval> i)
 *  \param i is the interval we are interested.
 *  \return the interval if found, the nil sentinel otherwise
 **/
RedBlackIntervalTreeElement * RedBlackIntervalTree::FindInterval(std::weak_ptr<Interval> i) {
  std::map<IntervalUid, RedBlackIntervalTreeElement*> possible_elements;

  possible_elements = FindAllElementsIntersectionRecursive(root_->left_, i.lock()->GetLowPoint(), i.lock()->GetHighPoint());
  auto found = possible_elements.find(i.lock()->GetUID());

  if (found != possible_elements.end()){
    return found->second;
  }
  return nil_;

  /*Print();

	RedBlackIntervalTreeElement * x = Search(root_->left_,i);

	x->Print(nil_,root_);
	return x;*/

}

/** \brief Delete an Interval from the tree
 * 	Deletes the entry for the element on the tree if found, but it does not delete (free) the Interval itself
 *  \fn void RedBlackIntervalTree::DeleteImpl(std::weak_ptr<Interval> i)
 *  \param i is the interval we are interested.
 **/
void RedBlackIntervalTree::DeleteImpl(std::weak_ptr<Interval> i) {

  RedBlackIntervalTreeElement * x = FindInterval(i);

  if (x!=nil_){
    DeleteElement(x);
  }
}

/** \brief Delete an element from the tree
 * 	Deletes the entry for the element on the tree if found. It calls FixUpMaxHigh to fix max_high_ fields
 * 	and calls DeleteFixUp to restore red-black properties. It is based on algorithm described in [1]
 *  \fn void RedBlackIntervalTree::DeleteElement(RedBlackIntervalTreeElement *z)
 *  \param z is the element to be deleted
 **/
void RedBlackIntervalTree::DeleteElement(RedBlackIntervalTreeElement *z){
  RedBlackIntervalTreeElement * y;
  RedBlackIntervalTreeElement * x;

  y= ((z->left_ == nil_) || (z->right_ == nil_)) ? z : GetSuccessorOf(z);
  x= (y->left_ == nil_) ? y->right_ : y->left_;
  if (root_ == (x->parent_ = y->parent_)) { // assignment of y->p to x->p is intentional
    root_->left_=x;
  }
  else {
    if (y == y->parent_->left_) {
      y->parent_->left_=x;
    } else {
      y->parent_->right_=x;
    }
  }
  if (y != z) { // y should not be nil in this case

    // y is the node to splice out and x is its child
    y->max_high_ = std::numeric_limits<IntervalBoundary>::lowest();
    y->left_=z->left_;
    y->right_=z->right_;
    y->parent_=z->parent_;
    z->left_->parent_=z->right_->parent_=y;
    if (z == z->parent_->left_) {
      z->parent_->left_=y;
    } else {
      z->parent_->right_=y;
    }
    FixUpMaxHigh(x->parent_);
    if (y->color_ == black) {
      y->color_ = z->color_;
      DeleteFixUp(x);
    }
    else {
      y->color_ = z->color_;
    }
    delete z;

  } else {
    FixUpMaxHigh(x->parent_);
    if (y->color_ == black) {
      DeleteFixUp(x);
    }
    delete y;
  }

  return;
}

/** \brief Find all intervals that intersect with a given bound [low, high]
 * 	This algorithm calls another protected method that find the intervals recursively
 *  \fn std::list<std::weak_ptr<Interval>> RedBlackIntervalTree::FindAllIntersectionsImpl(IntervalBoundary low, IntervalBoundary high)
 *  \param low is the low endpoint of the closed interval to be searched
 *  \param high is the high endpoint of the closed interval to be searched
 *  \return the list of intervals that intersects [low,high]
 **/
std::list<std::weak_ptr<Interval>> RedBlackIntervalTree::FindAllIntersectionsImpl(std::weak_ptr<Interval> i){
  return FindAllIntersectionsRecursive(root_->left_, i.lock()->GetLowPoint(), i.lock()->GetHighPoint());
}

/** \brief Find all elements that intersect with a given bound [low, high]
 * 	This algorithm runs recursively through the tree returning all overlapped elements
 *  \fn std::list<std::weak_ptr<Interval>> RedBlackIntervalTree::FindAllIntersectionsRecursive(RedBlackIntervalTreeElement *x, IntervalBoundary low, IntervalBoundary high)
 *  \param x is the element to start from
 *  \param low is the low endpoint of the closed interval to be searched
 *  \param high is the high endpoint of the closed interval to be searched
 *  \return a map with pairs [key=UID,RedBlackIntervalTreeElement*] of all elements that intersects [low,high]
 **/
std::map<IntervalUid, RedBlackIntervalTreeElement*> RedBlackIntervalTree::FindAllElementsIntersectionRecursive(RedBlackIntervalTreeElement * x, IntervalBoundary low, IntervalBoundary high){
  std::map<IntervalUid, RedBlackIntervalTreeElement*> intersected_elements;
  if ((x != nil_)  && OverlapClosed(low,high,x->key_,x->high_) ){
    intersected_elements.insert(std::make_pair(x->interval_.lock()->GetUID(),x));
  }

  if ((x->left_ != nil_) && (x->left_->max_high_ >= low) ){
    std::map<IntervalUid,RedBlackIntervalTreeElement *> new_list = FindAllElementsIntersectionRecursive(x->left_, low, high);
    intersected_elements.insert(new_list.begin(),new_list.end());
  }

  if ((x->right_ != nil_) && (x->interval_.lock()->GetLowPoint() <= high) && (x->right_->max_high_ >= low) ){
    std::map<IntervalUid,RedBlackIntervalTreeElement *> new_list = FindAllElementsIntersectionRecursive(x->right_, low, high);
    intersected_elements.insert(new_list.begin(),new_list.end());
  }

  return intersected_elements;
}


/** \brief Find all intervals that intersect with a given bound [low, high]
 * 	This algorithm runs recursively through the tree returning all overlapped intersections
 *  \fn std::list<std::weak_ptr<Interval>> RedBlackIntervalTree::FindAllIntersectionsRecursive(RedBlackIntervalTreeElement *x, IntervalBoundary low, IntervalBoundary high)
 *  \param x is the element to start from
 *  \param low is the low endpoint of the closed interval to be searched
 *  \param high is the high endpoint of the closed interval to be searched
 *  \return the list of intervals that intersects [low,high]
 **/
std::list<std::weak_ptr<Interval>> RedBlackIntervalTree::FindAllIntersectionsRecursive(RedBlackIntervalTreeElement * x, IntervalBoundary low, IntervalBoundary high){
  std::list<std::weak_ptr<Interval>> intersected_elements;
  if ((x != nil_)  && OverlapClosed(low,high,x->key_,x->high_) ){
    intersected_elements.push_front(x->interval_);
  }

  if ((x->left_ != nil_) && (x->left_->max_high_ >= low) ){
    std::list<std::weak_ptr<Interval>> new_list = FindAllIntersectionsRecursive(x->left_, low, high);
    intersected_elements.insert(intersected_elements.end(), new_list.begin(),new_list.end());
  }

  if ((x->right_ != nil_) && (x->interval_.lock()->GetLowPoint() <= high) && (x->right_->max_high_ >= low) ){
    std::list<std::weak_ptr<Interval>> new_list = FindAllIntersectionsRecursive(x->right_, low, high);
    intersected_elements.insert(intersected_elements.end(), new_list.begin(),new_list.end());
  }

  return intersected_elements;
}

/** \brief Check if the max_high_ fields are correct
 *  \fn IntervalBoundary RedBlackIntervalTree::VerifyMaxHighFieldsHelper(RedBlackIntervalTreeElement *y, const IntervalBoundary currentHigh, int match) const
 *  \param y is the element to start from
 *  \param current_high is the current max_high_
 *  \param match is
 *  \return 1 if the intervals overlap, and 0 otherwise
 **/
int RedBlackIntervalTree::VerifyMaxHighFieldsHelper(RedBlackIntervalTreeElement *y, const IntervalBoundary current_high, int match) const {
  if (y != nil_) {
    match = VerifyMaxHighFieldsHelper(y->left_,current_high,match) ?
        1 : match;
    assert(y->high_ <= current_high);
    if (y->high_ == current_high)
      match = 1;
    match = VerifyMaxHighFieldsHelper(y->right_,current_high,match) ?
        1 : match;
  }
  return match;
}

/** \brief Check if the max_high_ fields are correct
 *  \fn void RedBlackIntervalTree::VerifyMaxHighFields(RedBlackIntervalTreeElement * x) const
 **/
void RedBlackIntervalTree::VerifyMaxHighFields(RedBlackIntervalTreeElement *x) const {
  if (x != nil_) {
    VerifyMaxHighFields(x->left_);
    if(!(VerifyMaxHighFieldsHelper(x,x->max_high_,0) > 0)) {
      std::cout << "error found in CheckMaxHighFields" << std::endl;
    }
    VerifyMaxHighFields(x->right_);
  }
}

/** \brief Check if the RedBlack Tree properties are fulfilled
 *  \fn void RedBlackIntervalTree::CheckRedBlackTreeProperties() const
 **/
void RedBlackIntervalTree::CheckRedBlackTreeProperties() const {
  assert(nil_->key_ == std::numeric_limits<IntervalBoundary>::lowest());
  assert(nil_->high_ == std::numeric_limits<IntervalBoundary>::lowest());
  assert(nil_->max_high_ == std::numeric_limits<IntervalBoundary>::lowest());
  assert(root_->key_ == std::numeric_limits<IntervalBoundary>::max());
  assert(root_->high_ == std::numeric_limits<IntervalBoundary>::max());
  assert(root_->max_high_ == std::numeric_limits<IntervalBoundary>::max());
  assert(nil_->interval_.expired());
  assert(root_->interval_.expired());
  assert(nil_->color_ == 0);
  assert(root_->color_ == 0);
  VerifyMaxHighFields(root_->left_);
}
