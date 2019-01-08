/**
 *  \file   group.c
 *  \brief  Group support
 *  \author Luiz Henrique Suraty Filho
 *  \date   2016
 **/

#include <stdio.h>
#include <stdarg.h>
#include <kernel/include/definitions/nodearch.h>
#include <kernel/include/definitions/grouparch.h>
#include <kernel/include/definitions/group.h>
#include <kernel/include/definitions/node.h>

// Initialize array of groups
group_array_t groups = {0, NULL};

group_t *get_group_by_id(groupid_t _id){
	return groups.elts + _id;
}

nodeid_t get_group_leaderid(groupid_t _id){
	return (groups.elts+_id)->leaderid;
}

array_t get_group_all_membersid(groupid_t _id){
	return (groups.elts+_id)->membersid;
}

int get_group_number_nodes_with_nodetype(groupid_t _id, int _node_type){
	int nbr = 0;
	int i;
	nodeid_t leader = get_group_leaderid(_id);
	array_t members = get_group_all_membersid(_id);

	if (check_node_type(leader,_node_type))
		nbr++;

	for (i=0;i<members.size;i++)
		if (check_node_type(members.elts[i],_node_type))
				nbr++;

	return nbr;
}

array_t get_group_nodesid_by_nodetype(groupid_t _id, int _node_type){
	array_t nodesid = {get_group_number_nodes_with_nodetype(_id, _node_type), NULL};
	nodeid_t leader = get_group_leaderid(_id);
	array_t members = get_group_all_membersid(_id);

	int i;
	int j = 0;
	//if array is empty
	if (nodesid.size ==0)
		return nodesid;

	nodesid.elts = malloc(nodesid.size*sizeof(int));

	if (check_node_type(leader,_node_type)){
		nodesid.elts[j] = leader;
		j++;
	}

		for (i=0;i<members.size;i++)
			if (check_node_type(members.elts[i],_node_type)){
				nodesid.elts[j] = members.elts[i];
				j++;
			}

	return nodesid;
}

int compare_integers(const void *a, const void *b){
	const int *ia = (const int *) a;
	const int *ib = (const int *) b;
	return (int) (*ia - *ib);
}

bool is_member_in_group(nodeid_t _memberid,groupid_t _id){
	array_t node_groups = get_node_groups(_memberid);

	// binary search inside the groups
	if (bsearch(&_id,node_groups.elts,node_groups.size,sizeof(_id),compare_integers))
		return true;

	return false;
}

int	get_number_groups_by_memberid(nodeid_t _memberid){

	int nbr = 0;
	int i = groups.size;
	while (i--){
		if (is_member_in_group(_memberid,i))
			nbr++;
	}

	return nbr;
}

group_array_t get_groups_by_memberid(nodeid_t _memberid){
	group_array_t groups_by_member = {0, NULL};

	groups_by_member.size = get_number_groups_by_memberid(_memberid);

	if (groups_by_member.size == 0)
			return groups_by_member;

	groups_by_member.elts = (group_t *) malloc(sizeof(group_t) * groups_by_member.size);
	int k = 0;
	int i = groups.size;
	while (i--){
		if (is_member_in_group(_memberid,i)){
			groups_by_member.elts[k]=*get_group_by_id(i);
			k++;
		}
	}
	return groups_by_member;
}

bool is_in_same_group(int _nbr_nodes, ...){
	va_list list_arg;
	va_start(list_arg, _nbr_nodes);
	array_t * node_groups_array = (array_t *) malloc(sizeof(array_t)*_nbr_nodes);
	array_t smallest_groups_array = {grouparchs.size+1,NULL};
	int	smallest_groupid = 0;
	int i;
	int el;

	for (i=0;i<_nbr_nodes;i++){
		node_groups_array[i] = get_node_groups(va_arg(list_arg,int));
		if (node_groups_array[i].size < smallest_groups_array.size){
			smallest_groups_array = node_groups_array[i];
			smallest_groupid = i;
		}
	}

	//search elements of smallest group on others
	for (el=0;el<smallest_groups_array.size;el++){
		for (i=0;i<_nbr_nodes;i++){
			// does not check with own list
			if (i != smallest_groupid)
				// if it cannot find in at least one other list of groups
				if (!bsearch(smallest_groups_array.elts+el,node_groups_array[i].elts,node_groups_array[i].size,sizeof(int),compare_integers)){
					free(node_groups_array);
					va_end(list_arg);
					return false;
				}
		}
	}

	free(node_groups_array);
	va_end(list_arg);
	return true;
}

int	get_number_groups_by_grouparchid(grouparchid_t _group_arch){

	int nbr = 0;
	int i = groups.size;
	while (i--){
		if ((groups.elts+i)->grouparch == _group_arch)
			nbr++;
	}

	return nbr;
}

int	get_number_groups_by_type(int _group_type){

	int nbr = 0;
	int i = groups.size;
	while (i--){
		if ((groups.elts+i)->type == _group_type)
			nbr++;
	}

	return nbr;
}

group_array_t get_groups_by_grouparchid(grouparchid_t _grouparchid){
	group_array_t groups_by_arch = {0, NULL};

	groups_by_arch.size = get_number_groups_by_grouparchid(_grouparchid);

	if (groups_by_arch.size == 0)
			return groups_by_arch;

	groups_by_arch.elts = (group_t *) malloc(sizeof(group_t) * groups_by_arch.size);

	int k = 0;
	int i = groups.size;
	while (i--){
		if ((groups.elts+i)->grouparch == _grouparchid){
			groups_by_arch.elts[k]=groups.elts[i];
			k++;
		}
	}
	return groups_by_arch;
}

int calculate_default_number_of_groups(void){
	int arch_id;
	int sum_nodes = 0;

	// sum all group architecture
	for (arch_id=0; arch_id<grouparchs.size ; arch_id++)
		sum_nodes += get_grouparch_by_id(arch_id)->nbr_nodes;

	if (sum_nodes==0)
		return 0;

	// check consistency otherwise, return number of groups
	return ( ((nodes.size % sum_nodes) == 0) ? nodes.size/sum_nodes : -1);

}

int check_groups_size(void){

	return ( (groups.size < 0) ? -1 : 0);
}

int groups_create(void){

	int i = groups.size;

	if ( (groups.elts = (group_t *) malloc(sizeof(group_t) * groups.size) ) == NULL)
		return -1;

	while (i--){
		group_t *group 	= get_group_by_id(i);
		group->id			= i;
		group->grouparch	= 0;
		group->leaderid		= 0;
		group->size			= 1;
		group->type			= GROUP_TYPE_STANDARD;
		group->membersid.size	= 0;
		group->membersid.elts	= NULL;

	}

	return 0;

}

void groups_clean (void){
	int i;

	for (i=0; i<groups.size ; i++){
		if ((groups.elts+i)->membersid.elts)
			free((groups.elts+i)->membersid.elts);
	}

	free(groups.elts);
	return;
}

