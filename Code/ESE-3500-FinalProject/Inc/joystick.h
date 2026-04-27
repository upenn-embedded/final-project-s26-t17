/*
 * joystick.h
 *
 *  Created on: Apr 8, 2026
 *      Author: david
 */

#ifndef JOYSTICK_H_
#define JOYSTICK_H_

#include <stdint.h>

typedef struct {
    uint16_t raw_x;
    uint16_t raw_y;
    int16_t  motor_a_cmd;
    int16_t  motor_b_cmd;
} joystick_data_t;

void joystick_init(void);
void joystick_read(joystick_data_t *data);

#endif /* JOYSTICK_H_ */
