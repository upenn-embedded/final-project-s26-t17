/*
 * motor.c
 *
 *  Created on: Apr 8, 2026
 *      Author: david
 */


#include "motor.h"
#include "stm32f4xx.h"

#define PWM_MAX         999U

/*
 * Motor A direction pins
*/
#define M1_IN1_PIN      0U   /* PB0 */
#define M1_IN2_PIN      1U   /* PB1 */

/*
 * Motor B direction pins
 */
#define M2_IN1_PIN      10U  // PB10
#define M2_IN2_PIN      12U  // PB12

static void gpio_init_dir_pins(void);
static void pwm_init_tim4(void);
static void motor_set_dir_a_forward(void);
static void motor_set_dir_a_reverse(void);
static void motor_set_dir_a_stop(void);
static void motor_set_dir_b_forward(void);
static void motor_set_dir_b_reverse(void);
static void motor_set_dir_b_stop(void);
static void motor_set_pwm_a(uint16_t duty);
static void motor_set_pwm_b(uint16_t duty);

void motor_init(void)
{
    gpio_init_dir_pins();
    pwm_init_tim4();
    motor_stop_all();
}

void motor_set_command(motor_id_t motor, int16_t command)
{
    uint16_t duty;

    if (command > 100)  command = 100;
    if (command < -100) command = -100;

    duty = (uint16_t)((command >= 0 ? command : -command) * PWM_MAX / 100);

    if (motor == MOTOR_A)
    {
        if (command > 0)
        {
            motor_set_dir_a_forward();
            motor_set_pwm_a(duty);
        }
        else if (command < 0)
        {
            motor_set_dir_a_reverse();
            motor_set_pwm_a(duty);
        }
        else
        {
            motor_set_dir_a_stop();
            motor_set_pwm_a(0);
        }
    }
    else
    {
        if (command > 0)
        {
            motor_set_dir_b_forward();
            motor_set_pwm_b(duty);
        }
        else if (command < 0)
        {
            motor_set_dir_b_reverse();
            motor_set_pwm_b(duty);
        }
        else
        {
            motor_set_dir_b_stop();
            motor_set_pwm_b(0);
        }
    }
}

void motor_stop_all(void)
{
    motor_set_dir_a_stop();
    motor_set_dir_b_stop();
    motor_set_pwm_a(0);
    motor_set_pwm_b(0);
}

static void gpio_init_dir_pins(void)
{
    // Enable GPIOB clock
    RCC->AHB1ENR |= (1U << 1);

    // PB0, PB1, PB10, PB12 as general purpose output mode
    GPIOB->MODER &= ~(3U << (M1_IN1_PIN * 2));
    GPIOB->MODER |=  (1U << (M1_IN1_PIN * 2));

    GPIOB->MODER &= ~(3U << (M1_IN2_PIN * 2));
    GPIOB->MODER |=  (1U << (M1_IN2_PIN * 2));

    GPIOB->MODER &= ~(3U << (M2_IN1_PIN * 2));
    GPIOB->MODER |=  (1U << (M2_IN1_PIN * 2));

    GPIOB->MODER &= ~(3U << (M2_IN2_PIN * 2));
    GPIOB->MODER |=  (1U << (M2_IN2_PIN * 2));

    //* Push-pull */
    GPIOB->OTYPER &= ~(1U << M1_IN1_PIN);
    GPIOB->OTYPER &= ~(1U << M1_IN2_PIN);
    GPIOB->OTYPER &= ~(1U << M2_IN1_PIN);
    GPIOB->OTYPER &= ~(1U << M2_IN2_PIN);
}

static void pwm_init_tim4(void)
{
    //* Enable GPIOB and TIM4 clocks
    RCC->AHB1ENR |= (1U << 1);
    RCC->APB1ENR |= (1U << 2);

    // PB6, PB7 -> alternate function
    GPIOB->MODER &= ~(3U << (6 * 2));
    GPIOB->MODER |=  (2U << (6 * 2));

    GPIOB->MODER &= ~(3U << (7 * 2));
    GPIOB->MODER |=  (2U << (7 * 2));

    // AF2 for TIM4 on PB6/PB7 */
    GPIOB->AFR[0] &= ~(0xFU << (6 * 4));
    GPIOB->AFR[0] |=  (2U   << (6 * 4));

    GPIOB->AFR[0] &= ~(0xFU << (7 * 4));
    GPIOB->AFR[0] |=  (2U   << (7 * 4));

    /* Timer setup
       Assume 16 MHz timer clock.
       PSC = 15 -> 1 MHz timer tick
       ARR = 999 -> 1 kHz PWM
    */
    TIM4->PSC = 15;
    TIM4->ARR = PWM_MAX;

    // PWM mode 1 on CH1 and CH2
    TIM4->CCMR1 &= ~((7U << 4) | (7U << 12));
    TIM4->CCMR1 |=  (6U << 4);   /* OC1M = 110 */
    TIM4->CCMR1 |=  (6U << 12);  /* OC2M = 110 */

    // Enable preload
    TIM4->CCMR1 |= (1U << 3);    /* OC1PE */
    TIM4->CCMR1 |= (1U << 11);   /* OC2PE */

    // Enable CH1 and CH2 outputs
    TIM4->CCER |= (1U << 0);     /* CC1E */
    TIM4->CCER |= (1U << 4);     /* CC2E */

    // Initial duty = 0
    TIM4->CCR1 = 0;
    TIM4->CCR2 = 0;

    // Enable auto-reload preload
    TIM4->CR1 |= (1U << 7);      /* ARPE */

    // Force update event
    TIM4->EGR |= (1U << 0);

    // Start timer
    TIM4->CR1 |= (1U << 0);
}

static void motor_set_pwm_a(uint16_t duty)
{
    if (duty > PWM_MAX) duty = PWM_MAX;
    TIM4->CCR1 = duty;
}

static void motor_set_pwm_b(uint16_t duty)
{
    if (duty > PWM_MAX) duty = PWM_MAX;
    TIM4->CCR2 = duty;
}

static void motor_set_dir_a_forward(void)
{
    GPIOB->ODR |=  (1U << M1_IN1_PIN);
    GPIOB->ODR &= ~(1U << M1_IN2_PIN);
}

static void motor_set_dir_a_reverse(void)
{
    GPIOB->ODR &= ~(1U << M1_IN1_PIN);
    GPIOB->ODR |=  (1U << M1_IN2_PIN);
}

static void motor_set_dir_a_stop(void)
{
    GPIOB->ODR &= ~(1U << M1_IN1_PIN);
    GPIOB->ODR &= ~(1U << M1_IN2_PIN);
}

static void motor_set_dir_b_forward(void)
{
    GPIOB->ODR |=  (1U << M2_IN1_PIN);
    GPIOB->ODR &= ~(1U << M2_IN2_PIN);
}

static void motor_set_dir_b_reverse(void)
{
    GPIOB->ODR &= ~(1U << M2_IN1_PIN);
    GPIOB->ODR |=  (1U << M2_IN2_PIN);
}

static void motor_set_dir_b_stop(void)
{
    GPIOB->ODR &= ~(1U << M2_IN1_PIN);
    GPIOB->ODR &= ~(1U << M2_IN2_PIN);
}
