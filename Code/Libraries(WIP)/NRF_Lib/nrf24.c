#include "nrf24.h"
#include "spi.h"
#include "stm32f4xx.h"

#define NRF24_CSN_PORT         GPIOC
#define NRF24_CSN_PIN          4U
#define NRF24_CE_PORT          GPIOC
#define NRF24_CE_PIN           5U

#define NRF24_CMD_R_REGISTER   0x00
#define NRF24_CMD_W_REGISTER   0x20
#define NRF24_CMD_R_RX_PAYLOAD 0x61
#define NRF24_CMD_W_TX_PAYLOAD 0xA0
#define NRF24_CMD_FLUSH_TX     0xE1
#define NRF24_CMD_FLUSH_RX     0xE2
#define NRF24_CMD_NOP          0xFF

#define NRF24_REG_CONFIG       0x00
#define NRF24_REG_EN_AA        0x01
#define NRF24_REG_EN_RXADDR    0x02
#define NRF24_REG_SETUP_AW     0x03
#define NRF24_REG_SETUP_RETR   0x04
#define NRF24_REG_RF_CH        0x05
#define NRF24_REG_RF_SETUP     0x06
#define NRF24_REG_STATUS       0x07
#define NRF24_REG_RX_ADDR_P0   0x0A
#define NRF24_REG_TX_ADDR      0x10
#define NRF24_REG_RX_PW_P0     0x11
#define NRF24_REG_FIFO_STATUS  0x17
#define NRF24_REG_DYNPD        0x1C
#define NRF24_REG_FEATURE      0x1D

#define NRF24_CONFIG_PWR_UP    (1U << 1)
#define NRF24_CONFIG_PRIM_RX   (1U << 0)
#define NRF24_STATUS_RX_DR     (1U << 6)
#define NRF24_STATUS_TX_DS     (1U << 5)
#define NRF24_STATUS_MAX_RT    (1U << 4)
#define NRF24_FIFO_RX_EMPTY    (1U << 0)

static void delay_cycles(volatile uint32_t count)
{
    while (count--)
    {
    }
}

static void nrf24_gpio_init(void)
{
    RCC->AHB1ENR |= (1U << 2);

    NRF24_CSN_PORT->MODER &= ~(3U << (NRF24_CSN_PIN * 2));
    NRF24_CSN_PORT->MODER |=  (1U << (NRF24_CSN_PIN * 2));

    NRF24_CE_PORT->MODER &= ~(3U << (NRF24_CE_PIN * 2));
    NRF24_CE_PORT->MODER |=  (1U << (NRF24_CE_PIN * 2));

    NRF24_CSN_PORT->OTYPER &= ~(1U << NRF24_CSN_PIN);
    NRF24_CE_PORT->OTYPER &= ~(1U << NRF24_CE_PIN);

    NRF24_CSN_PORT->ODR |= (1U << NRF24_CSN_PIN);
    NRF24_CE_PORT->ODR &= ~(1U << NRF24_CE_PIN);
}

static void nrf24_csn_low(void)
{
    spi1_set_mode(SPI_MODE_0);
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

static uint8_t nrf24_read_reg(uint8_t reg)
{
    uint8_t cmd = NRF24_CMD_R_REGISTER | (reg & 0x1F);
    uint8_t value = 0;

    nrf24_csn_low();
    spi1_transmit(&cmd, 1);
    spi1_receive(&value, 1);
    nrf24_csn_high();

    return value;
}

static void nrf24_read_reg_multi(uint8_t reg, uint8_t *data, uint8_t len)
{
    uint8_t cmd = NRF24_CMD_R_REGISTER | (reg & 0x1F);

    nrf24_csn_low();
    spi1_transmit(&cmd, 1);
    spi1_receive(data, len);
    nrf24_csn_high();
}

static void nrf24_write_reg(uint8_t reg, uint8_t value)
{
    uint8_t tx[2];

    tx[0] = NRF24_CMD_W_REGISTER | (reg & 0x1F);
    tx[1] = value;

    nrf24_csn_low();
    spi1_transmit(tx, 2);
    nrf24_csn_high();
}

static void nrf24_write_reg_multi(uint8_t reg, const uint8_t *data, uint8_t len)
{
    uint8_t cmd = NRF24_CMD_W_REGISTER | (reg & 0x1F);
    uint8_t i;
    uint8_t buffer[NRF24_MAX_PAYLOAD];

    if (len > NRF24_MAX_PAYLOAD)
    {
        len = NRF24_MAX_PAYLOAD;
    }

    for (i = 0; i < len; i++)
    {
        buffer[i] = data[i];
    }

    nrf24_csn_low();
    spi1_transmit(&cmd, 1);
    spi1_transmit(buffer, len);
    nrf24_csn_high();
}

static void nrf24_command(uint8_t cmd)
{
    nrf24_csn_low();
    spi1_transmit(&cmd, 1);
    nrf24_csn_high();
}

static void nrf24_clear_irqs(void)
{
    nrf24_write_reg(NRF24_REG_STATUS,
                    NRF24_STATUS_RX_DR | NRF24_STATUS_TX_DS | NRF24_STATUS_MAX_RT);
}

static void nrf24_power_up_common(uint8_t config_bits)
{
    nrf24_write_reg(NRF24_REG_CONFIG, NRF24_CONFIG_PWR_UP | config_bits);
    delay_cycles(50000);
}

void nrf24_init(void)
{
    spi_gpio_init();
    spi1_config();
    nrf24_gpio_init();

    nrf24_ce_low();
    nrf24_write_reg(NRF24_REG_EN_AA, 0x00);
    nrf24_write_reg(NRF24_REG_EN_RXADDR, 0x01);
    nrf24_write_reg(NRF24_REG_SETUP_AW, 0x03);
    nrf24_write_reg(NRF24_REG_SETUP_RETR, 0x00);
    nrf24_write_reg(NRF24_REG_RF_CH, 76);
    nrf24_write_reg(NRF24_REG_RF_SETUP, 0x06);
    nrf24_write_reg(NRF24_REG_DYNPD, 0x00);
    nrf24_write_reg(NRF24_REG_FEATURE, 0x00);
    nrf24_write_reg(NRF24_REG_RX_PW_P0, NRF24_MAX_PAYLOAD);
    nrf24_clear_irqs();
    nrf24_command(NRF24_CMD_FLUSH_TX);
    nrf24_command(NRF24_CMD_FLUSH_RX);
    nrf24_power_up_common(0x00);
}

int nrf24_is_connected(void)
{
    return (nrf24_read_reg(NRF24_REG_SETUP_AW) == 0x03) ? 1 : 0;
}

void nrf24_config_tx(const uint8_t *addr, uint8_t channel)
{
    nrf24_ce_low();
    nrf24_write_reg(NRF24_REG_RF_CH, channel & 0x7F);
    nrf24_write_reg_multi(NRF24_REG_TX_ADDR, addr, NRF24_ADDR_LEN);
    nrf24_write_reg_multi(NRF24_REG_RX_ADDR_P0, addr, NRF24_ADDR_LEN);
    nrf24_write_reg(NRF24_REG_RX_PW_P0, NRF24_MAX_PAYLOAD);
    nrf24_clear_irqs();
    nrf24_command(NRF24_CMD_FLUSH_TX);
    nrf24_power_up_common(0x00);
}

void nrf24_config_rx(const uint8_t *addr, uint8_t channel, uint8_t payload_size)
{
    if (payload_size == 0U || payload_size > NRF24_MAX_PAYLOAD)
    {
        payload_size = NRF24_MAX_PAYLOAD;
    }

    nrf24_ce_low();
    nrf24_write_reg(NRF24_REG_RF_CH, channel & 0x7F);
    nrf24_write_reg_multi(NRF24_REG_RX_ADDR_P0, addr, NRF24_ADDR_LEN);
    nrf24_write_reg(NRF24_REG_RX_PW_P0, payload_size);
    nrf24_clear_irqs();
    nrf24_command(NRF24_CMD_FLUSH_RX);
    nrf24_power_up_common(NRF24_CONFIG_PRIM_RX);
    nrf24_ce_high();
    delay_cycles(3000);
}

int nrf24_send(const uint8_t *data, uint8_t len)
{
    uint8_t cmd = NRF24_CMD_W_TX_PAYLOAD;
    uint8_t i;
    uint8_t buffer[NRF24_MAX_PAYLOAD];
    uint8_t status;
    uint32_t timeout = 100000U;

    if (len == 0U || len > NRF24_MAX_PAYLOAD)
    {
        return -1;
    }

    nrf24_ce_low();
    nrf24_clear_irqs();
    nrf24_command(NRF24_CMD_FLUSH_TX);

    for (i = 0; i < len; i++)
    {
        buffer[i] = data[i];
    }

    nrf24_csn_low();
    spi1_transmit(&cmd, 1);
    spi1_transmit(buffer, len);
    nrf24_csn_high();

    nrf24_ce_high();
    delay_cycles(100);
    nrf24_ce_low();

    while (timeout--)
    {
        status = nrf24_read_reg(NRF24_REG_STATUS);

        if (status & NRF24_STATUS_TX_DS)
        {
            nrf24_clear_irqs();
            return 0;
        }

        if (status & NRF24_STATUS_MAX_RT)
        {
            nrf24_clear_irqs();
            nrf24_command(NRF24_CMD_FLUSH_TX);
            return -1;
        }
    }

    return -1;
}

uint8_t nrf24_data_ready(void)
{
    uint8_t status = nrf24_read_reg(NRF24_REG_STATUS);
    uint8_t fifo_status = nrf24_read_reg(NRF24_REG_FIFO_STATUS);

    return ((status & NRF24_STATUS_RX_DR) != 0U) &&
           ((fifo_status & NRF24_FIFO_RX_EMPTY) == 0U);
}

void nrf24_read_payload(uint8_t *data, uint8_t len)
{
    uint8_t cmd = NRF24_CMD_R_RX_PAYLOAD;

    if (len > NRF24_MAX_PAYLOAD)
    {
        len = NRF24_MAX_PAYLOAD;
    }

    nrf24_csn_low();
    spi1_transmit(&cmd, 1);
    spi1_receive(data, len);
    nrf24_csn_high();

    nrf24_clear_irqs();
}

uint8_t nrf24_read_status(void)
{
    return nrf24_read_reg(NRF24_REG_STATUS);
}

uint8_t nrf24_read_config(void)
{
    return nrf24_read_reg(NRF24_REG_CONFIG);
}
