/**
 *  \file   energy.h
 *  \brief  Energy declarations
 *  \author Luiz Henrique Suraty Filho
 *  \date   2016
 **/
#ifndef WSNET_KERNEL_MODEL_HANDLERS_ENERGY_H_
#define WSNET_KERNEL_MODEL_HANDLERS_ENERGY_H_

#include <kernel/include/definitions/types.h>

#ifdef __cplusplus
extern "C"{
#endif
/** \brief The recharge function can be called to indicate energy consumption
 *  \fn void energy_recharge(call_t *to, double energy_J)
 *  \param to is a pointer to the called class
 *  \param energy_J indicates the amount of energy to be recharged (in J)
 **/
void energy_recharge(call_t *to, double energy_J);

/** \brief The consume can be called to indicate energy consumption
 *  \fn void energy_consume(call_t *to, double current_mA, uint64_t duration_ns)
 *  \param to is a pointer to the called class
 *  \param current_mA indicates the current used (in mA)
 *  \param duration_ns indicates the duration on which it stayed ON (in ns)
 **/
void energy_consume(call_t *to, double current_mA, uint64_t duration_ns);


/** \brief The energy_check_energy_consumed is used to verify how much energy have been used
 *  \fn double energy_check_energy_consumed(call_t *to)
 *  \param to is a pointer to the called class
 *  \return the total amount of consumed energy (in J)
 **/
double energy_check_energy_consumed(call_t *to);

/** \brief The energy_check_energy_recharged is used to verify how much energy have been recharged
 *  \fn double energy_check_energy_recharged(call_t *to)
 *  \param to is a pointer to the called class
 *  \return the total amount of recharged energy (in J)
 **/
double energy_check_energy_recharged(call_t *to);


/** \brief The energy_check_energy_remaining is used to verify how much energy is still remaining
 *  \fn double energy_check_energy_remaining(call_t *to)
 *  \param to is a pointer to the called class
 *  \return the total amount remaning (in J)
 **/
double energy_check_energy_remaining(call_t *to);


/** \brief The energy_status is used to verify the status of the battery in percentage
 *  \fn double energy_status(call_t *to)
 *  \param to is a pointer to the called class
 *  \return the status of the battery (in percentage)
 **/
double energy_check_energy_status(call_t *to);

/** \brief The energy_get_supply_voltage is used to get the supplied voltage
 *  \fn double energy_get_supply_voltage(call_t *to)
 *  \param to is a pointer to the called class
 *  \return the voltage (in V)
 **/
double energy_get_supply_voltage(call_t *to);

#ifdef __cplusplus
}
#endif

#endif // WSNET_KERNEL_MODEL_HANDLERS_ENERGY_H_
