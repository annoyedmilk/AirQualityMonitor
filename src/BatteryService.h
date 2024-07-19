#ifndef BATTERYSERVICE_H
#define BATTERYSERVICE_H

#include "HomeSpan.h"

struct BatteryService : Service::BatteryService {
  SpanCharacteristic *batteryLevel;
  SpanCharacteristic *chargingState;
  SpanCharacteristic *statusLowBattery;

  BatteryService() : Service::BatteryService() {
    batteryLevel = new Characteristic::BatteryLevel(100);
    chargingState = new Characteristic::ChargingState(2);  // 2 = Not Charging
    statusLowBattery = new Characteristic::StatusLowBattery(0);
  }

  void updateBattery(float voltage) {
    int percentage = mapBatteryPercentage(voltage);
    batteryLevel->setVal(percentage);
    statusLowBattery->setVal(percentage <= 20 ? 1 : 0);
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
};

#endif // BATTERYSERVICE_H