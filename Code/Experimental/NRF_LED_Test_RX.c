#include "nrf24.h"
#include "stm32f4xx.h"
#include <stdint.h>

#define TEST_LED_PORT GPIOC
#define TEST_LED_PIN  8U

static const uint8_t nrf_addr[NRF24_ADDR_LEN] = { 'C', 'A', 'R', '0', '1' };

static void delay_cycles(volatile uint32_t count)
{
    while (count--)
    {
    }
}

static void test_led_init(void)
{
    RCC->AHB1ENR |= (1U << 2);

    TEST_LED_PORT->MODER &= ~(3U << (TEST_LED_PIN * 2));
    TEST_LED_PORT->MODER |=  (1U << (TEST_LED_PIN * 2));
    TEST_LED_PORT->OTYPER &= ~(1U << TEST_LED_PIN);
    TEST_LED_PORT->ODR &= ~(1U << TEST_LED_PIN);
}

static void test_led_toggle(void)
{
    TEST_LED_PORT->ODR ^= (1U << TEST_LED_PIN);
}

static void blink_not_connected(void)
{
    test_led_toggle();
    delay_cycles(200000);
}

int main(void)
{
    uint8_t payload[4];

    test_led_init();
    nrf24_init();

    if (!nrf24_is_connected())
    {
        while (1)
        {
            blink_not_connected();
        }
    }

    nrf24_config_rx(nrf_addr, 76, sizeof(payload));

    while (1)
    {
        if (nrf24_data_ready())
        {
            nrf24_read_payload(payload, sizeof(payload));
            test_led_toggle();
            delay_cycles(300000);
        }
    }
}
