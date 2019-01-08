/**
 *  \file   param.c
 *  \brief  Parameter parsing module
 *  \author Guillaume Chelius
 *  \date   2007
 */
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <kernel/include/tools/math/rng/rng.h>
#include <kernel/include/definitions/class.h>
#include "param.h"


/* ************************************************** */
/* ************************************************** */
int get_param_time(char *value, uint64_t *time) {
    uint64_t t1, t2;
    char *endptr;
    
    /* Random? */
    if (strcmp(value, "random") == 0) {
        *time = get_random_time();
        return 0;
    }
    
    /* skip space */
    while (isspace(*value))
        value++;

    /* decode integer */
    t1 = strtoll(value, &endptr, 10);
    if (value == endptr)
        goto missing;
    value = endptr;

    /* skip space */
    while (isspace(*value))
        value++;

    /* decode unit */
    if (!strncmp(value, "s", 1)) {
        t1 = t1 * 1000000000; value += 1;
    } else if (!strncmp(value, "ms", 2)) {
        t1 = t1 * 1000000;    value += 2;
    } else if (!strncmp(value, "us", 2)) {
        t1 = t1 * 1000; value += 2;
    } else if (!strncmp(value, "ns", 2)) {
        value += 2;
    }
    
    /* skip space */
    while (isspace(*value))
        value++;

    /* finished? */
    if (*value == '\0') {
        *time = t1;
        return 0;
    }

    /* range string */
    if (strncmp(value, "..", 2))
        goto garbage;
    value += 2;

    /* skip space */
    while (isspace(*value))
        value++;

    /* decode integer */
    t2 = strtoll(value, &endptr, 10);
    if (value == endptr) {
    missing:
        fprintf(stderr, "get_param_time(): missing time value ('%s')\n", value);
        return -1;
    }
    value = endptr;

    /* skip space */
    while (isspace(*value))
	value++;

    /* decode unit */
    if (!strncmp(value, "s", 1)) {
        t2 = t2 * 1000000000; value += 1;
    }else if (!strncmp(value, "ms", 2)) {
        t2 = t2 * 1000000;    value += 2;
    } else if (!strncmp(value, "us", 2)) {
        t2 = t2 * 1000;       value += 2;
    } else if (!strncmp(value, "ns", 2)) {
        value += 2;
    }

    /* skip space */
    while (isspace(*value))
        value++;

    /* garbage? */
    if (*value != '\0') {
    garbage:
        fprintf(stderr, "get_param_time(): garbage at end of time\n");
        return -1;
    }

    /* Generate random value */
    if (t1 > t2) {
        fprintf(stderr, "get_param_time(): time range must be min..max\n");
        return -1;
    }
    *time = t1 + (t2 - t1) * get_random_double();
    return 0;
}

int get_param_distance(char *value, double *distance) {
    if (strcmp(value, "random")) {
        *distance = strtod(value, NULL);
    } else {
        *distance = get_random_distance();   
    }
    return 0;
}

int get_param_x_position(char *value, double *position) {
    *position = (!strcmp(value, "random") || !strcmp(value, "?")) ? 
	get_random_x_position() : strtod(value, NULL);
    return 0;
}

int get_param_y_position(char *value, double *position) {
    *position = (!strcmp(value, "random") || !strcmp(value, "?")) ? 
	get_random_y_position() : strtod(value, NULL);
    return 0;
}

int get_param_z_position(char *value, double *position) {
    *position = (!strcmp(value, "random") || !strcmp(value, "?")) ? 
	get_random_z_position() : strtod(value, NULL);
    return 0;
}

int get_param_double_range(char *value, double *res, double min, double max) {
    if (strcmp(value, "random")) {
        *res = strtod(value, NULL);
    } else {
        *res = get_random_double() * (max - min) + min;
    }
    return 0;
}

int get_param_double(char *value, double *res) {
    if (strcmp(value, "random")) {
        *res = strtod(value, NULL);
    } else {
        *res = get_random_double();
    }
    return 0;
}

int get_param_integer(char *value, int *integer) {
    if (strcmp(value, "random")) {
        *integer = (int) strtoll(value, NULL, 10);
    } else {
        *integer = get_random_integer();
    }

    return 0;
}

/*edit to support uint64 input (Arturo Guizar */
int get_param_uint64_integer(char *value, uint64_t *u_integer_64) {
    if (strcmp(value, "random")) {
        *u_integer_64 = (uint64_t) strtoll(value, NULL, 10);
    } else {
        *u_integer_64 = (uint64_t) get_random_time();
    }

    return 0;
}
/*end of edition*/

int get_param_unsigned_integer(char *value, unsigned int *u_integer){
    if (strcmp(value, "random")) {
        *u_integer = (unsigned int) strtoll(value, NULL, 10);
    } else {
        *u_integer = (unsigned int) get_random_integer();
    }

    return 0;

}

int get_param_nodeid(char *value, nodeid_t *node, nodeid_t myself) {
    if (strcmp(value, "random")) {
        *node = (nodeid_t) strtoll(value, NULL, 10);
    } else {
        *node = get_random_node(myself);
    }

    return 0;
}

int get_param_class(char *value, classid_t *class) {
    classid_t id;
    
    if ((id = get_classid_by_name(value)) < 0) {
        fprintf(stderr, "get_param_class(): class %s not found \n", value);
        return -1;
    } else {
        *class = id;
        return 0;
    }
}
