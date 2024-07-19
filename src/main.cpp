/**
 * @file bmain.cpp
 * @brief BME680 Environmental Sensor and Battery Voltage Monitor for XIAO ESP32C3
 *
 * This script reads data from a BME680 environmental sensor using the BSEC2 library,
 * measures battery voltage, and outputs the results via serial communication.
 * It's designed for use with a Seeed Studio XIAO ESP32C3 microcontroller.
 *
 * @author annoyedmilk
 * @date 17.07.2024
 * @version 1.0
 *
 * @copyright Copyright (c) 2024
 *
 * @note This code is configured for a XIAO ESP32C3 with a BME680 sensor and a voltage divider for battery measurement.
 *       Ensure proper hardware setup before use.
 */

#include <Arduino.h>
#include <Wire.h>
#include <bsec2.h>

#define SDA_PIN 20
#define SCL_PIN 21
#define BATT_PIN 2

void checkBsecStatus(Bsec2 bsec);
void newDataCallback(const bme68xData data, const bsecOutputs outputs, Bsec2 bsec);
float readBatteryVoltage();
int mapBatteryPercentage(float voltage);

Bsec2 envSensor;

void setup() {
    Serial.begin(115200);
    Wire.begin(SDA_PIN, SCL_PIN);
    pinMode(BATT_PIN, INPUT);

    while (!Serial) delay(10);

    Serial.println("BME680 and Battery Monitor for XIAO ESP32C3");
    Serial.println("============================================");

    bsecSensor sensorList[] = {
        BSEC_OUTPUT_IAQ,
        BSEC_OUTPUT_STATIC_IAQ,
        BSEC_OUTPUT_CO2_EQUIVALENT,
        BSEC_OUTPUT_BREATH_VOC_EQUIVALENT,
        BSEC_OUTPUT_RAW_TEMPERATURE,
        BSEC_OUTPUT_RAW_PRESSURE,
        BSEC_OUTPUT_RAW_HUMIDITY,
        BSEC_OUTPUT_RAW_GAS,
        BSEC_OUTPUT_STABILIZATION_STATUS,
        BSEC_OUTPUT_RUN_IN_STATUS,
        BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_TEMPERATURE,
        BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_HUMIDITY,
        BSEC_OUTPUT_GAS_PERCENTAGE
    };

    if (!envSensor.begin(0x76, Wire)) {
        Serial.println("Failed to initialize BME680, check wiring!");
        checkBsecStatus(envSensor);
    }

    if (!envSensor.updateSubscription(sensorList, sizeof(sensorList) / sizeof(sensorList[0]), BSEC_SAMPLE_RATE_LP)) {
        Serial.println("Failed to update subscription!");
        checkBsecStatus(envSensor);
    }

    envSensor.attachCallback(newDataCallback);

    Serial.println("BSEC library version " + String(envSensor.version.major) + "." + String(envSensor.version.minor) + "." + String(envSensor.version.major_bugfix) + "." + String(envSensor.version.minor_bugfix));
    Serial.println("Setup completed, monitoring started.");
    Serial.println("============================================");
}

void loop() {
    if (!envSensor.run()) {
        checkBsecStatus(envSensor);
    }

    delay(1000);  // Small delay to prevent tight looping
}

void newDataCallback(const bme68xData data, const bsecOutputs outputs, Bsec2 bsec) {
    if (!outputs.nOutputs) {
        return;
    }

    float batteryVoltage = readBatteryVoltage();
    int batteryPercentage = mapBatteryPercentage(batteryVoltage);

    Serial.println("BME680 Sensor Data:");
    Serial.println("Timestamp: " + String((int)(outputs.output[0].time_stamp / INT64_C(1000000))) + " ms");

    for (uint8_t i = 0; i < outputs.nOutputs; i++) {
        const bsecData output = outputs.output[i];
        switch (output.sensor_id) {
        case BSEC_OUTPUT_IAQ:
            Serial.println("IAQ: " + String(output.signal) + " (Accuracy: " + String((int)output.accuracy) + ")");
            break;
        case BSEC_OUTPUT_STATIC_IAQ:
            Serial.println("Static IAQ: " + String(output.signal) + " (Accuracy: " + String((int)output.accuracy) + ")");
            break;
        case BSEC_OUTPUT_CO2_EQUIVALENT:
            Serial.println("CO2 Equivalent: " + String(output.signal) + " ppm (Accuracy: " + String((int)output.accuracy) + ")");
            break;
        case BSEC_OUTPUT_BREATH_VOC_EQUIVALENT:
            Serial.println("Breath VOC Equivalent: " + String(output.signal) + " ppm");
            break;
        case BSEC_OUTPUT_RAW_TEMPERATURE:
            Serial.println("Raw Temperature: " + String(output.signal) + " °C");
            break;
        case BSEC_OUTPUT_RAW_PRESSURE:
            Serial.println("Pressure: " + String(output.signal / 100.0) + " hPa");
            break;
        case BSEC_OUTPUT_RAW_HUMIDITY:
            Serial.println("Raw Humidity: " + String(output.signal) + " %");
            break;
        case BSEC_OUTPUT_RAW_GAS:
            Serial.println("Gas Resistance: " + String(output.signal / 1000.0) + " kOhms");
            break;
        case BSEC_OUTPUT_STABILIZATION_STATUS:
            Serial.println("Stabilization Status: " + String(output.signal));
            break;
        case BSEC_OUTPUT_RUN_IN_STATUS:
            Serial.println("Run-in Status: " + String(output.signal));
            break;
        case BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_TEMPERATURE:
            Serial.println("Compensated Temperature: " + String(output.signal) + " °C");
            break;
        case BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_HUMIDITY:
            Serial.println("Compensated Humidity: " + String(output.signal) + " %");
            break;
        case BSEC_OUTPUT_GAS_PERCENTAGE:
            Serial.println("Gas Percentage: " + String(output.signal) + " %");
            break;
        default:
            Serial.println("Unknown Sensor ID: " + String(output.sensor_id));
            break;
        }
    }

    Serial.println("Battery Voltage: " + String(batteryVoltage, 3) + " V");
    Serial.println("Battery Charge: " + String(batteryPercentage) + " %");
    Serial.println("--------------------------------------------");
}

void checkBsecStatus(Bsec2 bsec) {
    if (bsec.status < BSEC_OK) {
        Serial.println("BSEC error code : " + String(bsec.status));
    } else if (bsec.status > BSEC_OK) {
        Serial.println("BSEC warning code : " + String(bsec.status));
    }

    if (bsec.sensor.status < BME68X_OK) {
        Serial.println("BME68X error code : " + String(bsec.sensor.status));
    } else if (bsec.sensor.status > BME68X_OK) {
        Serial.println("BME68X warning code : " + String(bsec.sensor.status));
    }
}

float readBatteryVoltage() {
    uint32_t Vbatt = 0;
    for (int i = 0; i < 16; i++) {
        Vbatt += analogReadMilliVolts(BATT_PIN);
    }
    return 2 * (Vbatt / 16) / 1000.0;  // Apply attenuation ratio 1/2, convert mV to V
}

int mapBatteryPercentage(float voltage) {
    if (voltage >= 4.2) {
        return 100;
    } else if (voltage <= 3.4) {
        return 0;
    } else {
        return (int)((voltage - 3.4) * 100.0 / (4.2 - 3.4));
    }
}