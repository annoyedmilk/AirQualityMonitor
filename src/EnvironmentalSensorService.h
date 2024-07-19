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
    int airQualityValue;
    if (iaq <= 50) airQualityValue = 1;      // Excellent
    else if (iaq <= 100) airQualityValue = 2; // Good
    else if (iaq <= 150) airQualityValue = 3; // Fair
    else if (iaq <= 200) airQualityValue = 4; // Inferior
    else airQualityValue = 5;                 // Poor

    airQuality->setVal(airQualityValue);
    voc->setVal(vocEquivalent);
    temperature->setVal(temp);
    humidity->setVal(humid);
    co2->setVal(co2Equivalent);
  }
};

#endif // ENVIRONMENTALSENSOR_H