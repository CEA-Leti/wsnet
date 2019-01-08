/**
 *  \file   linear_battery_energy.c
 *  \brief  linear_battery_energy description
 *  \author linear_battery_energy
 *  \date   16-11-2016
 **/

#include "linear_battery_energy.h"

#include <stdio.h>
#include <kernel/include/modelutils.h>


/** \brief A structure to define the simulation module information
 *  \struct model_t
 **/
model_t model =  {
		(char*) "linear_battery_energy description",
		(char*) "Luiz Henrique SURATY FILHO",
		(char*) "0.1",
		MODELTYPE_ENERGY
};

/* ************************************************** */
/* ************************************************** */

/** \brief The init input-interface to read/initialize the GLOBAL class variables (from the XML configuration file) at the simulation startup.
 *  \fn int init(call_t *to, void *params)
 *  \param to is a pointer to the called class
 *  \param params is a pointer to the parameters read from the XML configuration file
 *  \return SUCCESSFUL if success, UNSUCCESSFUL otherwise
 **/
int init(call_t *to, void *params) {
  linear_battery_energy_class_private_t *classdata = (linear_battery_energy_class_private_t *) malloc(sizeof(linear_battery_energy_class_private_t));
  param_t *param;

  // Default values
  classdata->global_consumption_J    = 0;

  // Get module parameters from the XML configuration file
  list_init_traverse((list_t*) params);
  while ((param = (param_t *) list_traverse( (list_t*) params)) != NULL) {

    if (!strcmp(param->key, "global_consumption_J")) {
      if (get_param_double(param->value, &(classdata->global_consumption_J))) {
        free(classdata);
        return UNSUCCESSFUL;
      }
    }
  }


  // Assign the GLOBAL class variables
  set_class_private_data(to, classdata);

  return SUCCESSFUL;
}

/** \brief The destroy input-interface to deallocate/free the GLOBAL class variables at the end of simulation.
 *  \fn int destroy(call_t *to)
 *  \param to is a pointer to the called class
 *  \return SUCCESSFUL if success, UNSUCCESSFUL otherwise
 **/
int destroy(call_t *to) {
  linear_battery_energy_class_private_t *classdata = (linear_battery_energy_class_private_t *) get_class_private_data(to);
  free(classdata);
  classdata = NULL;
  set_class_private_data(to, classdata);
  return SUCCESSFUL;
}

/** \brief The bind input-interface to read/initialize the LOCAL node variables (from the XML configuration file) at the simulation startup.
 *  \fn int bind(call_t *to, void *params)
 *  \param to is a pointer to the called class
 *  \param params is a pointer to the parameters read from the XML configuration file
 *  \return SUCCESSFUL if success, UNSUCCESSFUL otherwise
 **/
int bind(call_t *to, void *params) {
  linear_battery_energy_node_private_t *nodedata = (linear_battery_energy_node_private_t *) malloc(sizeof(linear_battery_energy_node_private_t));
  param_t *param;

  // Default values
  nodedata->battery_capacity_mAh  = 1000;
  nodedata->consumed_energy_J     = 0;
  nodedata->recharged_energy_J    = 0;
  nodedata->voltage_V             = 3;
  nodedata->battery_capacity_J    = 10800;
  nodedata->available_energy_J    = -1;

  // Get module parameters from the XML configuration file
  list_init_traverse((list_t*) params);
  while ((param = (param_t *) list_traverse( (list_t*) params)) != NULL) {

    if (!strcmp(param->key, "available_energy_J")) {
      if (get_param_double(param->value, &(nodedata->available_energy_J))) {
        free(nodedata);
        return UNSUCCESSFUL;
      }
    }
    if (!strcmp(param->key, "battery_capacity_mAh")) {
      if (get_param_double(param->value, &(nodedata->battery_capacity_mAh))) {
        free(nodedata);
        return UNSUCCESSFUL;
      }
    }
    if (!strcmp(param->key, "voltage_V")) {
      if (get_param_double(param->value, &(nodedata->voltage_V))) {
        free(nodedata);
        return UNSUCCESSFUL;
      }
    }
  }

  // (1mWh = 3.6J)
  nodedata->battery_capacity_J   = nodedata->battery_capacity_mAh * nodedata->voltage_V * 3.6;

  // User did not define a value
  if (nodedata->available_energy_J == -1)
    nodedata->available_energy_J = nodedata->battery_capacity_J;

  // Assign the initialized local variables to the node
  set_node_private_data(to, nodedata);

  return SUCCESSFUL;
}


/** \brief The unbind input-interface to deallocate/free the LOCAL node variables at the end of simulation.
 *  \fn int unbind(call_t *to)
 *  \param to is a pointer to the called class
 *  \return SUCCESSFUL if success, UNSUCCESSFUL otherwise
 **/
int unbind(call_t *to) {
  linear_battery_energy_node_private_t *nodedata = (linear_battery_energy_node_private_t *) get_node_private_data(to);

  printf("[ENERGY] Node %d consumption=%.20f J\n", to->object, nodedata->consumed_energy_J);

  free(nodedata);
  nodedata=NULL;
  set_node_private_data(to, nodedata);
  return SUCCESSFUL;
}

/** \brief The bootstrap input-interface which is automatically called by the WSNet simulation core at the beginning of the simulation.
 *  \fn int bootstrap(call_t *to)
 *  \param to is a pointer to the called class
 *  \return SUCCESSFUL if success, UNSUCCESSFUL otherwise
 **/
int bootstrap(call_t *to) {
  (void) to;
  return SUCCESSFUL;
}

/** \brief The ioctl is called whenever other module calls IOCTL with the to pointing to this module
 *  \fn int ioctl(call_t *to, int option, void *in, void **out)
 *  \param to is a pointer to the called class
 *  \param option indicates an option to be used by the ioctl
 *  \param in is the input address of a variable, when the function who invoked pass you a value
 *  \param out is the address for the, when you want to save an output for the function who invoked
 *  \return SUCCESSFUL if success, UNSUCCESSFUL otherwise
 **/
int ioctl(call_t *to, int option, void *in, void **out) {
  (void) to;
  (void) option;
  (void) in;
  (void) out;
  return SUCCESSFUL;
}

/* ************************************************** */
/* ************************************************** */

/** \brief The recharge function can be called to indicate energy consumption
 *  \fn void recharge(call_t *to, double energy_J)
 *  \param to is a pointer to the called class
 *  \param energy_J indicates the amount of energy to be recharged (in J)
 **/
void recharge(call_t *to, double energy_J) {
  linear_battery_energy_node_private_t *nodedata = (linear_battery_energy_node_private_t *) get_node_private_data(to);
  nodedata->available_energy_J = fmin(nodedata->battery_capacity_J,nodedata->available_energy_J+energy_J);
  nodedata->recharged_energy_J += energy_J;
  return;
}

/** \brief The consume can be called to indicate energy consumption
 *  \fn void consume(call_t *to, double current_mA, uint64_t duration_ns)
 *  \param to is a pointer to the called class
 *  \param current_mA indicates the current used (in mA)
 *  \param duration_ns indicates the duration on which it stayed ON (in ns)
 **/
void consume(call_t *to, double current_mA, uint64_t duration_ns) {
  linear_battery_energy_node_private_t *nodedata = (linear_battery_energy_node_private_t *) get_node_private_data(to);

  // 1J = 1 watt second = 1 A * V * s
  // Duration is in nanoseconds, we need to transfer it to seconds
  // Current is in mA, we need to convert it to A
  double energy_draw_J =  TIME_TO_SECONDS(duration_ns) * (current_mA * 1e-3) * nodedata->voltage_V;
  nodedata->available_energy_J -= energy_draw_J;
  nodedata->consumed_energy_J += energy_draw_J;
  if (nodedata->available_energy_J <= 0) {
      nodedata->available_energy_J = 0;
      node_kill(to->object);
  }
  return;
}

/** \brief The energy_consumed is used to verify how much energy have been used
 *  \fn double energy_consumed(call_t *to)
 *  \param to is a pointer to the called class
 *  \return the total amount of consumed energy (in J)
 **/
double energy_consumed(call_t *to) {
  linear_battery_energy_node_private_t *nodedata = (linear_battery_energy_node_private_t *) get_node_private_data(to);
  return nodedata->consumed_energy_J;
}

/** \brief The energy_recharged is used to verify how much energy have been recharged
 *  \fn double energy_recharged(call_t *to)
 *  \param to is a pointer to the called class
 *  \return the total amount of recharged energy (in J)
 **/
double energy_recharged(call_t *to) {
  linear_battery_energy_node_private_t *nodedata = (linear_battery_energy_node_private_t *) get_node_private_data(to);
  return nodedata->recharged_energy_J;
}

/** \brief The energy_remaining is used to verify how much energy is still remaining
 *  \fn double energy_remaining(call_t *to)
 *  \param to is a pointer to the called class
 *  \return the total amount remaning (in J)
 **/
double energy_remaining(call_t *to) {
  linear_battery_energy_node_private_t *nodedata = (linear_battery_energy_node_private_t *) get_node_private_data(to);
  return nodedata->available_energy_J;
}

/** \brief The energy_status is used to verify the status of the battery in percentage
 *  \fn double energy_status(call_t *to)
 *  \param to is a pointer to the called class
 *  \return the status of the battery (in percentage)
 **/
double energy_status(call_t *to) {
  linear_battery_energy_node_private_t *nodedata = (linear_battery_energy_node_private_t *) get_node_private_data(to);

  if (nodedata->battery_capacity_J == 0)
    return 0;

  double status = nodedata->available_energy_J / nodedata->battery_capacity_J;
  if (nodedata->available_energy_J <= 0) {
      return 0;
  } else if ((status >= 0) && (status <= 1)) {
      return status;
  } else {
      return 0;
  }

}

/** \brief The get_supply_voltage is used to get the supplied voltage
 *  \fn double get_supply_voltage(call_t *to)
 *  \param to is a pointer to the called class
 *  \return the voltage (in V)
 **/
double get_supply_voltage(call_t *to){
  linear_battery_energy_node_private_t *nodedata = (linear_battery_energy_node_private_t *) get_node_private_data(to);

  return nodedata->voltage_V;
}


/* ************************************************** */
/* ************************************************** */
energy_methods_t methods = {recharge,
    consume,
    energy_consumed,
    energy_recharged,
    energy_remaining,
    energy_status,
    get_supply_voltage};
