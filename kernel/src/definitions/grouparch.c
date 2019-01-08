/**
 *  \file   grouparch.c
 *  \brief  Group architecture module
 *  \author Luiz Henrique Suraty Filho
 *  \date   2016
 */

#include <stdio.h>
#include <string.h>
#include <kernel/include/definitions/grouparch.h>
#include <kernel/include/definitions/node.h>

// Initialize array of group architectures
grouparch_array_t grouparchs = {0, NULL};

grouparchid_t	get_grouparchid_by_name	(char *_name){
	int i;

	for (i=0; i<grouparchs.size ; i++)
		if (!strcmp(_name,(grouparchs.elts + i)->name))
			return (grouparchs.elts + i)->id;

	return -1;
}


grouparch_t	*get_grouparch_by_name (char *_name){
	int i;

	for (i=0; i<grouparchs.size ; i++)
		if (!strcmp(_name,(grouparchs.elts + i)->name))
			return (grouparchs.elts + i);

	return NULL;
}

grouparch_t *get_grouparch_by_id (grouparchid_t _id){
	int i;

	for (i=0; i<grouparchs.size ; i++)
		if ((grouparchs.elts + i)->id == _id)
			return (grouparchs.elts + i);

	return NULL;

}

int grouparchs_malloc(void){
	// allocate memory for group of archictectures
	if ((grouparchs.elts = (grouparch_t *) malloc(sizeof(grouparch_t) * grouparchs.size)) == NULL)
	{
		fprintf(stderr, "ERROR: malloc error (grouparchs_malloc())\n");
		return -1;
	}

	return 0;
}

void grouparch_init(grouparchid_t _id){
	grouparch_t *grouparch = (grouparchs.elts + _id);

	grouparch->id = _id;
	grouparch->name = NULL;
	grouparch->nbr_nodes = 0;
	grouparch->leader_nodearchid = 0;
	grouparch->type = GROUP_TYPE_STANDARD;
	grouparch->membersarch.size = 0;
	grouparch->membersarch.elts = NULL;
}

void grouparchs_clean (void){
	int i;

	for (i=0; i<grouparchs.size ; i++){
		grouparch_t *grouparch = get_grouparch_by_id(i);

		if (grouparch->name)
			free(grouparch->name);

		if (grouparch->membersarch.elts)
			free(grouparch->membersarch.elts);
	}

	free(grouparchs.elts);
	return;
}

