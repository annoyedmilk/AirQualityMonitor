/**
 * @file main.cpp
 * @brief Simplified BME680 Environmental Sensor and Battery Voltage Monitor for XIAO ESP32C3
 *
 * This script reads data from a BME680 environmental sensor using the BSEC2 library,
 * measures battery voltage, and outputs the results via HomeKit integration.
 * It's designed for use with a Seeed Studio XIAO ESP32C3 microcontroller.
 *
 * Features:
 * - Configured for 3.3V operation of BME680
 * - Sensor readings every second
 * - Basic error handling and logging
 *
 * @date 19.07.2024
 * @version 1.0
 */

#include "HomeSpan.h"
#include <Wire.h>
#include <bsec2.h>
#include "config/Default_H2S_NonH2S/Default_H2S_NonH2S.h"

#define SDA_PIN 20
#define SCL_PIN 21
#define BATT_PIN 2

#define UPDATE_INTERVAL 300000 // 5 minutes in milliseconds

Bsec2 envSensor;
unsigned long lastUpdateTime = 0;

void checkBsecStatus(Bsec2 bsec);
void newDataCallback(const bme68xData data, const bsecOutputs outputs, Bsec2 bsec);
float readBatteryVoltage();

#include "EnvironmentalSensorService.h"
#include "BatteryService.h"

EnvironmentalSensor *envSensorService;
BatteryService *batteryService;

void setup() {
  Serial.begin(115200);
  Wire.begin(SDA_PIN, SCL_PIN);
  pinMode(BATT_PIN, INPUT);

  homeSpan.begin(Category::Sensors, "Annoyedmilk BME680 Air Quality Monitor");

  new SpanAccessory();
    new Service::AccessoryInformation();
      new Characteristic::Identify();
      new Characteristic::Name("Annoyedmilk BME680 Sensor");
      new Characteristic::Manufacturer("Annoyedmilk");
      new Characteristic::SerialNumber("AMB-BME680-001");
      new Characteristic::Model("Annoyedmilk BME680 AQI");
      new Characteristic::FirmwareRevision("1.0");
    
    envSensorService = new EnvironmentalSensor();
    batteryService = new BatteryService();

  if (!envSensor.begin(0x76, Wire)) {
    checkBsecStatus(envSensor);
  }

  // Load the custom configuration
  if (!envSensor.setConfig(Default_H2S_NonH2S_config)) {
    checkBsecStatus(envSensor);
  }

  bsec_virtual_sensor_t sensorList[] = {
    BSEC_OUTPUT_IAQ,
    BSEC_OUTPUT_BREATH_VOC_EQUIVALENT,
    BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_TEMPERATURE,
    BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_HUMIDITY,
    BSEC_OUTPUT_CO2_EQUIVALENT
  };

  if (!envSensor.updateSubscription(sensorList, sizeof(sensorList) / sizeof(sensorList[0]), BSEC_SAMPLE_RATE_LP)) {
    checkBsecStatus(envSensor);
  }

  envSensor.attachCallback(newDataCallback);
}

void loop() {
  homeSpan.poll();
  
  unsigned long currentTime = millis();
  if (currentTime - lastUpdateTime >= UPDATE_INTERVAL) {
    if (!envSensor.run()) {
      checkBsecStatus(envSensor);
    }
    lastUpdateTime = currentTime;
  }
}

void newDataCallback(const bme68xData data, const bsecOutputs outputs, Bsec2 bsec) {
  if (!outputs.nOutputs) return;

  float iaq = 0, vocEquivalent = 0, temperature = 0, humidity = 0, co2Equivalent = 0;

  for (uint8_t i = 0; i < outputs.nOutputs; i++) {
    const bsecData output = outputs.output[i];
    switch (output.sensor_id) {
      case BSEC_OUTPUT_IAQ:
        iaq = output.signal;
        break;
      case BSEC_OUTPUT_BREATH_VOC_EQUIVALENT:
        vocEquivalent = output.signal;
        break;
      case BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_TEMPERATURE:
        temperature = output.signal;
        break;
      case BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_HUMIDITY:
        humidity = output.signal;
        break;
      case BSEC_OUTPUT_CO2_EQUIVALENT:
        co2Equivalent = output.signal;
        break;
    }
  }

  envSensorService->updateReadings(iaq, vocEquivalent, temperature, humidity, co2Equivalent);

  float batteryVoltage = readBatteryVoltage();
  batteryService->updateBattery(batteryVoltage);
}

void checkBsecStatus(Bsec2 bsec) {
  if (bsec.status < BSEC_OK) {
    Serial.print("BSEC error code : ");
    Serial.println(bsec.status);
  } else if (bsec.status > BSEC_OK) {
    Serial.print("BSEC warning code : ");
    Serial.println(bsec.status);
  }

  if (bsec.sensor.status < BME68X_OK) {
    Serial.print("BME680 error code : ");
    Serial.println(bsec.sensor.status);
  } else if (bsec.sensor.status > BME68X_OK) {
    Serial.print("BME680 warning code : ");
    Serial.println(bsec.sensor.status);
  }
}

float readBatteryVoltage() {
  uint32_t Vbatt = 0;
  for (int i = 0; i < 16; i++) {
    Vbatt += analogReadMilliVolts(BATT_PIN);
  }
  return 2 * (Vbatt / 16) / 1000.0;  // Apply attenuation ratio 1/2, convert mV to V
}