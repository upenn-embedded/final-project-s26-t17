# Embedded Drivers & Libraries (WIP)
This subfolder contains custom-written drivers and libraries for the final project. Look at each Libraries sub-folder for code and information. 

## ADC Driver (STM32)

Handles analog-to-digital conversion for sensor inputs for the STM. Will be used to read the joystick values for the cars controller. 

**Status**
* Basic initialization implemented
* Channel read working (single conversion)
* No interrupt or DMA support

---

## SPI Driver (STM32)

Provides SPI communication for peripherals.

**Status**
* Basic transmit/receive working
* Used by dependent libraries (LCD, sensors)
* Tested with ST7735 LCD screen and LSM6DSO 

---

## Motor Control Library (L298N H-Bridge)
Controls two DC motors (direction + PWM speed).

**Status**
* Direction control implemented
* PWM control partially implemented
* Works for 2 motor control and can read from joystick. 
* Needs touching up when it comes to creating gradual speed up and slow down. 

---

## ST7735 LCD Library (SPI)

Handles display initialization and drawing functions.

**Status**
* Initialization working
* Basic drawing (pixel, rect, fill) implemented
* Minor bugs observed (e.g., screen fill bounds)
* Needs optimization and cleanup

---

## LSM6DSO Accelerometer Library
Interfaces with IMU sensor for motion data.

**Status**
* SPI communication established
* Register reads working for 3 registers. 

---

## NRF Communication Library

Enables wireless communication using NRF modules.

**Status**

* Not yet implemented
* Planned to use SPI driver
* Design pending
---

## IR Detection Library
Supports infrared-based detection.

**Status**

* Not yet implemented
* Design pending (hardware + interface)

---
