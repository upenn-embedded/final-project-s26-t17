#include "ir_tx.h"
#include "stm32f4xx.h"

#define IR_TX_TIMER_PERIOD       420U
#define IR_TX_TIMER_DUTY         140U

static void delay_cycles(volatile uint32_t count)
{
    while (count--)
    {
    }
}

static void delay_us(uint32_t us)
{
    while (us--)
    {
        delay_cycles(16U);
    }
}

void ir_tx_init(void)
{
    /* PC7 -> TIM3_CH2 -> TSAL6200 transistor driver input */
    RCC->AHB1ENR |= (1U << 2);
    RCC->APB1ENR |= (1U << 1);

    GPIOC->MODER &= ~(3U << (7U * 2U));
    GPIOC->MODER |=  (2U << (7U * 2U));

    GPIOC->OTYPER &= ~(1U << 7U);
    GPIOC->OSPEEDR |= (3U << (7U * 2U));

    GPIOC->AFR[0] &= ~(0xFU << (7U * 4U));
    GPIOC->AFR[0] |=  (2U   << (7U * 4U));

    TIM3->CR1 = 0;
    TIM3->PSC = 0;
    TIM3->ARR = IR_TX_TIMER_PERIOD;
    TIM3->CCR2 = 0;

    TIM3->CCMR1 &= ~(7U << 12);
    TIM3->CCMR1 |=  (6U << 12);
    TIM3->CCMR1 |=  (1U << 11);

    TIM3->CCER &= ~(1U << 5);
    TIM3->CCER |=  (1U << 4);

    TIM3->EGR |= (1U << 0);
    TIM3->CR1 |= (1U << 7);
    TIM3->CR1 |= (1U << 0);
}

void ir_tx_start_carrier(void)
{
    TIM3->CCR2 = IR_TX_TIMER_DUTY;
}

void ir_tx_stop_carrier(void)
{
    TIM3->CCR2 = 0;
}

void ir_tx_send_burst_us(uint32_t burst_us)
{
    ir_tx_start_carrier();
    delay_us(burst_us);
    ir_tx_stop_carrier();
}
