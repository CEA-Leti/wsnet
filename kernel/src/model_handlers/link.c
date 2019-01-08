/**
 *  \file   link.c
 *  \brief  Link utilities
 *  \author Luiz Henrique Suraty Filho
 *  \date   2016
 **/

#include <kernel/include/definitions/class.h>
#include <kernel/include/model_handlers/link.h>


link_condition_t link_get_link_condition(call_t *to_link, call_t *to_interface, call_t *from_interface){
	class_t *class = get_class_by_id(to_link->class);
	if (class->model->type != MODELTYPE_LINK)
		return 0;
	return class->methods->link.get_link_condition(to_link, to_interface, from_interface);
}

link_type_t link_get_link_type(call_t *to_link, call_t *to_interface, call_t *from_interface){
	class_t *class = get_class_by_id(to_link->class);
	if (class->model->type != MODELTYPE_LINK)
		return 0;
	return class->methods->link.get_link_type(to_link, to_interface, from_interface);
}

link_communication_type_t link_get_communication_type(call_t *to_link, call_t *to_interface, call_t *from_interface){
	class_t *class = get_class_by_id(to_link->class);
	if (class->model->type != MODELTYPE_LINK)
		return 0;
	return class->methods->link.get_communication_type(to_link, to_interface, from_interface);
}

int link_get_complementary_link_condition(call_t *to_link, call_t *to_interface, call_t *from_interface){
	class_t *class = get_class_by_id(to_link->class);
	if (class->model->type != MODELTYPE_LINK)
		return 0;
	return class->methods->link.get_complementary_link_condition(to_link, to_interface, from_interface);
}

double link_get_mutual_orientation(call_t *to_link, call_t *to_interface, call_t *from_interface){
	class_t *class = get_class_by_id(to_link->class);
	if (class->model->type != MODELTYPE_LINK)
		return 0;
	return class->methods->link.get_mutual_orientation(to_link, to_interface, from_interface);
}
