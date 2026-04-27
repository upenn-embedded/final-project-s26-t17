///*
// * spi.h
// *
// *  Created on: Apr 5, 2026
// *      Author: david
// */
//
//#ifndef SPI_H_
//#define SPI_H_
//
//#include <stdint.h>
//
//void spi1_receive(uint8_t *data,uint32_t size);
//void spi1_transmit(uint8_t *data,uint32_t size);
//void spi1_config(void);
//void spi_gpio_init(void);
//void cs_enable(void);
//void cs_disable(void);
//
//#endif /* SPI_H_ */




#ifndef SPI_H_
#define SPI_H_

#include <stdint.h>

void spi_gpio_init(void);
void spi1_config(void);

void spi1_transmit(uint8_t *data, uint32_t size);
void spi1_receive(uint8_t *data, uint32_t size);


uint8_t spi1_exchange_byte(uint8_t data);

void spi1_set_mode0(void);

void spi1_set_mode3(void);

#endif /* SPI_H_ */
