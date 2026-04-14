/*
 * nrf24.h
 *
 *  Created on: Apr 10, 2026
 */

#ifndef NRF24_H_
#define NRF24_H_

#include <stdint.h>

#define NRF24_ADDR_LEN     5U
#define NRF24_MAX_PAYLOAD 32U

void nrf24_init(void);
int nrf24_is_connected(void);

void nrf24_config_tx(const uint8_t *addr, uint8_t channel);
void nrf24_config_rx(const uint8_t *addr, uint8_t channel, uint8_t payload_size);

int nrf24_send(const uint8_t *data, uint8_t len);
uint8_t nrf24_data_ready(void);
void nrf24_read_payload(uint8_t *data, uint8_t len);

uint8_t nrf24_read_status(void);
uint8_t nrf24_read_config(void);

#endif /* NRF24_H_ */
