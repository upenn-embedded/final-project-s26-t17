# ADC Driver (STM32)

Simple ADC driver for reading analog inputs using `ADC1`.

## Wiring (Important)

| Signal Source  | STM32 Pin | ADC Channel | Notes                           |
| -------------- | --------- | ----------- | ------------------------------- |
| Analog Input 1 | PA1       | Channel 1   | Configure signal between 0–3.3V |
| Analog Input 2 | PA4       | Channel 4   | Configure signal between 0–3.3V |

**Requirements**

* Signals must be within **0V to 3.3V**
* Common **GND required** between sensor and STM32
* No internal pull-up/pull-down is used (pure analog mode)

---

## Usage

```c
adc_gpio_init();
adc_init();

uint16_t val1 = adc_read_channel(1); // PA1
uint16_t val2 = adc_read_channel(4); // PA4
```

---

## Notes

* Single conversion mode only
* One channel read at a time
* Resolution depends on ADC configuration (default 12-bit)
* No interrupt or DMA support
