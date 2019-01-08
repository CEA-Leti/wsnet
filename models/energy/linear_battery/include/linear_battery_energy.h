/*
 *  \file   linear_battery_energy.h
 *  \brief  linear_battery_energy description - header
 *  \author Luiz-Henrique SURATY-FILHO
 *  \date   16-11-2016
 */

#ifndef ENERGY_LINEAR_BATTERY_ENERGY_H_
#define ENERGY_LINEAR_BATTERY_ENERGY_H_


/* ************************************************** */
/* ************************************************** */

/** \brief A structure to define the LOCAL node variables
 *  \struct linear_battery_energy_node_private_t
 **/
typedef struct _linear_battery_energy_node_private {
    double available_energy_J;      /*!< Defines the current available energy in the battery in joules(J) (1mWh = 3.6J)*/
    double battery_capacity_mAh;    /*!< Defines the maximum capacity of the battery in milliampere-hour(mAh) */
    double battery_capacity_J;      /*!< Defines the initial energy capacity of the battery in joules(J) */
    double consumed_energy_J;       /*!< Defines the energy that has been consumed from the battery in joules (J) */
    double recharged_energy_J;      /*!< Defines the energy that has been recharged to the battery in joules (J) */
    double voltage_V;               /*!< Defines the voltage_V used by battery in volts (V) */
}linear_battery_energy_node_private_t;

/** \brief A structure to define the GLOBAL class variables
 *  \struct linear_battery_energy_class_private_t
 **/
typedef struct _linear_battery_energy_class_private {
    double global_consumption_J;    /*!< Defines the global consumption in joules (J)*/
}linear_battery_energy_class_private_t;


/* ************************************************** */
/* ************************************************** */

#endif  // ENERGY_LINEAR_BATTERY_ENERGY_
