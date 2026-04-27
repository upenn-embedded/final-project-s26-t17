/*
 * nrf24.c
 *
 *  Created on: Apr 14, 2026
 *      Author: david
 */


#include "nrf24.h"
#include "spi.h"
#include "stm32f4xx.h"
#include <stdint.h>

/* -------------------- Pin configuration -------------------- */
/*
 * SPI bus comes from spi.c:
 * PA5 = SCK
 * PA6 = MISO
 * PA7 = MOSI
 *
 * NRF-specific control pins:
 * PB8 = CSN
 * PB5 = CE
 */
#define NRF24_CSN_PORT      GPIOB
#define NRF24_CSN_PIN       8U

#define NRF24_CE_PORT       GPIOB
#define NRF24_CE_PIN        5U

/* -------------------- Commands -------------------- */
#define NRF24_CMD_R_REGISTER       0x00
#define NRF24_CMD_W_REGISTER       0x20
#define NRF24_CMD_R_RX_PAYLOAD     0x61
#define NRF24_CMD_W_TX_PAYLOAD     0xA0
#define NRF24_CMD_FLUSH_TX         0xE1
#define NRF24_CMD_FLUSH_RX         0xE2
#define NRF24_CMD_NOP              0xFF

/* -------------------- Registers -------------------- */
#define NRF24_REG_CONFIG           0x00
#define NRF24_REG_EN_AA            0x01
#define NRF24_REG_EN_RXADDR        0x02
#define NRF24_REG_SETUP_AW         0x03
#define NRF24_REG_SETUP_RETR       0x04
#define NRF24_REG_RF_CH            0x05
#define NRF24_REG_RF_SETUP         0x06
#define NRF24_REG_STATUS           0x07
#define NRF24_REG_RX_ADDR_P0       0x0A
#define NRF24_REG_RX_ADDR_P1       0x0B
#define NRF24_REG_RX_ADDR_P2       0x0C
#define NRF24_REG_RX_ADDR_P3       0x0D
#define NRF24_REG_RX_ADDR_P4       0x0E
#define NRF24_REG_RX_ADDR_P5       0x0F
#define NRF24_REG_TX_ADDR          0x10
#define NRF24_REG_RX_PW_P0         0x11
#define NRF24_REG_RX_PW_P1         0x12
#define NRF24_REG_RX_PW_P2         0x13
#define NRF24_REG_RX_PW_P3         0x14
#define NRF24_REG_RX_PW_P4         0x15
#define NRF24_REG_RX_PW_P5         0x16
#define NRF24_REG_FIFO_STATUS      0x17

/* -------------------- CONFIG bits -------------------- */
#define NRF24_CONFIG_MASK_RX_DR    (1U << 6)
#define NRF24_CONFIG_MASK_TX_DS    (1U << 5)
#define NRF24_CONFIG_MASK_MAX_RT   (1U << 4)
#define NRF24_CONFIG_EN_CRC        (1U << 3)
#define NRF24_CONFIG_CRCO          (1U << 2)
#define NRF24_CONFIG_PWR_UP        (1U << 1)
#define NRF24_CONFIG_PRIM_RX       (1U << 0)

/* -------------------- STATUS bits -------------------- */
#define NRF24_STATUS_RX_DR         (1U << 6)
#define NRF24_STATUS_TX_DS         (1U << 5)
#define NRF24_STATUS_MAX_RT        (1U << 4)

/* -------------------- FIFO bits -------------------- */
#define NRF24_FIFO_RX_EMPTY        (1U << 0)

/* -------------------- Private state -------------------- */
static uint8_t nrf24_payload_size = 32U;

/* -------------------- Private helpers -------------------- */

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

static void delay_ms(uint32_t ms)
{
    while (ms--)
    {
        delay_us(1000U);
    }
}

static void nrf24_csn_low(void)
{
    NRF24_CSN_PORT->ODR &= ~(1U << NRF24_CSN_PIN);
}

static void nrf24_csn_high(void)
{
    NRF24_CSN_PORT->ODR |= (1U << NRF24_CSN_PIN);
}

static void nrf24_ce_low(void)
{
    NRF24_CE_PORT->ODR &= ~(1U << NRF24_CE_PIN);
}

static void nrf24_ce_high(void)
{
    NRF24_CE_PORT->ODR |= (1U << NRF24_CE_PIN);
}

static uint8_t nrf24_pipe_payload_reg(uint8_t pipe)
{
    switch (pipe)
    {
        case 0: return NRF24_REG_RX_PW_P0;
        case 1: return NRF24_REG_RX_PW_P1;
        case 2: return NRF24_REG_RX_PW_P2;
        case 3: return NRF24_REG_RX_PW_P3;
        case 4: return NRF24_REG_RX_PW_P4;
        case 5: return NRF24_REG_RX_PW_P5;
        default: return NRF24_REG_RX_PW_P0;
    }
}

static void nrf24_write_buffer(uint8_t reg, const uint8_t *data, uint8_t len)
{
    uint8_t i;

    spi1_set_mode0();

    nrf24_csn_low();
    spi1_exchange_byte(NRF24_CMD_W_REGISTER | (reg & 0x1F));

    for (i = 0; i < len; i++)
    {
        spi1_exchange_byte(data[i]);
    }

    nrf24_csn_high();
}

static void nrf24_read_buffer(uint8_t reg, uint8_t *data, uint8_t len)
{
    uint8_t i;

    spi1_set_mode0();

    nrf24_csn_low();
    spi1_exchange_byte(NRF24_CMD_R_REGISTER | (reg & 0x1F));

    for (i = 0; i < len; i++)
    {
        data[i] = spi1_exchange_byte(NRF24_CMD_NOP);
    }

    nrf24_csn_high();
}

static void nrf24_write_payload(const uint8_t *data, uint8_t len)
{
    uint8_t i;

    spi1_set_mode0();

    nrf24_csn_low();
    spi1_exchange_byte(NRF24_CMD_W_TX_PAYLOAD);

    for (i = 0; i < len; i++)
    {
        spi1_exchange_byte(data[i]);
    }

    nrf24_csn_high();
}

static void nrf24_read_payload(uint8_t *data, uint8_t len)
{
    uint8_t i;

    spi1_set_mode0();

    nrf24_csn_low();
    spi1_exchange_byte(NRF24_CMD_R_RX_PAYLOAD);

    for (i = 0; i < len; i++)
    {
        data[i] = spi1_exchange_byte(NRF24_CMD_NOP);
    }

    nrf24_csn_high();
}

static void nrf24_command(uint8_t cmd)
{
	spi1_set_mode0();

    nrf24_csn_low();
    spi1_exchange_byte(cmd);
    nrf24_csn_high();
}

static void nrf24_clear_interrupts(void)
{
    nrf24_write_register(NRF24_REG_STATUS,
                         NRF24_STATUS_RX_DR |
                         NRF24_STATUS_TX_DS |
                         NRF24_STATUS_MAX_RT);
}

static void nrf24_power_up(void)
{
    uint8_t config;

    config = nrf24_read_register(NRF24_REG_CONFIG);
    config |= NRF24_CONFIG_PWR_UP;
    nrf24_write_register(NRF24_REG_CONFIG, config);

    delay_ms(5);
}

static void nrf24_set_tx_mode(void)
{
    uint8_t config;

    config = nrf24_read_register(NRF24_REG_CONFIG);
    config &= ~NRF24_CONFIG_PRIM_RX;
    nrf24_write_register(NRF24_REG_CONFIG, config);

    delay_us(150);
}

static void nrf24_set_rx_mode(void)
{
    uint8_t config;

    config = nrf24_read_register(NRF24_REG_CONFIG);
    config |= NRF24_CONFIG_PRIM_RX;
    nrf24_write_register(NRF24_REG_CONFIG, config);

    delay_us(150);
}

/* -------------------- Public functions -------------------- */

void nrf24_gpio_init(void)
{
    /* Enable GPIOB clock */
    RCC->AHB1ENR |= (1U << 1);

    /* CSN output */
    NRF24_CSN_PORT->MODER &= ~(3U << (NRF24_CSN_PIN * 2));
    NRF24_CSN_PORT->MODER |=  (1U << (NRF24_CSN_PIN * 2));

    /* CE output */
    NRF24_CE_PORT->MODER &= ~(3U << (NRF24_CE_PIN * 2));
    NRF24_CE_PORT->MODER |=  (1U << (NRF24_CE_PIN * 2));

    /* Push-pull */
    NRF24_CSN_PORT->OTYPER &= ~(1U << NRF24_CSN_PIN);
    NRF24_CE_PORT->OTYPER  &= ~(1U << NRF24_CE_PIN);

    /* Default states */
    nrf24_csn_high();
    nrf24_ce_low();
}

void nrf24_init(void)
{
    spi_gpio_init();
    spi1_config();
    nrf24_gpio_init();

    delay_ms(5);

    /* 2-byte CRC, power down initially */
    nrf24_write_register(NRF24_REG_CONFIG,
                         NRF24_CONFIG_EN_CRC |
                         NRF24_CONFIG_CRCO);

    /* Enable auto-ack on pipes 0 and 1 */
    nrf24_write_register(NRF24_REG_EN_AA, 0x03);

    /* Enable RX pipes 0 and 1 */
    nrf24_write_register(NRF24_REG_EN_RXADDR, 0x03);

    /* Address width = 5 bytes */
    nrf24_write_register(NRF24_REG_SETUP_AW, 0x03);

    /* Retry delay and retry count */
    nrf24_write_register(NRF24_REG_SETUP_RETR, (3U << 4) | 15U);

    /* RF channel 76 */
    nrf24_write_register(NRF24_REG_RF_CH, 76U);

    /* 1 Mbps, 0 dBm */
    nrf24_write_register(NRF24_REG_RF_SETUP, (3U << 1));

    nrf24_set_payload_size(32U);

    nrf24_flush_rx();
    nrf24_flush_tx();
    nrf24_clear_interrupts();

    nrf24_power_up();
    nrf24_set_tx_mode();
}

void nrf24_set_channel(uint8_t channel)
{
    if (channel > 125U)
    {
        channel = 125U;
    }

    nrf24_write_register(NRF24_REG_RF_CH, channel);
}

void nrf24_set_payload_size(uint8_t size)
{
    if (size == 0U)
    {
        size = 1U;
    }

    if (size > NRF24_MAX_PAYLOAD_SIZE)
    {
        size = NRF24_MAX_PAYLOAD_SIZE;
    }

    nrf24_payload_size = size;

    nrf24_write_register(NRF24_REG_RX_PW_P0, size);
    nrf24_write_register(NRF24_REG_RX_PW_P1, size);
}

void nrf24_set_retries(uint8_t delay, uint8_t count)
{
    delay &= 0x0F;
    count &= 0x0F;

    nrf24_write_register(NRF24_REG_SETUP_RETR, (delay << 4) | count);
}

void nrf24_open_tx_pipe(const uint8_t *addr)
{
    nrf24_write_buffer(NRF24_REG_TX_ADDR, addr, NRF24_ADDR_WIDTH);
    nrf24_write_buffer(NRF24_REG_RX_ADDR_P0, addr, NRF24_ADDR_WIDTH);
    nrf24_write_register(NRF24_REG_RX_PW_P0, nrf24_payload_size);
}

void nrf24_open_rx_pipe(uint8_t pipe, const uint8_t *addr)
{
    uint8_t en_rxaddr;

    if (pipe > 5U)
    {
        return;
    }

    if (pipe <= 1U)
    {
        nrf24_write_buffer(NRF24_REG_RX_ADDR_P0 + pipe, addr, NRF24_ADDR_WIDTH);
    }
    else
    {
        /* pipes 2-5 only store the LSB, upper 4 bytes come from pipe 1 */
        nrf24_write_register(NRF24_REG_RX_ADDR_P0 + pipe, addr[0]);
    }

    nrf24_write_register(nrf24_pipe_payload_reg(pipe), nrf24_payload_size);

    en_rxaddr = nrf24_read_register(NRF24_REG_EN_RXADDR);
    en_rxaddr |= (1U << pipe);
    nrf24_write_register(NRF24_REG_EN_RXADDR, en_rxaddr);
}

void nrf24_start_rx(void)
{
    nrf24_ce_low();
    nrf24_flush_rx();
    nrf24_clear_interrupts();
    nrf24_set_rx_mode();
    nrf24_ce_high();

    delay_us(150);
}

void nrf24_stop_rx(void)
{
    nrf24_ce_low();
    nrf24_set_tx_mode();
}

int nrf24_send(const uint8_t *data, uint8_t len)
{
    uint32_t timeout;
    uint8_t status;

    if (len == 0U || len > NRF24_MAX_PAYLOAD_SIZE)
    {
        return NRF24_ERR;
    }

    nrf24_ce_low();
    nrf24_set_tx_mode();
    nrf24_clear_interrupts();
    nrf24_flush_tx();

    nrf24_write_payload(data, len);

    nrf24_ce_high();
    delay_us(20);
    nrf24_ce_low();

    timeout = 1000000U;

    while (timeout--)
    {
        status = nrf24_read_register(NRF24_REG_STATUS);

        if (status & NRF24_STATUS_TX_DS)
        {
            nrf24_clear_interrupts();
            return NRF24_OK;
        }

        if (status & NRF24_STATUS_MAX_RT)
        {
            nrf24_clear_interrupts();
            nrf24_flush_tx();
            return NRF24_ERR_MAX_RT;
        }
    }

    return NRF24_ERR_TIMEOUT;
}

int nrf24_available(void)
{
    uint8_t fifo_status;

    fifo_status = nrf24_read_register(NRF24_REG_FIFO_STATUS);

    if (fifo_status & NRF24_FIFO_RX_EMPTY)
    {
        return 0;
    }

    return 1;
}

int nrf24_read(uint8_t *data, uint8_t *len, uint8_t *pipe)
{
    uint8_t status;
    uint8_t rx_pipe;

    if (!nrf24_available())
    {
        return NRF24_ERR_NO_DATA;
    }

    status = nrf24_get_status();
    rx_pipe = (status >> 1) & 0x07U;

    if (rx_pipe > 5U)
    {
        return NRF24_ERR;
    }

    nrf24_read_payload(data, nrf24_payload_size);

    if (len != 0)
    {
        *len = nrf24_payload_size;
    }

    if (pipe != 0)
    {
        *pipe = rx_pipe;
    }

    nrf24_clear_interrupts();

    return NRF24_OK;
}

void nrf24_flush_rx(void)
{
    nrf24_command(NRF24_CMD_FLUSH_RX);
}

void nrf24_flush_tx(void)
{
    nrf24_command(NRF24_CMD_FLUSH_TX);
}

uint8_t nrf24_get_status(void)
{
    uint8_t status;

    spi1_set_mode0();

    nrf24_csn_low();
    status = spi1_exchange_byte(NRF24_CMD_NOP);
    nrf24_csn_high();

    return status;
}

uint8_t nrf24_read_register(uint8_t reg)
{
    uint8_t value;

    spi1_set_mode0();

    nrf24_csn_low();
    spi1_exchange_byte(NRF24_CMD_R_REGISTER | (reg & 0x1F));
    value = spi1_exchange_byte(NRF24_CMD_NOP);
    nrf24_csn_high();

    return value;
}

void nrf24_write_register(uint8_t reg, uint8_t value)
{
	spi1_set_mode0();

    nrf24_csn_low();
    spi1_exchange_byte(NRF24_CMD_W_REGISTER | (reg & 0x1F));
    spi1_exchange_byte(value);
    nrf24_csn_high();
}
