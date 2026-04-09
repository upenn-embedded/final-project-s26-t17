# ST7735 LCD Library

SPI-based library for initializing and drawing to the ST7735 TFT display.

## Wiring (Important)

### SPI Signals

| ST7735 Pin | STM32 Pin | Function                       |
| ---------- | --------- | ------------------------------ |
| SCK        | PA5       | SPI clock                      |
| MOSI / SDA | PA7       | SPI data from STM32 to display |
| CS         | PA9       | Chip select                    |
| A0 / DC    | PA8       | Data/command control           |
| RST / RES  | PA10      | Display reset                  |

---

### Power

| ST7735 Pin | Connection                               | Notes                  |
| ---------- | ---------------------------------------- | ---------------------- |
| VCC        | 3.3V                                     | Use 3.3V logic         |
| GND        | GND                                      | Common ground required |
| LED / LITE | 3.3V or board-supported backlight supply | Needed for backlight   |

---

### Unused in This Driver

| ST7735 Pin | Notes                     |
| ---------- | ------------------------- |
| MISO       | Not used                  |
| Other pins | Depends on breakout board |

---

## Display Configuration

| Setting      | Value           |
| ------------ | --------------- |
| Width        | 160             |
| Height       | 128             |
| Color Format | RGB565 (16-bit) |
| X Start      | 0               |
| Y Start      | 0               |

---

## Usage

```c id="v5dtso"
st7735_init();

st7735_fill_screen(ST7735_BLACK);
st7735_draw_pixel(10, 10, ST7735_WHITE);
st7735_draw_rect(20, 20, 50, 30, ST7735_RED);
st7735_fill_rect(80, 40, 40, 40, ST7735_BLUE);
```

---

## Notes

* Uses the existing SPI1 driver
* Display is write-only in this implementation
* D/C selects command vs data mode
* RST is controlled manually through GPIO
* Current `MADCTL` setting is `0xA0`
* Drawing bounds are clipped to the configured screen size
