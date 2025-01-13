# USB Type-C Power Meter

This project is structured as STM32CubeIDE project. See _core_ folder for the main code.

## Description

This is a USB Type-C power meter that can measure voltage, current and power. It is based on the STM32G030F6P6 MCU, INA226 for precision ADC (16-bit resolution) and ADXL345 for haptic control.

<figure style="display: block; margin-left: auto; margin-right: auto; text-align: center;">
    <img src="image/example_use.jpg" alt="Usage" width="45%" />
    <figcaption>Example usage of the USB Type-C Power Meter, measuring the charging status of a tablet</figcaption>
</figure>

<figure style="display: block; margin-left: auto; margin-right: auto; text-align: center;">
    <img src="image/overview.jpg" alt="Usage" width="45%" />
    <figcaption>Overview of the device</figcaption>
</figure>

## Schematic

<figure style="display: block; margin-left: auto; margin-right: auto; text-align: center;">
    <img src="image/schematic.png" alt="Usage" width="95%" />
    <figcaption>schematic of the device</figcaption>
</figure>

## PCB Layout

<figure style="display: block; margin-left: auto; margin-right: auto; text-align: center;">
    <img src="image/top.png" alt="Usage" width="95%" />
    <figcaption>Top layer of the PCB</figcaption>
</figure>

<figure style="display: block; margin-left: auto; margin-right: auto; text-align: center;">
    <img src="image/bottom.png" alt="Usage" width="95%" />
    <figcaption>bottom layer of the PCB</figcaption>
</figure>
