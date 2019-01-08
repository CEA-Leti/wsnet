/**
 *  \file   rng.h
 *  \brief  Random number generator declarations
 *  \author Luiz Henrique Suraty Filho
 *  \date   2018
 **/
#ifndef WSNET_CORE_INCLUDE_TOOLS_MATH_RNG_RNG_H_
#define WSNET_CORE_INCLUDE_TOOLS_MATH_RNG_RNG_H_

#include <kernel/include/definitions/types.h>

#ifdef __cplusplus
#include <random>
#include <typeinfo>
#include <iostream>
#include <unordered_set>
#include <type_traits>

/*
 * The random_number_generator is composed of an engine and a distribution
 *
 * An engine’s role is to return unpredictable (random) bits, ensuring that the likelihood of
 * next obtaining a 0 bit is always the same as the likelihood of next obtaining a 1 bit.
 *
 * A distribution ’s role is to return random numbers (variates) those likelihoods correspond to a specific shape.
 * E.g., a normal distribution produces variates according to a “bell-shaped curve.”
 */
template <class TEngineClass = std::default_random_engine
    , class TDistributionClass = std::uniform_int_distribution<unsigned long int>>
class random_number_generator {
 public:
  // types
  using seed_type = typename TEngineClass::result_type;
  using param_type = typename TDistributionClass::param_type;
  using result_type = typename TDistributionClass::result_type;

 private:
  TEngineClass engine_;
  TDistributionClass distribution_;

 public:
  // construct
  random_number_generator( ) = default;
  random_number_generator( seed_type seed ) : engine_{seed}, distribution_{} { };
  random_number_generator( TEngineClass engine ) : engine_(engine), distribution_{} { };

  // use compiler-generated copy/move/destroy

  // reinitialize
  random_number_generator& seed( seed_type seed ) {engine_.seed(seed); distribution_.reset(); return *this; }
  random_number_generator& randomize( ) { return seed( std::random_device{}() ); }

  template< class TParam0, class... TParam1toN >
  param_type param( TParam0&& p0, TParam1toN&&... p1ton ){
    param_type p = distribution_.param();
    distribution_.param(param_type( std::forward<TParam0>(p0) , std::forward<TParam1toN...>(p1ton...)));
    distribution_.reset();
    return p;
  }

  // produce random variate
  result_type operator () ( ) { return distribution_(engine_); }
  template< class TParam0, class... TParam1toN >
  result_type operator () ( TParam0 p0, TParam1toN... p1ton ) {
    return distribution_(engine_, param_type( std::forward<TParam0>(p0) , std::forward<TParam1toN...>(p1ton...)));
  }

  // observe
  TEngineClass&        engine( )  { return engine_; }
  TDistributionClass&  distribution( )  { return distribution_; }

  // equality-compare
  bool operator == ( random_number_generator const& other ) {
    return engine_ == other.engine_ and distribution_ == other.distribution_;
  }

  bool operator != ( random_number_generator const& other ){
    return not (*this == other);
  }
};

#endif

/**
 * ALL THE FOLLOWING FUNCTION ARE USED BY THE WSNET KERNEL WITH THE DEFAULT RNG. 
 **/
#ifdef __cplusplus
extern "C"{
#endif

/** 
 * \brief Return a random distance.
 * \return A random distance.
 **/
double get_random_distance(void);


/** 
 * \brief Return a random position on the x dimension.
 * \return A random position on the x dimension.
 **/
double get_random_x_position(void);


/** 
 * \brief Return a random position on the y dimension.
 * \return A random position on the y dimension.
 **/
double get_random_y_position(void);


/** 
 * \brief Return a random position on the z dimension.
 * \return A random position on the z dimension.
 **/
double get_random_z_position(void);

/**
 * \brief Return a random double value in [0,1[.
 * \return A random double value in [0,1[.
 **/
double get_random_double_position(void);

/** 
 * \brief Return a random double value in [0,1[.
 * \return A random double value in [0,1[.
 **/
double get_random_double(void);

/** 
 * \brief Return a random double value in [min,max[.
 * \param min the min value that can be drawn.
 * \param max the max value that can be drawn.
 * \return A random double value in [min,max[.
 **/
double get_random_double_range(double min, double max);


/** 
 * \brief Return a random integer value.
 * \return A random integer value.
 **/
int get_random_integer(void);


/** 
 * \brief Return a random integer value in [min,max].
 * \param min the min value that can be drawn.
 * \param max the max value that can be drawn.
 * \return A random integer value.
 **/
int get_random_integer_range(int min, int max);


/** 
 * \brief Return a random time in [0,simulation_end].
 * \return A random time [0,simulation_end].
 **/
uint64_t get_random_time(void);


/** 
 * \brief Return a random time in [min,max].
 * \param min the min value that can be drawn.
 * \param max the max value that can be drawn.
 * \return A random time in [min,max].
 **/
uint64_t get_random_time_range(uint64_t min, uint64_t max);


/** 
 * \brief Return a random node id different from a specified one.
 * \param exclusion the node identifier we do not want to be returned.
 * \return A random node identifier.
 **/
nodeid_t get_random_node(nodeid_t exclusion);

/**
 * \brief Get a random value X following a Gaussian distribution X ~ N(mu,sigma)
 * \parameter sigma is the sigma of the distribution
 * \parameter mu is the mean of the distribution
 * \return a random value X following a Gaussian distribution X ~ N(mu,sigma)
 */
double get_gaussian(double mu, double sigma);

/**
 * \brief set seed for RNG
 * \param the seed of the RNG
 **/
void rng_set_position_seed(char *seed);

/**
 * \brief set seed for RNG
 * \param the seed of the RNG
 **/
void rng_set_default_seed(char *seed);

/* init the default RNG */
int rng_init(void);

/* not used yet */
int rng_bootstrap(void);

/* clean all rngs, delete the hadas structure */
void rng_clean(void);

#ifdef __cplusplus
}
#endif

#endif // WSNET_CORE_INCLUDE_TOOLS_MATH_RNG_RNG_H_
