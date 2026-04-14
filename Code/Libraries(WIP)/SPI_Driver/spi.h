/*
 * spi.h
 *
 *  Created on: Apr 5, 2026
 *      Author: david
 */

#ifndef SPI_H_
#define SPI_H_

#include <stdint.h>

#define SPI_MODE_0 0U
#define SPI_MODE_3 3U

void spi1_receive(uint8_t *data,uint32_t size);
void spi1_transmit(uint8_t *data,uint32_t size);
void spi1_config(void);
void spi1_set_mode(uint8_t mode);
void spi_gpio_init(void);
void cs_enable(void);
void cs_disable(void);

#endif /* SPI_H_ */
