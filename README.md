# ESP32 Heater Control System

This project is a temperature-based state controller for a simulated heater, implemented on the ESP32 platform.

## Features

-   *State-Driven Control:* Implements Idle, Heating, Stabilizing, Target Reached, and Overheat states based on temperature thresholds.
-   *Visual & Auditory Feedback:* Uses a red LED for heater status and a piezo buzzer for a critical overheat alarm.
-   *Continuous Logging:* Logs all sensor data and system states to the Serial Monitor.
-   *Bonus - BLE Advertising:* Broadcasts the current system state for monitoring on a BLE scanner app (included in the hardware version).
-   *Bonus - Periodic Operation:* Uses a standard delay() in the main loop to ensure consistent, periodic temperature checks.

## Implementation Strategy: Simulation vs. Hardware

To meet all project requirements and address the technical limitations of the Wokwi simulation environment, a two-file implementation strategy has been adopted. This repository contains two distinct and separate Arduino sketches:

1.  **Wokwi_Simulation.ino**
    *   *Purpose:* This sketch is designed specifically to run on the *Wokwi platform*.
    *   *Functionality:* It includes all core features: the state machine, sensor reading, LED/buzzer feedback, and serial logging. The Bluetooth Low Energy (BLE) feature has been excluded from this file to ensure a smooth, non-blocking simulation experience.

2.  **Hardware_BLE_Implementation.ino**
    *   *Purpose:* This sketch is the complete and final version intended for a *physical ESP32 board*.
    *   *Functionality:* It contains all features from the simulation sketch, plus the fully implemented *BLE advertising* feature. This code is ready to be deployed on a physical ESP32-C3-MINI or similar board.

## Hardware Wiring

-   *NTC Sensor:* Signal pin to GPIO 34, VCC to 3V3, GND to GND.
-   *Red LED:* GPIO 13 to Anode (+), Cathode (-) to GND.
-   *Buzzer:* GPIO 22 to Positive (+), Negative (-) to GND.
