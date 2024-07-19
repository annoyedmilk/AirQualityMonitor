# AirQualityMonitor

## Overview

This project implements an air quality monitor using a BME680 environmental sensor and a Seeed Studio XIAO ESP32C3 microcontroller. It measures air quality, temperature, humidity, and CO2 levels, and integrates with HomeKit for easy monitoring. The project is developed using PlatformIO.

## Features

- Air Quality Index (AQI) measurement
- VOC (Volatile Organic Compounds) detection
- Temperature and humidity monitoring
- CO2 equivalent estimation
- Battery voltage monitoring
- HomeKit integration for easy access to sensor data
- Configured for 3.3V operation of BME680
- Sensor readings updated every 5 minutes

## Hardware Requirements

- Seeed Studio XIAO ESP32C3
- BME680 environmental sensor
- Battery (for portable operation)

## Software Dependencies

- PlatformIO
- [HomeSpan library](https://github.com/HomeSpan/HomeSpan)
- [BSEC2 library](https://github.com/boschsensortec/Bosch-BSEC2-Library)

## Setup

1. Clone this repository:
   ```
   git clone https://github.com/annoyedmilk/AirQualityMonitor.git
   ```

2. Open the project in PlatformIO.

3. PlatformIO should automatically detect and install the required libraries based on the `platformio.ini` file. If not, you can manually install them:
   - HomeSpan
   - BSEC2

4. Connect the BME680 sensor to your XIAO ESP32C3:
   - SDA to pin 20
   - SCL to pin 21
   - VCC to 3.3V
   - GND to GND

5. If using a battery, connect it to pin 2 for voltage monitoring.

6. Build and upload the project to your XIAO ESP32C3 using PlatformIO.

## Usage

1. After uploading, power on the device.
2. The device will appear as a HomeKit accessory.
3. Add the accessory to your HomeKit setup using the Home app on your iOS device.
4. You can now monitor air quality, temperature, humidity, and battery level through the Home app.

## Configuration

- The sensor is configured to use the Default_H2S_NonH2S configuration for the BME680.
- Sensor readings are updated every 5 minutes.
- Battery voltage is mapped to a percentage (0-100%) based on a range of 3.4V to 4.2V.
- Project-specific configurations can be found in the `platformio.ini` file.

## Development

To make changes or contribute to the project:

1. Make your code changes.
2. Use PlatformIO's build system to compile the project and check for errors.
3. Test your changes thoroughly.
4. If you're using VS Code with PlatformIO extension, you can use the PlatformIO toolbar for building, uploading, and monitoring.

## Contributing

Contributions to improve the project are welcome. Please follow these steps:

1. Fork the repository
2. Create a new branch (`git checkout -b feature/AmazingFeature`)
3. Make your changes and commit them (`git commit -m 'Add some AmazingFeature'`)
4. Push to the branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

## Acknowledgments

- [HomeSpan](https://github.com/HomeSpan/HomeSpan) for the HomeKit integration
- [Bosch Sensortec](https://www.bosch-sensortec.com/) for the BME680 sensor and BSEC library
- [PlatformIO](https://platformio.org/) for the development platform