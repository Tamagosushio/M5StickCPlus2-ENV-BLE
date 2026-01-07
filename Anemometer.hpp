#pragma once
#include "esp32-hal-gpio.h"
#include "FunctionalInterrupt.h"
#include "esp32-hal.h"

#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <M5StickCPlus2.h>

#include "Utils.hpp"


static volatile unsigned long g_lastRiseTimeMicroSec = 0;
static volatile double g_latestPeriodMiliSec = 0.0;

// 立ち上がりの周期を保存
void IRAM_ATTR ISR_AnemometerPulse() {
  const unsigned long currentMicroSec = micros();
  const unsigned long diff = CalcDiffWithOverflow(static_cast<unsigned long>(g_lastRiseTimeMicroSec), currentMicroSec);
  g_latestPeriodMiliSec = static_cast<double>(diff) / 1000.0;
  g_lastRiseTimeMicroSec = currentMicroSec;
}


class Anemometer {
public:
  // セットアップ
  void Setup();
  // 一定間隔で再起動、間隔が経っていなければ何もしない
  void Reboot();
  // 風速の測定
  void Update();
  double GetWindSpeed() const { return m_windSpeed; };
private:
  // 風速計の電源をONにする
  void PowerOn() const;
  // 風速計の電源をOFFにする
  void PowerOff() const;
  // 周期から風速値を計算
  double CalcWindSpeed(const double periodMiliSec) const;
  // 風速値[m/s]
  double m_windSpeed = 0.0;
  // 前回起動した時間[ms]
  unsigned long m_preBootTimeMiliSec = 0;
};

void Anemometer::Setup() {
  // 確実に風速計の電源を付けるため、ON->OFF->ONの手順を踏む
  PowerOn();
  PowerOff();
  PowerOn();
  m_windSpeed = 0.0;
  g_latestPeriodMiliSec = 0.0;
  g_lastRiseTimeMicroSec = micros();
  m_preBootTimeMiliSec = millis();
  // GPIO_BLUE_WIREの立ち上がり時にISR_AnemometerPulseを実行
  attachInterrupt(digitalPinToInterrupt(GPIO_BLUE_WIRE), ISR_AnemometerPulse, RISING);
}

void Anemometer::Reboot() {
  // millisは約50日でオーバーフローするため対応
  const size_t periodBootMiliSec = CalcDiffWithOverflow(m_preBootTimeMiliSec, millis());
  // 定数周期で再起動する
  if (periodBootMiliSec >= PERIOD_REBOOT_ANEMOMETER_MS) {
    detachInterrupt(digitalPinToInterrupt(GPIO_BLUE_WIRE));
    Setup();
  }
}

void Anemometer::Update(void) {
  // 風速計から方形波を取得
  // micros()は約70分でオーバーフローするため対応
  const unsigned long currentMicroSec = micros();
  const size_t timeSinceLastPulseMicroSec = CalcDiffWithOverflow(static_cast<unsigned long>(g_lastRiseTimeMicroSec), currentMicroSec);
  const double timeSinceLastPulseMiliSec = static_cast<double>(timeSinceLastPulseMicroSec) / 1000.0;
  if (timeSinceLastPulseMiliSec > PERIOD_TIMEOUT_MS) {
    m_windSpeed = WIND_SPEED_TIMEOUT;
    g_latestPeriodMiliSec = 0.0;
  } else {
    noInterrupts();
    double period = g_latestPeriodMiliSec;
    interrupts();
    const double newWindSpeed = CalcWindSpeed(period);
    if (std::clamp(newWindSpeed, WIND_SPEED_MIN, WIND_SPEED_MAX) == newWindSpeed) {
      m_windSpeed = newWindSpeed;
    }
  }
}

void Anemometer::PowerOn() const {
  digitalWrite(GPIO_YELLOW_WIRE, HIGH);
  digitalWrite(GPIO_GREEN_WIRE, LOW);
  delay(1000); // 1000ms待機
  digitalWrite(GPIO_GREEN_WIRE, HIGH);
  delay(1000); // 1000ms待機
}

void Anemometer::PowerOff() const {
  digitalWrite(GPIO_YELLOW_WIRE, LOW);
  digitalWrite(GPIO_GREEN_WIRE, LOW);
  delay(2000); // 2000ms待機
  digitalWrite(GPIO_YELLOW_WIRE, HIGH);
  digitalWrite(GPIO_GREEN_WIRE, HIGH);
  delay(1000); // 1000ms待機
}

double Anemometer::CalcWindSpeed(const double periodMiliSec) const {
  if (periodMiliSec <= 0.001) return 0.0;
  return 0.1 * 500.0 / periodMiliSec + 0.5;
}
