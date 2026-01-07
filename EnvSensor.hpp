#pragma once

#include <M5Unified.h>
#include "M5UnitENV.h"

#include "Utils.hpp"

class EnvSensor {
public:
  bool Setup();
  void Update();
  Measurements GetMeasurements() const;
private:
  SCD4X m_scd4x;
  uint16_t m_co2;
  float m_temperature;
  float m_humidity;
};

bool EnvSensor::Setup() {
  if (!m_scd4x.begin(&Wire, SCD4X_I2C_ADDR, 32, 33, 400000U)) {
      Serial.println("Couldn't find SCD4X");
      return false;
  }
  m_scd4x.stopPeriodicMeasurement();
  m_scd4x.startPeriodicMeasurement();
  return true;
}

void EnvSensor::Update() {
  if (m_scd4x.update()) {
    const uint16_t co2 = m_scd4x.getCO2();
    if (std::clamp(co2, CO2_MIN, CO2_MAX) == co2) {
      m_co2 = co2;
    }
    const float temperature = m_scd4x.getTemperature();
    if (std::clamp(temperature, TEMP_MIN, TEMP_MAX) == temperature) {
      m_temperature = temperature;
    }
    const float humidity = m_scd4x.getHumidity();
    if (std::clamp(humidity, HUMIDITY_MIN, HUMIDITY_MAX) == humidity) {
      m_humidity = humidity;
    }
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
