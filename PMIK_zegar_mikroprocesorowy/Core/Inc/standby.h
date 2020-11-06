/*
 * standby_init.h
 *
 *  Created on: Nov 4, 2020
 *      Author: michal
 */

#ifndef INC_STANDBY_H_
#define INC_STANDBY_H_

void to_do_after_wake_up_from_standby(void);
void to_do_before_going_to_standby(void);
void go_to_standby(void);

#endif /* INC_STANDBY_H_ */
