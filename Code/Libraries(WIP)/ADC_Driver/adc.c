/*
 * adc.c
 *
 *  Created on: Apr 7, 2026
 *      Author: david
 */

#include "adc.h"
#include "stm32f4xx.h"


void adc_gpio_init(void) {
	//Enable GPIOA clock
    RCC->AHB1ENR |= (1U << 0);

    // PA1 -> analog mode (11)
    GPIOA->MODER &= ~(3U << (1 * 2));
    GPIOA->MODER |=  (3U << (1 * 2));

    // PA4 -> analog mode (11)
    GPIOA->MODER &= ~(3U << (4 * 2));
    GPIOA->MODER |=  (3U << (4 * 2));
}

 void adc_init(void) {
    // Enable ADC1 clock
    RCC->APB2ENR |= (1U << 8);

    // Turn ADC on
    ADC1->CR2 |= ADC_CR2_ADON;
}

uint16_t adc_read_channel(uint8_t channel) {
    // Sequence length = 1 conversion
    ADC1->SQR1 = 0;

    // Put selected channel in 1st slot
    ADC1->SQR3 = channel;

    // Start conversion
    ADC1->CR2 |= ADC_CR2_SWSTART;

    // Wait for conversion complete
    while ((ADC1->SR & ADC_SR_EOC) == 0) {
    }

    // Read result
    return (uint16_t)ADC1->DR;
}
