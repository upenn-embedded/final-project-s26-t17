# NRF24L01 Library

SPI-based library for basic `NRF24L01` communication on the STM32 board.

## Wiring

| NRF24L01 Pin | STM32 Pin | Notes |
| ------------ | --------- | ----- |
| VCC          | 3.3V      | Never use 5V |
| GND          | GND       | Common ground required |
| SCK          | PA5       | Shared `SPI1` clock |
| MISO         | PA6       | Shared `SPI1` MISO |
| MOSI         | PA7       | Shared `SPI1` MOSI |
| CSN          | PC4       | Dedicated chip select |
| CE           | PC5       | Dedicated enable pin |
| IRQ          | Optional  | Not used by this library |

## Notes

* Uses `SPI1`
* Forces `SPI1` to `Mode 0` for NRF transfers
* Designed to coexist with the LCD driver, which uses `Mode 3`
* Default address width is 5 bytes
* Default RF channel in `nrf24_init()` is `76`
* Auto-ack is currently disabled for simpler bring-up

## Usage

```c
#include "nrf24.h"

static const uint8_t addr[5] = { 'C', 'A', 'R', '0', '1' };

nrf24_init();

if (nrf24_is_connected())
{
    nrf24_config_tx(addr, 76);
}
```

### TX Example

```c
uint8_t payload[4] = { 10, 20, 30, 40 };

if (nrf24_send(payload, 4) == 0)
{
    /* packet sent */
}
```

### RX Example

```c
uint8_t rx_buf[8];

nrf24_config_rx(addr, 76, 8);

if (nrf24_data_ready())
{
    nrf24_read_payload(rx_buf, 8);
}
```

## LED Bring-Up Test

Two simple test programs are available in:

* `Code/Experimental/NRF_LED_Test_TX.c`
* `Code/Experimental/NRF_LED_Test_RX.c`

Connect an LED plus resistor to `PC8`.

* TX test: one blink means send success, two quick blinks means send failure
* RX test: LED toggles each time a packet is received
