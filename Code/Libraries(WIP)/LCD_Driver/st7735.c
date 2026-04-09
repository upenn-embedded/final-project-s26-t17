/*
 * st7735.c
 *
 *  Created on: Apr 9, 2026
 *      Author: david
 */

#include "st7735.h"
#include "spi.h"
#include "stm32f4xx.h"
#include <stdint.h>

/* -------------------- ST7735 commands -------------------- */
#define ST7735_SWRESET  0x01
#define ST7735_SLPOUT   0x11
#define ST7735_COLMOD   0x3A
#define ST7735_MADCTL   0x36
#define ST7735_CASET    0x2A
#define ST7735_RASET    0x2B
#define ST7735_RAMWR    0x2C
#define ST7735_DISPON   0x29

/* -------------------- Control pins -------------------- */
/*
 * SPI pins are already fixed by your spi.c:
 * PA5 = SCK
 * PA6 = MISO
 * PA7 = MOSI
 * PA9 = CS
 *
 * Choose any free GPIO pins for D/C and RST.
 * These are placeholders. Change if needed.
 */
#define ST7735_DC_PORT   GPIOA
#define ST7735_DC_PIN    8U

#define ST7735_RST_PORT  GPIOA
#define ST7735_RST_PIN   10U

/* -------------------- Private helpers -------------------- */

static void delay_cycles(volatile uint32_t count)
{
    while (count--) {}
}

static void delay_ms(uint32_t ms)
{
    while (ms--)
    {
        delay_cycles(16000);
    }
}

static void st7735_gpio_init(void)
{
    // Enable GPIOA clock
    RCC->AHB1ENR |= (1U << 0);

    // D/C pin as output
    ST7735_DC_PORT->MODER &= ~(3U << (ST7735_DC_PIN * 2));
    ST7735_DC_PORT->MODER |=  (1U << (ST7735_DC_PIN * 2));

    // RST pin as output
    ST7735_RST_PORT->MODER &= ~(3U << (ST7735_RST_PIN * 2));
    ST7735_RST_PORT->MODER |=  (1U << (ST7735_RST_PIN * 2));

    // Default states
    ST7735_DC_PORT->ODR |=  (1U << ST7735_DC_PIN);
    ST7735_RST_PORT->ODR |= (1U << ST7735_RST_PIN);
}

static void dc_command(void)
{
    ST7735_DC_PORT->ODR &= ~(1U << ST7735_DC_PIN);
}

static void dc_data(void)
{
    ST7735_DC_PORT->ODR |= (1U << ST7735_DC_PIN);
}

static void rst_low(void)
{
    ST7735_RST_PORT->ODR &= ~(1U << ST7735_RST_PIN);
}

static void rst_high(void)
{
    ST7735_RST_PORT->ODR |= (1U << ST7735_RST_PIN);
}

static void st7735_reset(void)
{
    rst_high();
    delay_ms(10);

    rst_low();
    delay_ms(20);

    rst_high();
    delay_ms(20);
}

static void st7735_write_command(uint8_t cmd)
{
    dc_command();
    cs_enable();
    spi1_transmit(&cmd, 1);
    cs_disable();
}

static void st7735_write_data(const uint8_t *data, uint32_t size)
{
    dc_data();
    cs_enable();
    spi1_transmit((uint8_t *)data, size);
    cs_disable();
}

static void st7735_write_data8(uint8_t data)
{
    st7735_write_data(&data, 1);
}

static void st7735_write_data16(uint16_t data)
{
    uint8_t buf[2];
    buf[0] = (uint8_t)(data >> 8);
    buf[1] = (uint8_t)(data & 0xFF);
    st7735_write_data(buf, 2);
}

static void st7735_set_addr_window(uint16_t x0, uint16_t y0,
                                   uint16_t x1, uint16_t y1)
{
    uint8_t data[4];

    x0 += ST7735_XSTART;
    x1 += ST7735_XSTART;
    y0 += ST7735_YSTART;
    y1 += ST7735_YSTART;

    st7735_write_command(ST7735_CASET);
    data[0] = (uint8_t)(x0 >> 8);
    data[1] = (uint8_t)(x0 & 0xFF);
    data[2] = (uint8_t)(x1 >> 8);
    data[3] = (uint8_t)(x1 & 0xFF);
    st7735_write_data(data, 4);

    st7735_write_command(ST7735_RASET);
    data[0] = (uint8_t)(y0 >> 8);
    data[1] = (uint8_t)(y0 & 0xFF);
    data[2] = (uint8_t)(y1 >> 8);
    data[3] = (uint8_t)(y1 & 0xFF);
    st7735_write_data(data, 4);

    st7735_write_command(ST7735_RAMWR);
}

/* -------------------- Public functions -------------------- */

void st7735_init(void)
{
    spi_gpio_init();
    spi1_config();
    st7735_gpio_init();
    st7735_reset();

    st7735_write_command(ST7735_SWRESET);
    delay_ms(150);

    st7735_write_command(ST7735_SLPOUT);
    delay_ms(150);

    // 16-bit color mode
    st7735_write_command(ST7735_COLMOD);
    st7735_write_data8(0x05);
    delay_ms(10);

    /*
     * MADCTL: pin, ths will change the orientaion and base screen value
     */
    st7735_write_command(ST7735_MADCTL);
    st7735_write_data8(0xA0);

    st7735_write_command(ST7735_DISPON);
    delay_ms(100);

    st7735_fill_screen(ST7735_BLACK);
}

void st7735_draw_pixel(uint16_t x, uint16_t y, uint16_t color)
{
    if (x >= ST7735_WIDTH || y >= ST7735_HEIGHT)
    {
        return;
    }

    st7735_set_addr_window(x, y, x, y);
    st7735_write_data16(color);
}

void st7735_fill_rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color)
{
    uint32_t i;
    uint32_t pixels;
    uint8_t buf[2];

    if (x >= ST7735_WIDTH || y >= ST7735_HEIGHT)
    {
        return;
    }

    if ((x + w) > ST7735_WIDTH)
    {
        w = ST7735_WIDTH - x;
    }

    if ((y + h) > ST7735_HEIGHT)
    {
        h = ST7735_HEIGHT - y;
    }

    st7735_set_addr_window(x, y, x + w - 1, y + h - 1);

    buf[0] = (uint8_t)(color >> 8);
    buf[1] = (uint8_t)(color & 0xFF);

    dc_data();
    cs_enable();

    pixels = (uint32_t)w * (uint32_t)h;
    for (i = 0; i < pixels; i++)
    {
        spi1_transmit(buf, 2);
    }

    cs_disable();
}

void st7735_fill_screen(uint16_t color)
{
    st7735_fill_rect(0, 0, ST7735_WIDTH, ST7735_HEIGHT, color);
}

void st7735_draw_fast_hline(uint16_t x, uint16_t y, uint16_t w, uint16_t color)
{
    if (y >= ST7735_HEIGHT || x >= ST7735_WIDTH)
    {
        return;
    }

    if ((x + w) > ST7735_WIDTH)
    {
        w = ST7735_WIDTH - x;
    }

    st7735_fill_rect(x, y, w, 1, color);
}

void st7735_draw_fast_vline(uint16_t x, uint16_t y, uint16_t h, uint16_t color)
{
    if (x >= ST7735_WIDTH || y >= ST7735_HEIGHT)
    {
        return;
    }

    if ((y + h) > ST7735_HEIGHT)
    {
        h = ST7735_HEIGHT - y;
    }

    st7735_fill_rect(x, y, 1, h, color);
}

void st7735_draw_rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color)
{
    if (w == 0 || h == 0)
    {
        return;
    }

    st7735_draw_fast_hline(x, y, w, color);
    st7735_draw_fast_hline(x, y + h - 1, w, color);
    st7735_draw_fast_vline(x, y, h, color);
    st7735_draw_fast_vline(x + w - 1, y, h, color);
}
