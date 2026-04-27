/*
 * joystick.c
 *
 *  Created on: Apr 8, 2026
 *      Author: david
 */


#include "joystick.h"
#include "adc.h"

#define ADC_MAX         4095U
#define ADC_CENTER      2048U
#define DEADZONE        150U

static int16_t adc_to_motor_command(uint16_t adc_val);

void joystick_init(void)
{
    adc_gpio_init();
    adc_init();
}

void joystick_read(joystick_data_t *data)
{
	data->raw_x = adc_read_channel(1);   // PA1

    data->raw_y = adc_read_channel(4);   // PA4


    data->motor_a_cmd = adc_to_motor_command(data->raw_x);
    data->motor_b_cmd = adc_to_motor_command(data->raw_y);
}

static int16_t adc_to_motor_command(uint16_t adc_val)
{
    int32_t offset;
    int32_t magnitude;

    offset = (int32_t)adc_val - (int32_t)ADC_CENTER;

    if (offset > -(int32_t)DEADZONE && offset < (int32_t)DEADZONE)
    {
        return 0;
    }

    if (offset > 0)
    {
        magnitude = (offset - DEADZONE) * 100 / ((int32_t)ADC_MAX - (int32_t)ADC_CENTER - DEADZONE);
        if (magnitude > 100) magnitude = 100;
        return (int16_t)magnitude;
    }
    else
    {
        magnitude = ((-offset) - DEADZONE) * 100 / ((int32_t)ADC_CENTER - DEADZONE);
        if (magnitude > 100) magnitude = 100;
        return (int16_t)(-magnitude);
    }
}
