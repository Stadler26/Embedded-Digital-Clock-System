# Embedded Digital Clock System

<img width="4284" height="5712" alt="20261006_222428260_iOS (1)" src="https://github.com/user-attachments/assets/d69d86de-a79b-499c-a16f-d89d037c1631" /> <img width="4284" height="5712" alt="20261006_222431193_iOS (1)" src="https://github.com/user-attachments/assets/f21c1e0f-7e4e-4715-9d1b-0b006c617739" />

Register-level C firmware for a digital clock with alarm on the Texas Instruments Tiva TM4C1294NCPDT. Developed for 2303ENG Embedded Systems (Griffith University).

The system is interrupt-driven and written against peripheral registers directly, with no vendor driver libraries. Time is rendered to a serial terminal, and the clock and alarm are configured with a matrix keypad and a potentiometer.

## Features

**Core requirements**

- Terminal-rendered `HH:MM:SS` display using ANSI escape codes
- 12-hour / 24-hour display toggle
- Time and alarm setting by keypad and potentiometer
- Single alarm with RGB status LED
- 1 kHz alarm tone on hardware PWM

**Extensions**

- 1-minute snooze
- Patterned alarm tone synchronised with the LED
- Settings persisted in on-chip EEPROM across power cycles and resets
- Low-power idle via CPU sleep between interrupts

## Controls

| Key | Function |
| --- | --- |
| `1` | Toggle time-set mode |
| `3` | Toggle alarm-set mode |
| `4` / `5` | Select hour / minute field while in a set mode (value taken from the potentiometer) |
| `*` | Force-set the current value |
| `6` | Toggle seconds between 0 and 59 |
| `0` | Toggle 12/24-hour display (available at any time) |
| `#` | Disarm a primed alarm; dismiss an active alarm |

### Alarm behaviour

- Entering alarm-set mode primes the alarm; the RGB LED shows **green**.
- Pressing `#` while the alarm is primed deactivates it (LED off).
- When the alarm sounds, `#` silences it. A second `#` shuts the alarm off completely.
- Any other key while the alarm is sounding snoozes it for one minute.

## Architecture

Nothing polls. Each peripheral interrupt performs the minimum work and flags an event; the main loop then resolves what was flagged.

| Source | Behaviour |
| --- | --- |
| Keypad ISR | Flags a key press. The main loop scans and decodes the key and the FSM updates state. |
| 1 Hz timer ISR | Advances the clock, checks alarm and snooze matches, and drives the LED and buzzer pattern. |
| ADC ISR | Captures the potentiometer value. The FSM maps it onto the selected hour or minute field. |
| Main loop | Re-renders the terminal only when a dirty flag is set, then sleeps until the next event. |

## Module structure

Each peripheral is owned by exactly one source file.

| Module | Responsibility |
| --- | --- |
| `gpio_keypad` | Matrix scan, key decode, GPIO interrupts |
| `timer_clock` | 1 Hz tick, time state, alarm match |
| `adc_pot` | Potentiometer sampling via ADC0 |
| `buzzer` / `rgb_led` | Alarm tone (PWM) and status colour |
| `snooze` | Snooze scheduling, independent of the FSM |
| `eeprom_store` | Settings persistence |
| `idle` | CPU sleep between interrupts |
| `app_fsm` / `uart_display` / `main` | Mode logic, terminal rendering, orchestration |

### Design rationale

- Isolating a fault, for example in UART, does not require touching ADC or PWM code.
- Satisfies the brief's multi-file compilation requirement.
- Extensions such as snooze and idle mode were added as new files without modifying the core peripheral drivers.
- Every module was bench-tested standalone before integration.

## Hardware

- Microcontroller: TM4C1294NCPDT
- 4x3 matrix keypad *(confirm layout)*
- Potentiometer on ADC0
- RGB LED
- Buzzer driven by hardware PWM
- Serial (UART) connection to a terminal emulator

## Building and running

<!-- Replace with your toolchain and steps, e.g. Keil uVision / Code Composer Studio / arm-none-eabi-gcc + OpenOCD -->

1. Open the project in your toolchain and build all modules.
2. Flash to the board.
3. Connect a terminal emulator at your configured baud rate (8-N-1) to display the clock.

## Licence

2303ENG Embedded Systems (Griffith University)
