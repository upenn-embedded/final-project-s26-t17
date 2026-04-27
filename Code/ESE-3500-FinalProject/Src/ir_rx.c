#include "ir_rx.h"
#include "stm32f4xx.h"

#define IR_RX_PORT              GPIOB
#define IR_RX_PIN               9U

static volatile uint32_t ir_rx_last_edge_us = 0;
static volatile uint8_t ir_rx_latched = 0;

static uint32_t ir_rx_now_us(void)
{
    return TIM2->CNT;
}

void ir_rx_init(void)
{
    /* PB9 -> HS0038B digital output, active low */
    RCC->AHB1ENR |= (1U << 1);
    RCC->APB1ENR |= (1U << 0);
    RCC->APB2ENR |= (1U << 14);

    IR_RX_PORT->MODER &= ~(3U << (IR_RX_PIN * 2U));
    IR_RX_PORT->PUPDR &= ~(3U << (IR_RX_PIN * 2U));
    IR_RX_PORT->PUPDR |=  (1U << (IR_RX_PIN * 2U));

    TIM2->PSC = 15U;
    TIM2->ARR = 0xFFFFFFFFU;
    TIM2->CNT = 0;
    TIM2->CR1 |= (1U << 0);

    SYSCFG->EXTICR[2] &= ~(0xFU << 4);
    SYSCFG->EXTICR[2] |=  (0x1U << 4);

    EXTI->IMR  |= (1U << IR_RX_PIN);
    EXTI->RTSR &= ~(1U << IR_RX_PIN);
    EXTI->FTSR |=  (1U << IR_RX_PIN);

    NVIC_EnableIRQ(EXTI9_5_IRQn);
}

uint8_t ir_rx_carrier_detected(void)
{
    return (IR_RX_PORT->IDR & (1U << IR_RX_PIN)) == 0U;
}

uint8_t ir_rx_recent_activity(uint32_t window_us)
{
    if (ir_rx_carrier_detected())
    {
        return 1U;
    }

    if ((uint32_t)(ir_rx_now_us() - ir_rx_last_edge_us) <= window_us)
    {
        return 1U;
    }

    return 0U;
}

uint8_t ir_rx_event_latched(void)
{
    return ir_rx_latched;
}

void ir_rx_clear_event(void)
{
    ir_rx_latched = 0U;
}

void EXTI9_5_IRQHandler(void)
{
    if (EXTI->PR & (1U << IR_RX_PIN))
    {
        EXTI->PR = (1U << IR_RX_PIN);
        ir_rx_last_edge_us = ir_rx_now_us();
        ir_rx_latched = 1U;
    }
}
