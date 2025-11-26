#pragma once

#include "Utils.hpp"

#include <BLEDevice.h> // Bluetooth Low Energy
#include <BLEServer.h> // Bluetooth Low Energy
#include <BLEUtils.h>  // Bluetooth Low Energy

class BLE {
public:
  void Setup();
  void Send(const Measurements& measurements, std::function<void()> onWaitAction = nullptr);
private:
  // アドバタイズデータをセット
  void SetAdvData(BLEAdvertising *pAdvertising, const Measurements& measurements);
  // 送信SEQ
  uint8_t m_seq = 0;
  // BLEサーバー
  BLEServer *m_pServer = nullptr;
};


// BLE通信のセットアップ
void BLE::Setup() {
  BLEDevice::init("blepub-01"); // デバイスを初期化
  m_pServer = BLEDevice::createServer();  // サーバーを生成
}

void BLE::Send(const Measurements& measurements, std::function<void()> onWaitAction) {
  if (!m_pServer) return;
  BLEAdvertising *pAdvertising = m_pServer->getAdvertising(); // アドバタイズオブジェクトを取得
  SetAdvData(pAdvertising, measurements); // アドバタイジングデーターをセット
  pAdvertising->start(); // アドバタイズ起動
  // T_PERIOD秒アドバタイズする
  const unsigned long startTime = millis();
  while (millis() - startTime < PERIOD_AD * 1000) {
    if (onWaitAction) {
      onWaitAction();
    }
    delay(10);
  }
  pAdvertising->stop(); // アドバタイズ停止
}


void BLE::SetAdvData(BLEAdvertising *pAdvertising, const Measurements& measurements) {
  BLEAdvertisementData oAdvertisementData = BLEAdvertisementData();
  oAdvertisementData.setFlags(0x06); // BR_EDR_NOT_SUPPORTED | General Discoverable Mode
  String strServiceData = "";
  strServiceData += static_cast<char>(0x0c); // 長さ（12Byte）
  strServiceData += static_cast<char>(0xff); // AD Type 0xFF: Manufacturer specific data
  strServiceData += static_cast<char>(0xff); // Test manufacture ID low byte
  strServiceData += static_cast<char>(0xff); // Test manufacture ID high byte
  strServiceData += static_cast<char>(m_seq++); // シーケンス番号
  // 風速
  const uint16_t windSpeedInt = static_cast<uint16_t>(measurements.windSpeed * 100.0);
  strServiceData += static_cast<char>(windSpeedInt & 0xff);
  strServiceData += static_cast<char>((windSpeedInt >> 8) & 0xff);
  // CO2濃度
  strServiceData += static_cast<char>(measurements.co2 & 0xff);
  strServiceData += static_cast<char>((measurements.co2 >> 8) & 0xff);
  // 温度
  const uint16_t temperatureInt = static_cast<uint16_t>(measurements.temperature * 100.0);
  strServiceData += static_cast<char>(temperatureInt & 0xff);
  strServiceData += static_cast<char>((temperatureInt >> 8) & 0xff);
  // 湿度
  const uint16_t humidityInt = static_cast<uint16_t>(measurements.humidity * 100.0);
  strServiceData += static_cast<char>(humidityInt & 0xff);
  strServiceData += static_cast<char>((humidityInt >> 8) & 0xff);
  // データをセット
  oAdvertisementData.addData(strServiceData);
  pAdvertising->setAdvertisementData(oAdvertisementData);
}

