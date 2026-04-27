/*
 * nrf24.h
 *
 *  Created on: Apr 14, 2026
 *      Author: david
 */

#ifndef NRF24_H_
#define NRF24_H_

#include <stdint.h>

#define NRF24_MAX_PAYLOAD_SIZE   32U
#define NRF24_ADDR_WIDTH         5U

#define NRF24_OK                 0
#define NRF24_ERR               -1
#define NRF24_ERR_TIMEOUT       -2
#define NRF24_ERR_MAX_RT        -3
#define NRF24_ERR_NO_DATA       -4

typedef struct
{
    uint16_t adc0;
    uint16_t adc1;
} nrf24_adc_packet_t;

void nrf24_gpio_init(void);
void nrf24_init(void);

void nrf24_set_channel(uint8_t channel);
void nrf24_set_payload_size(uint8_t size);
void nrf24_set_retries(uint8_t delay, uint8_t count);

void nrf24_open_tx_pipe(const uint8_t *addr);
void nrf24_open_rx_pipe(uint8_t pipe, const uint8_t *addr);

void nrf24_start_rx(void);
void nrf24_stop_rx(void);

int  nrf24_send(const uint8_t *data, uint8_t len);
int  nrf24_available(void);
int  nrf24_read(uint8_t *data, uint8_t *len, uint8_t *pipe);

void nrf24_flush_rx(void);
void nrf24_flush_tx(void);

uint8_t nrf24_get_status(void);
uint8_t nrf24_read_register(uint8_t reg);
void nrf24_write_register(uint8_t reg, uint8_t value);

#endif /* NRF24_H_ */