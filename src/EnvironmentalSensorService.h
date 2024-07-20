#ifndef ENVIRONMENTALSENSOR_H
#define ENVIRONMENTALSENSOR_H

#include "HomeSpan.h"

struct EnvironmentalSensor : Service::AirQualitySensor {
  SpanCharacteristic *airQuality;
  SpanCharacteristic *voc;
  SpanCharacteristic *temperature;
  SpanCharacteristic *humidity;
  SpanCharacteristic *co2;

  EnvironmentalSensor() : Service::AirQualitySensor() {
    airQuality = new Characteristic::AirQuality(1);
    voc = new Characteristic::VOCDensity(0);
    temperature = new Characteristic::CurrentTemperature(20);
    humidity = new Characteristic::CurrentRelativeHumidity(50);
    co2 = new Characteristic::CarbonDioxideLevel(400);
  }

  void updateReadings(float iaq, float vocEquivalent, float temp, float humid, float co2Equivalent) {
    int airQualityValue = mapIAQtoAirQuality(iaq);
    airQuality->setVal(airQualityValue);
    voc->setVal(vocEquivalent);
    temperature->setVal(temp);
    humidity->setVal(humid);
    co2->setVal(co2Equivalent);

    LOG1("Environmental Sensor Update:\n");
    LOG1("  IAQ: ");
    LOG1(iaq);
    LOG1("\n  Air Quality: ");
    LOG1(airQualityValue);
    LOG1("\n  VOC: ");
    LOG1(vocEquivalent);
    LOG1(" ppb\n  Temperature: ");
    LOG1(temp);
    LOG1(" °C\n  Humidity: ");
    LOG1(humid);
    LOG1(" %\n  CO2: ");
    LOG1(co2Equivalent);
    LOG1(" ppm\n");
  }

private:
  int mapIAQtoAirQuality(float iaq) {
    if (iaq <= 50) return 1;      // Excellent
    if (iaq <= 100) return 2;     // Good
    if (iaq <= 150) return 3;     // Fair
    if (iaq <= 200) return 4;     // Inferior
    return 5;                     // Poor
  }
};

#endif // ENVIRONMENTALSENSOR_H