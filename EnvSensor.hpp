#pragma once

#include <M5Unified.h>
#include "M5UnitENV.h"

#include "Utils.hpp"

class EnvSensor {
public:
  void Setup();
  void Update();
  Measurements GetMeasurements() const;
private:
  SCD4X m_scd4x;
  uint16_t m_co2;
  float m_temperature;
  float m_humidity;
};

void EnvSensor::Setup() {
  if (!m_scd4x.begin(&Wire, SCD4X_I2C_ADDR, 32, 33, 400000U)) {
      Serial.println("Couldn't find SCD4X");
      while (1) delay(1);
  }
  uint16_t error;
  if (error = m_scd4x.stopPeriodicMeasurement()) {
      Serial.print("Error trying to execute stopPeriodicMeasurement(): ");
  }
  if (error = m_scd4x.startPeriodicMeasurement()) {
      Serial.print("Error trying to execute startPeriodicMeasurement(): ");
  }
}

void EnvSensor::Update() {
  if (m_scd4x.update()) {
    m_co2 = m_scd4x.getCO2();
    m_temperature = m_scd4x.getTemperature();
    m_humidity = m_scd4x.getHumidity();
  }
}

Measurements EnvSensor::GetMeasurements() const {
  Measurements response = {
    .co2 = m_co2,
    .humidity = m_humidity,
    .temperature = m_temperature,
  };
  return response;
}
