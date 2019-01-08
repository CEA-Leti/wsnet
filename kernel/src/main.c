/**
 *  \file   main.c
 *  \brief  Simulator main
 *  \author Guillaume Chelius & Loic Lemaitre
 *  \date   2010
 **/

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <kernel/include/data_structures/mem_fs/mem_fs.h>
#include <kernel/include/data_structures/list/list.h>
#include <kernel/include/data_structures/circular_array/circ_array.h>
#include <kernel/include/data_structures/heap/heap.h>
#include <kernel/include/data_structures/hashtable/hashtable.h>
#include <kernel/include/data_structures/sliding_window/sliding_window.h>
#include <kernel/include/definitions/node.h>
#include <kernel/include/definitions/group.h>
#include <kernel/include/scheduler/scheduler.h>
#include <kernel/include/configuration_parser/xmlparser.h>
#include <kernel/include/model_handlers/node_mobility.h>
#include <kernel/include/model_handlers/monitor.h>
#include <kernel/include/model_handlers/noise.h>
#include <kernel/include/tools/math/rng/rng.h>
#include <libraries/wiplan/wiplan_parser.h>

/* ************************************************** */
/* ************************************************** */
int  do_parse_arg (int argc, char *argv[]);
int  do_init      (void);
int  do_bootstrap (void);
void do_clean     (void);
void do_end(void);

/* ************************************************** */
/* ************************************************** */
int main(int argc, char *argv[]) { 
  int error_code = 0;

  if (do_parse_arg(argc, argv)) {  /* parse arguments */
    error_code=1;
    goto end;
  }

  if (do_init()){
    error_code=2;
    goto end;
  }

  if (do_configuration()){
    error_code=3;
    goto end;
  }

  if (do_bootstrap()){
    error_code=4;
    goto end;
  }

  if (do_observe()){
    error_code=5;
    goto end;
  }

  do_end();

  end:
  do_clean();
  return error_code;

}


/* ************************************************** */
/* ************************************************** */
int do_parse_arg(int argc, char *argv[]) {
  char c;

  while((c = getopt(argc, argv, "c:s:m:S:")) != -1) {

    switch (c) {
      case 'c':
        config_set_configfile(optarg);
        break;
      case 's':
        config_set_schemafile(optarg);
        break;
      case 'm':
        config_set_sys_modulesdir(optarg);
        break;
      case 'S':
        rng_set_position_seed(optarg);
        break;
      default: 
        return -1;
    }
  }

  return 0;
}


/* ************************************************** */
/* ************************************************** */
int do_init(void) {
  list_init();          /* list                      */
  heap_init();          /* heap                      */
  hashtable_init();     /* hashtable                 */
  sliding_window_init();
  circ_array_init();
  rng_init();           /* random number generator   */
  wiplan_parser_init(); /* wiplan config file parser */

  if (monitors_init()  ||
      timer_init()     ||
      noise_init()     ||
      nodes_init()     ||
      packet_init()) {
    return -1;
  }

  return 0;
}


/* ************************************************** */
/* ************************************************** */
int do_bootstrap(void) {
  if (scheduler_bootstrap()    ||
      environments_bootstrap() ||
      mobility_bootstrap()     ||
      nodes_bootstrap()        ||
      rng_bootstrap()          ||
      noise_bootstrap()        ||
      mediums_bootstrap()      ||
      simulation_bootstrap()){
    return -1;
  }

  return 0;
}


/* ************************************************** */
/* ************************************************** */
void do_clean(void) {
  scheduler_clean();
  monitors_clean();
  simulation_clean();
  noise_clean();
  nodes_clean();
  nodearchs_clean();
  groups_clean();
  grouparchs_clean();
  environments_clean();
  mediums_clean();
  classes_clean();
  rng_clean();
  wiplan_parser_clean();
  timer_clean();
  packet_clean();
  mem_fs_clean();
}

/* ************************************************** */
/* ************************************************** */
void do_end(void) {
  // unbind simulation classes
  simulation_unbind();

  // unbind node classes
  nodes_unbind();

  // unbind environment classes
  environments_unbind();

  // unbind medium classes
  mediums_unbind();

  // detroy classes
  classes_destroy();
}
