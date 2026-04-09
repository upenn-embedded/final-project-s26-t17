/*
 * motor.h
 *
 *  Created on: Apr 8, 2026
 *      Author: david
 */

#ifndef MOTOR_H_
#define MOTOR_H_


#include <stdint.h>

typedef enum {
    MOTOR_A = 0,
    MOTOR_B
} motor_id_t;

void motor_init(void);

/* Command range: -100 to +100
   positive = forward
   negative = reverse
   zero     = stop
*/
void motor_set_command(motor_id_t motor, int16_t command);

void motor_stop_all(void);


#endif /* MOTOR_H_ */
