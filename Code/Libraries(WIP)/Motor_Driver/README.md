# Motor Control Library (L298N)

Controls two DC motors using the L298N H-bridge with direction + PWM speed control.

## Wiring (Important)

### Motor A

| L298N Pin   | STM32 Pin         | Function            |
| ----------- | ----------------- | ------------------- |
| IN1         | PB0               | Direction control   |
| IN2         | PB1               | Direction control   |
| ENA         | PB6               | PWM (speed control) |
| OUT1 / OUT2 | Motor A terminals | Motor connection    |

---

### Motor B

| L298N Pin   | STM32 Pin         | Function            |
| ----------- | ----------------- | ------------------- |
| IN3         | PB10              | Direction control   |
| IN4         | PB12              | Direction control   |
| ENB         | PB7               | PWM (speed control) |
| OUT3 / OUT4 | Motor B terminals | Motor connection    |

---

### Power

| L298N Pin | Connection                                        |
| --------- | ------------------------------------------------- |
| 12V       | External motor power supply                       |
| GND       | Common ground with STM32                          |
| 5V        | (Optional) logic supply depending on board config |

---

## Usage

```c
motor_init();

// Motor A forward at 50%
motor_set_command(MOTOR_A, 50);

// Motor B reverse at 75%
motor_set_command(MOTOR_B, -75);

// Stop all motors
motor_stop_all();
```

---

## Command Format

| Value | Behavior     |
| ----- | ------------ |
| +100  | Full forward |
| 0     | Stop         |
| -100  | Full reverse |

---

## Notes

* PWM generated using TIM4

  * PB6 → TIM4_CH1 (Motor A)
  * PB7 → TIM4_CH2 (Motor B)
* PWM frequency ≈ 1 kHz
* Direction controlled via GPIO outputs
* Commands automatically clamp to [-100, 100]
