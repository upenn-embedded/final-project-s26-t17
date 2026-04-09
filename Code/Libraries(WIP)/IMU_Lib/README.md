# LSM6DSO Accelerometer Library

SPI-based library for basic communication with the LSM6DSO accelerometer.

## Wiring (Important)

This library uses the existing SPI1 driver and its chip select line.

### SPI Signals

| LSM6DSO Pin | STM32 Pin | Function                      |
| ----------- | --------- | ----------------------------- |
| SCK         | PA5       | SPI clock                     |
| MISO / SDO  | PA6       | SPI data from sensor to STM32 |
| MOSI / SDA  | PA7       | SPI data from STM32 to sensor |
| CS          | PA9       | Chip select                   |

---

### Power

| LSM6DSO Pin | Connection | Notes                  |
| ----------- | ---------- | ---------------------- |
| VCC         | 3.3V       | Use 3.3V logic         |
| GND         | GND        | Common ground required |

---

## Library Behavior

| Item                    | Value    |
| ----------------------- | -------- |
| Interface               | SPI      |
| WHO_AM_I register       | `0x0F`   |
| Expected WHO_AM_I value | `0x6C`   |
| Accelerometer ODR       | 104 Hz   |
| Accelerometer range     | ±2g      |
| Gyroscope               | Disabled |

---

## Usage

```c id="4n9m5z"
lsm6ds0_accel_raw_t accel;

spi_gpio_init();
spi1_config();

if (lsm6ds0_test(&accel) == 0)
{
    lsm6ds0_read_accel_raw(&accel);
}
```

### Manual Example

```c id="8v4o2w"
lsm6ds0_accel_raw_t accel;

spi_gpio_init();
spi1_config();
lsm6ds0_init();

lsm6ds_wait_for_xlda();
lsm6ds0_read_accel_raw(&accel);
```

---

## Notes

* Uses the existing SPI1 driver
* Current chip select is shared with the SPI driver `CS` pin
* Raw accelerometer values are read from registers `0x28` to `0x2D`
* Library currently reads accelerometer data only
* Register auto-increment support exists internally
* Polling is used, not interrupts
