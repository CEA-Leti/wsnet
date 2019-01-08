/**
 *  \file   rng.cc
 *  \brief  Random Number Generator
 *  \author Luiz Henrique Suraty Filho
 *  \date   2018
 **/

#include <kernel/include/tools/math/rng/rng.h>
#include <kernel/include/model_handlers/topology.h>
#include <kernel/include/definitions/node.h>
#include <kernel/include/scheduler/scheduler.h>

/**
 *default RNG parameters, set in main
 **/
static unsigned long int default_rng_seed = 0;
static unsigned long int position_rng_seed = 0;
static bool default_rng_seed_is_set = false;
static bool position_rng_seed_is_set = false;

/**
 * We use this approach to share the default engine with several distribution
 * Each distribution is inside the specific function
 * Another approach could be to create rngs that would be assigned to each function.
 * We could have a rng for int, a rng for double, a rng for time, etc ...
 * Thus, we would have more granular control to the seed of each rng
 * However, in this way they would not share the engine, and will have different engines.
 *
 */
static std::default_random_engine &default_rng_engine() {
  static std::default_random_engine default_rng_engine {};
  return default_rng_engine;
}

// RNG dedicated to the generation of position-related variates
static random_number_generator<std::default_random_engine, std::uniform_real_distribution<double> > rng_position;

// set the position seed
void rng_set_position_seed(char *seed){
  position_rng_seed = atol(seed);
  position_rng_seed_is_set = true;
}

void rng_set_default_seed(char *seed){
  default_rng_seed = atol(seed);
  default_rng_seed_is_set = true;
}

int rng_init(void) {
  default_rng_engine().seed(std::random_device{}());
  rng_position.seed(std::random_device{}());
  return 0;
}

int rng_bootstrap(void) {
  if (default_rng_seed_is_set){
    default_rng_engine().seed(default_rng_seed);
  }

  if (position_rng_seed_is_set){
    rng_position.seed(position_rng_seed);
  }

  return 0;
}

void rng_clean(void) {
  return;
}

/******************************************************************************/
/******************************************************************************/

uint64_t get_random_time(void) {
  static std::uniform_int_distribution<uint64_t> distribution{};
  return distribution(default_rng_engine(), decltype(distribution)::param_type{0, scheduler_get_end()} );
}

uint64_t get_random_time_range(uint64_t min, uint64_t max){
  static std::uniform_int_distribution<uint64_t> distribution{};
  return distribution(default_rng_engine(), decltype(distribution)::param_type{min, max} );
}

double get_random_distance(void) {
  double max = sqrt(get_topology_area()->x * get_topology_area()->x +
                    get_topology_area()->y * get_topology_area()->y +
                    get_topology_area()->z * get_topology_area()->z);
  return rng_position(0.0,max);
}

double get_random_x_position(void) {
  return rng_position(0.0,get_topology_area()->x);
}

double get_random_y_position(void) {
  return rng_position(0.0,get_topology_area()->y);
}

double get_random_z_position(void) {
  return rng_position(0.0,get_topology_area()->z);
}

double get_random_double_position(void) {
  return rng_position(0.0,1.0);
}

double get_random_double(void) {
  static std::uniform_real_distribution<double> distribution{};
  return distribution(default_rng_engine(), decltype(distribution)::param_type{0.0, 1.0} );
}

double get_random_double_range(double min, double max) {
  static std::uniform_real_distribution<double> distribution{};
  return distribution(default_rng_engine(), decltype(distribution)::param_type{min, max} );
}

int get_random_integer(void) {
  static std::uniform_int_distribution<int> distribution{};
  return distribution(default_rng_engine());
}

int get_random_integer_range(int min, int max) {
  static std::uniform_int_distribution<int> distribution{};
  return distribution(default_rng_engine(), decltype(distribution)::param_type{min, max} );
}

nodeid_t get_random_node(nodeid_t exclusion) {
  nodeid_t node;
  do {
    node = get_random_integer_range(0,nodes.size);
  } while (node == exclusion);
  return node;
}

double get_gaussian(double mu, double sigma){
  static std::normal_distribution<double> distribution{};
  return distribution(default_rng_engine(), decltype(distribution)::param_type{mu, sigma} );
}
