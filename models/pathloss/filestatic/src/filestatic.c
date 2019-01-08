/**
 *  \file   filestatic.c
 *  \brief  Link success probability from file
 *  \author Guillaume Chelius
 *  \date   2007
 **/

#include <kernel/include/modelutils.h>


/* ************************************************** */
/* ************************************************** */
model_t model =  {
    "Link success probability extracted from file",
    "Guillaume Chelius",
    "0.1",
    MODELTYPE_PATHLOSS
};


/* ************************************************** */
/* ************************************************** */
struct classdata {
    double *success;
    int node_cnt;
};


/* ************************************************** */
/* ************************************************** */
int init(call_t *to, void *params) {
    struct classdata *classdata = malloc(sizeof(struct classdata));
    param_t *param;
    char *filepath = NULL;
    int src, dst;
    double proba;
    FILE *file;
    char str[128];

    /* default values */
    filepath = "propagation.data";

    /* get parameters */
    list_init_traverse(params);
    while ((param = (param_t *) list_traverse(params)) != NULL) {
        if (!strcmp(param->key, "file")) {
            filepath = param->value;
        }
    }

    /* open file */
    if ((file = fopen(filepath, "r")) == NULL) {
        fprintf(stderr, "filestatic: can not open file %s in init()\n", filepath);
        goto error;
    }

    /* extract link success probability */
    classdata->node_cnt = get_node_count();
    classdata->success = malloc(sizeof(double) * classdata->node_cnt *classdata->node_cnt);
    for (src = 0; src < classdata->node_cnt; src++) {
        for (dst = 0; dst < classdata->node_cnt; dst++) {
            
            *(classdata->success + (src * classdata->node_cnt) + dst) = MIN_DBM;
        }
    }
    fseek(file, 0L, SEEK_SET);
    while (fgets(str, 128, file) != NULL) {
        sscanf(str, "%d %d %lf\n",  &src, &dst, &proba);
        *(classdata->success + (src * classdata->node_cnt) + dst) = proba;
    }

    fclose(file);
    set_class_private_data(to, classdata);
    return 0;

 error:
    free(classdata);
    return -1;
}

int destroy(call_t *to) {
    struct classdata *classdata = get_class_private_data(to);
    free(classdata->success);
    free(classdata);
    return 0;
}


/* ************************************************** */
/* ************************************************** */
int bootstrap(call_t *to, void *params) {
  return 0;
}


/* ************************************************** */
/* ************************************************** */
int bind(call_t *to, void *params) {
  DBG_PATHLOSS("model filestatic.c: binding class %s with medium %s\n",
	     get_class_by_id(to->class)->name, get_medium_by_id(to->object)->name);
  return 0;
}

int unbind(call_t *to) {
  return 0;
}


/* ************************************************** */
/* ************************************************** */
double pathloss(call_t *to_pathloss, call_t *to_interface ,call_t *from_interface, packet_t *packet, double rxdBm){
    struct classdata *classdata = get_class_private_data(to_pathloss);
    double success = *(classdata->success + (packet->node * classdata->node_cnt) + to_interface->object);

    if (success == 1) {
        return rxdBm;
    } else if (success == 0) {
        return MIN_DBM;
    } else if (get_random_double() <= success) {
        return rxdBm;        
    } else {
        return MIN_DBM;
    }
}


/* ************************************************** */
/* ************************************************** */
pathloss_methods_t methods = {pathloss};
