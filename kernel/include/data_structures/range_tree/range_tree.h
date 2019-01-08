/**
 *  \file   range_tree.h
 *  \brief  RangeTree Abstract Class definition
 *  \author Luiz Henrique Suraty Filho
 *  \date   July 2017
 *  \version 1.0
 **/

#ifndef WSNET_CORE_DATA_STRUCTURE_RANGE_TREE_RANGE_TREE_H_
#define WSNET_CORE_DATA_STRUCTURE_RANGE_TREE_RANGE_TREE_H_

#include <memory>

/** \brief The Abstract Base Class : RangeTreeTree Class
 *
 *  This class is just a stub for the moment. In the future
 *  we expect to have different range trees implemented to
 *  avoid copies of signals being sent to nodes very far from
 *  the source
 *
 * \fn Delete - deletes an interval from the tree
 * \fn Insert - inserts an interval in the tree
 * \fn GetSize - return the current size of the interval tree
 **/
class RangeTree {
public:
	RangeTree(){ size_ = 0;};
	virtual ~RangeTree(){};
	void Delete(){DeleteImpl();};
	void Insert(){InsertImpl();};
	uint GetSize() {return size_;};
private:
	virtual void DeleteImpl(){};
	virtual void InsertImpl(){};
protected:
	uint size_;
};

#endif // WSNET_CORE_DATA_STRUCTURE_RANGE_TREE_RANGE_TREE_H_
