# SPI Driver (STM32)

Simple SPI1 driver for basic transmit and receive.

## Wiring (Important)

| SPI Signal | STM32 Pin | Function                         |
| ---------- | --------- | -------------------------------- |
| SCK        | PA5       | SPI1 clock                       |
| MISO       | PA6       | SPI1 receive                     |
| MOSI       | PA7       | SPI1 transmit                    |
| CS         | PA9       | Manual chip select (GPIO output) |

---

## Peripheral Configuration

| Setting          | Value                           |
| ---------------- | ------------------------------- |
| SPI Peripheral   | SPI1                            |
| Mode             | Master                          |
| Data Size        | 8-bit                           |
| Bit Order        | MSB first                       |
| Clock Polarity   | CPOL = 1                        |
| Clock Phase      | CPHA = 1                        |
| Duplex           | Full duplex                     |
| Slave Management | Software (`SSM = 1`, `SSI = 1`) |

---

## Usage

```c id="6qdlaz"
spi_gpio_init();
spi1_config();

cs_enable();
spi1_transmit(tx_data, tx_size);
cs_disable();
```

### Receive Example

```c id="ldw6bh"
spi_gpio_init();
spi1_config();

cs_enable();
spi1_receive(rx_data, rx_size);
cs_disable();
```

---

## Notes

* `PA9` is used as a manual chip select pin
* CS is active low
* Driver uses polling, not interrupts or DMA
* `spi1_receive()` reads by sending dummy bytes
* SPI mode is currently configured as **Mode 3** (`CPOL = 1`, `CPHA = 1`)
