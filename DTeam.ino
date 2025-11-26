#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <M5StickCPlus2.h>

#include "Utils.hpp"
#include "EnvSensor.hpp"
#include "Anemometer.hpp"
#include "BLE.hpp"

class DTeam {
public:
  void Setup();
  void Loop();
private:
  void Update();
  void SetupPins();
  void SetupDisplay();
  void DisplayMeasurements();
  EnvSensor m_envSensor;
  Anemometer m_anemometer;
  BLE m_ble;
  Measurements m_measurements;
  unsigned long m_preSendTimeMiliSec = 0;
};

void DTeam::Setup() {
  M5.begin();
  Serial.begin(9600);
  SetupPins();
  SetupDisplay();
  m_envSensor.Setup();
  m_anemometer.Setup();
  m_ble.Setup();
}

void DTeam::Loop() {
  Update();
  const size_t periodSend = CalcDiffWithOverflow(m_preSendTimeMiliSec, millis());
  if (periodSend >= (PERIOD_SEND - PERIOD_AD) * 1000) {
    m_ble.Send(m_measurements, [&]{
      Update();
    });
    m_preSendTimeMiliSec = millis();
    m_anemometer.Reboot();
  }
}

void DTeam::Update() {
  m_envSensor.Update();
  m_measurements = m_envSensor.GetMeasurements();
  m_anemometer.Update();
  m_measurements.windSpeed = m_anemometer.GetWindSpeed();
  DisplayMeasurements();
}

// ピン設定
void DTeam::SetupPins() {
  pinMode(GPIO_GREEN_WIRE, OUTPUT);
  pinMode(GPIO_YELLOW_WIRE, OUTPUT);
  pinMode(GPIO_BLUE_WIRE, INPUT);
}

// ディスプレイのセットアップ
void DTeam::SetupDisplay() {
  M5.Lcd.setRotation(1); // LCDの方向を変える
  M5.Lcd.setTextSize(2); // フォントサイズを2にする
  M5.Lcd.setTextColor(WHITE, BLACK); // 文字を白、背景を黒
}

// 風速、CO2濃度、温度、湿度の4測定値をディスプレイに表示
void DTeam::DisplayMeasurements(void) {
  M5.Lcd.setCursor(0, 0, 1);
  M5.Lcd.printf("windSpeed:\r\n");
  M5.Lcd.printf("    %2.2f[m/s]\r\n", m_measurements.windSpeed);
  M5.Lcd.printf("CO2:\r\n");
  M5.Lcd.printf("    %4d[ppm]\r\n", m_measurements.co2);
  M5.Lcd.printf("Temperature:\r\n");
  M5.Lcd.printf("    %2.2f[C]\r\n", m_measurements.temperature);
  M5.Lcd.printf("Humidity:\r\n");
  M5.Lcd.printf("    %2.2f[%%]\r\n", m_measurements.humidity);
}


DTeam g_system;
void setup() {
  g_system.Setup();
}
void loop() {
  g_system.Loop();
}