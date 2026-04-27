//#include "spi.h"
//#include "stm32f4xx.h"
//
//void spi_gpio_init(void)
//{
//    // Enable clock access to GPIOA
//    RCC->AHB1ENR |= (1U << 0);
//
//    // Set PA5, PA6, PA7 to alternate function mode
//
//    // PA5
//    GPIOA->MODER &= ~(3U << (5 * 2));
//    GPIOA->MODER |=  (2U << (5 * 2));
//
//    //  PA6
//    GPIOA->MODER &= ~(3U << (6 * 2));
//    GPIOA->MODER |=  (2U << (6 * 2));
//
//    // PA7
//    GPIOA->MODER &= ~(3U << (7 * 2));
//    GPIOA->MODER |=  (2U << (7 * 2));
//
//    // Set PA9 as general purpose output
//    GPIOA->MODER &= ~(3U << (9 * 2));
//    GPIOA->MODER |=  (1U << (9 * 2));
//
//    // Set PA5, PA6, PA7 alternate function type to AF5 (SPI1)
//
//    // PA5
//    GPIOA->AFR[0] &= ~(0xFU << (5 * 4));
//    GPIOA->AFR[0] |=  (5U   << (5 * 4));
//
//    // PA6
//    GPIOA->AFR[0] &= ~(0xFU << (6 * 4));
//    GPIOA->AFR[0] |=  (5U   << (6 * 4));
//
//    // PA7
//    GPIOA->AFR[0] &= ~(0xFU << (7 * 4));
//    GPIOA->AFR[0] |=  (5U   << (7 * 4));
//
//    // Default CS high
//    GPIOA->ODR |= (1U << 9);
//}
//
//void spi1_config(void)
//{
//    // Enable clock access to SPI1 module
//    RCC->APB2ENR |= (1U << 12);
//
//    // Set baud rate to fPCLK/4: BR[2:0] = 001
//    SPI1->CR1 &= ~(7U << 3);
//    SPI1->CR1 |=  (1U << 3);
//
//    // Set CPOL = 1 and CPHA = 1
//    SPI1->CR1 |= (1U << 0);
//    SPI1->CR1 |= (1U << 1);
//
//    // Full duplex
//    SPI1->CR1 &= ~(1U << 10);
//
//    // MSB first
//    SPI1->CR1 &= ~(1U << 7);
//
//    // Master mode
//    SPI1->CR1 |= (1U << 2);
//
//    // 8-bit data frame
//    SPI1->CR1 &= ~(1U << 11);
//
//    // Software slave management: SSM = 1, SSI = 1
//    SPI1->CR1 |= (1U << 8);
//    SPI1->CR1 |= (1U << 9);
//
//    // Enable SPI
//    SPI1->CR1 |= (1U << 6);
//}
//
//void spi1_transmit(uint8_t *data, uint32_t size)
//{
//    uint32_t i = 0;
//    volatile uint8_t temp;
//
//    while (i < size)
//    {
//        // Wait until TXE is set
//        while (!(SPI1->SR & (1U << 1))) {}
//
//        // Write data to data register
//        *((volatile uint8_t *)&SPI1->DR) = data[i];
//        i++;
//    }
//
//    // Wait until TXE is set
//    while (!(SPI1->SR & (1U << 1))) {}
//
//    // Wait until BUSY flag resets
//    while (SPI1->SR & (1U << 7)) {}
//
//    // Clear OVR flag
//    temp = *((volatile uint8_t *)&SPI1->DR);
//    temp = SPI1->SR;
//    (void)temp;
//}
//
//void spi1_receive(uint8_t *data, uint32_t size)
//{
//    while (size)
//    {
//        // Send dummy data
//        *((volatile uint8_t *)&SPI1->DR) = 0x00;
//
//        // Wait for RXNE
//        while (!(SPI1->SR & (1U << 0))) {}
//
//        // Read data
//        *data++ = *((volatile uint8_t *)&SPI1->DR);
//        size--;
//    }
//}
//
//void cs_enable(void)
//{
//    GPIOA->ODR &= ~(1U << 9);
//}
//
//void cs_disable(void)
//{
//    GPIOA->ODR |= (1U << 9);
//}


#include "spi.h"
#include "stm32f4xx.h"

void spi_gpio_init(void)
{
    // Enable clock access to GPIOA
    RCC->AHB1ENR |= (1U << 0);

    // Set PA5, PA6, PA7 to alternate function mode

    // PA5 = SCK
    GPIOA->MODER &= ~(3U << (5 * 2));
    GPIOA->MODER |=  (2U << (5 * 2));

    // PA6 = MISO
    GPIOA->MODER &= ~(3U << (6 * 2));
    GPIOA->MODER |=  (2U << (6 * 2));

    // PA7 = MOSI
    GPIOA->MODER &= ~(3U << (7 * 2));
    GPIOA->MODER |=  (2U << (7 * 2));

    // Set PA5, PA6, PA7 alternate function type to AF5 (SPI1)

    // PA5
    GPIOA->AFR[0] &= ~(0xFU << (5 * 4));
    GPIOA->AFR[0] |=  (5U   << (5 * 4));

    // PA6
    GPIOA->AFR[0] &= ~(0xFU << (6 * 4));
    GPIOA->AFR[0] |=  (5U   << (6 * 4));

    // PA7
    GPIOA->AFR[0] &= ~(0xFU << (7 * 4));
    GPIOA->AFR[0] |=  (5U   << (7 * 4));
}

void spi1_config(void)
{
    // Enable clock access to SPI1 module
    RCC->APB2ENR |= (1U << 12);

    // Disable SPI before configuration
    SPI1->CR1 &= ~(1U << 6);

    // Set baud rate to fPCLK/4: BR[2:0] = 001
    SPI1->CR1 &= ~(7U << 3);
    SPI1->CR1 |=  (1U << 3);

    // Set CPOL = 1 and CPHA = 1
//    SPI1->CR1 |= (1U << 0);
//    SPI1->CR1 |= (1U << 1);
    SPI1->CR1 &= ~(1U << 0);   // CPHA = 0
    SPI1->CR1 &= ~(1U << 1);   // CPOL = 0

    // Full duplex
    SPI1->CR1 &= ~(1U << 10);

    // MSB first
    SPI1->CR1 &= ~(1U << 7);

    // Master mode
    SPI1->CR1 |= (1U << 2);

    // 8-bit data frame
    SPI1->CR1 &= ~(1U << 11);

    // Software slave management: SSM = 1, SSI = 1
    SPI1->CR1 |= (1U << 8);
    SPI1->CR1 |= (1U << 9);



    // Enable SPI
    SPI1->CR1 |= (1U << 6);
}

void spi1_transmit(uint8_t *data, uint32_t size)
{
    uint32_t i = 0;
    volatile uint8_t temp;

    while (i < size)
    {
        // Wait until TXE is set
        while (!(SPI1->SR & (1U << 1)))
        {
        }

        // Write data to data register
        *((volatile uint8_t *)&SPI1->DR) = data[i];
        i++;

        // Wait until RXNE so received junk does not pile up
        while (!(SPI1->SR & (1U << 0)))
        {
        }

        temp = *((volatile uint8_t *)&SPI1->DR);
        (void)temp;
    }

    // Wait until TXE is set
    while (!(SPI1->SR & (1U << 1)))
    {
    }

    // Wait until BUSY flag resets
    while (SPI1->SR & (1U << 7))
    {
    }
}

void spi1_receive(uint8_t *data, uint32_t size)
{
    while (size)
    {
        // Wait until TXE is set
        while (!(SPI1->SR & (1U << 1)))
        {
        }

        // Send dummy data to generate clock
        *((volatile uint8_t *)&SPI1->DR) = 0x00;

        // Wait for RXNE
        while (!(SPI1->SR & (1U << 0)))
        {
        }

        // Read received data
        *data++ = *((volatile uint8_t *)&SPI1->DR);
        size--;
    }

    // Wait until BUSY flag resets
    while (SPI1->SR & (1U << 7))
    {
    }
}

uint8_t spi1_exchange_byte(uint8_t data)
{
    // Wait until TXE is set
    while (!(SPI1->SR & (1U << 1)))
    {
    }

    // Write one byte
    *((volatile uint8_t *)&SPI1->DR) = data;

    // Wait until RXNE is set
    while (!(SPI1->SR & (1U << 0)))
    {
    }

    // Return received byte
    return *((volatile uint8_t *)&SPI1->DR);
}

void spi1_set_mode0(void)

{

    while (SPI1->SR & (1U << 7))   // wait until not busy

    {

    }



    SPI1->CR1 &= ~(1U << 6);       // disable SPI



    SPI1->CR1 &= ~(1U << 0);       // CPHA = 0

    SPI1->CR1 &= ~(1U << 1);       // CPOL = 0



    SPI1->CR1 |= (1U << 6);        // enable SPI

}



void spi1_set_mode3(void)

{

    while (SPI1->SR & (1U << 7))   // wait until not busy

    {

    }



    SPI1->CR1 &= ~(1U << 6);       // disable SPI



    SPI1->CR1 |= (1U << 0);        // CPHA = 1

    SPI1->CR1 |= (1U << 1);        // CPOL = 1



    SPI1->CR1 |= (1U << 6);        // enable SPI

}
