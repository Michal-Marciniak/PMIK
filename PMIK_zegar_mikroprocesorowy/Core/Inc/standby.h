/*
 * standby_init.h
 *
 *  Created on: Nov 4, 2020
 *      Author: michal
 */

#ifndef INC_STANDBY_H_
#define INC_STANDBY_H_

/**
 * @brief Funkcja odpowiedzialna za obsługę programu, po wybudzeniu układu z tryby STANDBY
 */
void to_do_after_wake_up_from_standby(void);

/**
 * @brief Funkcja odpowiedzialna za obsługę programu, przed wejściem w tryb STANDBY
 */
void to_do_before_going_to_standby(void);

/**
 * @brief Funkcja odpowiedzialna za wejście układu w tryb STANDBY
 */
void go_to_standby(void);

#endif /* INC_STANDBY_H_ */
