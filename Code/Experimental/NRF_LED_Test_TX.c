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

static void test_led_on(void)
{
    TEST_LED_PORT->ODR |= (1U << TEST_LED_PIN);
}

static void test_led_off(void)
{
    TEST_LED_PORT->ODR &= ~(1U << TEST_LED_PIN);
}

static void blink_success(void)
{
    test_led_on();
    delay_cycles(500000);
    test_led_off();
    delay_cycles(500000);
}

static void blink_error(void)
{
    uint8_t i;

    for (i = 0; i < 2; i++)
    {
        test_led_on();
        delay_cycles(200000);
        test_led_off();
        delay_cycles(200000);
    }

    delay_cycles(600000);
}

int main(void)
{
    uint8_t payload[4] = { 0xAA, 0x55, 0x00, 0x00 };

    test_led_init();
    nrf24_init();

    if (!nrf24_is_connected())
    {
        while (1)
        {
            blink_error();
        }
    }

    nrf24_config_tx(nrf_addr, 76);

    while (1)
    {
        payload[2]++;

        if (nrf24_send(payload, sizeof(payload)) == 0)
        {
            blink_success();
        }
        else
        {
            blink_error();
        }

        delay_cycles(1500000);
    }
}
