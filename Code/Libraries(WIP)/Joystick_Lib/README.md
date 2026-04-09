# Joystick Library

Reads a 2-axis analog joystick through the ADC driver and converts each axis into a motor command.

## Wiring (Important)

| Joystick Pin | STM32 Pin | ADC Channel | Function            |
| ------------ | --------- | ----------- | ------------------- |
| VRx          | PA1       | Channel 1   | X-axis analog input |
| VRy          | PA4       | Channel 4   | Y-axis analog input |
| VCC          | 3.3V      | —           | Power               |
| GND          | GND       | —           | Common ground       |

---

## Output Mapping

| Input            | Output        |
| ---------------- | ------------- |
| X-axis (`raw_x`) | `motor_a_cmd` |
| Y-axis (`raw_y`) | `motor_b_cmd` |

---

## Command Behavior

| Range        | Behavior         |
| ------------ | ---------------- |
| Near center  | `0`              |
| Above center | Positive command |
| Below center | Negative command |

* ADC center is assumed to be `2048`
* Deadzone is `150`
* Output command range is `-100` to `100`

---

## Usage

```c id="yjlwmu"
joystick_data_t js;

joystick_init();
joystick_read(&js);
```

After reading:

```c id="6q3jlc"
js.raw_x
js.raw_y
js.motor_a_cmd
js.motor_b_cmd
```

---

## Notes

* Uses the ADC driver internally
* `PA1` is used for X-axis input
* `PA4` is used for Y-axis input
* Centered joystick gives command `0`
* Output is scaled for direct motor control
