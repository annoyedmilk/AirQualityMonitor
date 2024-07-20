/**
 * @file main.cpp
 * @brief Professional BME680 Environmental Sensor for XIAO ESP32C3
 *
 * This script reads data from a BME680 environmental sensor using the BSEC2 library
 * and outputs the results via HomeKit integration. It's designed for use with a
 * Seeed Studio XIAO ESP32C3 microcontroller.
 *
 * Features:
 * - Configured for 3.3V operation of BME680
 * - Sensor readings every second
 * - Error handling and logging
 *
 * @date 20.07.2024
 * @version 1.1
 */

#include "HomeSpan.h"
#include <Wire.h>
#include <bsec2.h>
#include "config/Default_H2S_NonH2S/Default_H2S_NonH2S.h"
#include "EnvironmentalSensorService.h"

#define SDA_PIN 20
#define SCL_PIN 21

Bsec2 envSensor;
EnvironmentalSensor *envSensorService;

void checkBsecStatus(Bsec2 bsec);
void newDataCallback(const bme68xData data, const bsecOutputs outputs, Bsec2 bsec);

void setup() {
  Serial.begin(115200);
  Wire.begin(SDA_PIN, SCL_PIN);

  homeSpan.begin(Category::Sensors, "BME680 Air Quality Monitor");

  new SpanAccessory();
    new Service::AccessoryInformation();
      new Characteristic::Identify();
      new Characteristic::Name("BME680 Sensor");
      new Characteristic::Manufacturer("Annoyedmilk");
      new Characteristic::SerialNumber("ANN-BME680-001");
      new Characteristic::Model("BME680 AQI");
      new Characteristic::FirmwareRevision("1.1");
    
    envSensorService = new EnvironmentalSensor();

  bsecSensor sensorList[] = {
    BSEC_OUTPUT_IAQ,
    BSEC_OUTPUT_BREATH_VOC_EQUIVALENT,
    BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_TEMPERATURE,
    BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_HUMIDITY,
    BSEC_OUTPUT_CO2_EQUIVALENT
  };

  if (!envSensor.begin(0x76, Wire)) {
    Serial.println("Failed to initialize BME680 sensor!");
    checkBsecStatus(envSensor);
  }

  if (!envSensor.updateSubscription(sensorList, sizeof(sensorList) / sizeof(sensorList[0]), BSEC_SAMPLE_RATE_LP)) {
    Serial.println("Failed to update subscription!");
    checkBsecStatus(envSensor);
  }

  envSensor.attachCallback(newDataCallback);
}

void loop() {
  homeSpan.poll();
  
  if (!envSensor.run()) {
    Serial.println("Failed to perform measurement!");
    checkBsecStatus(envSensor);
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