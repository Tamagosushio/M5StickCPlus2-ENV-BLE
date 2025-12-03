#pragma once

#include "Utils.hpp"

#include <BLEDevice.h> // Bluetooth Low Energy
#include <BLEServer.h> // Bluetooth Low Energy
#include <BLEUtils.h>  // Bluetooth Low Energy
#include <algorithm>

class BLE {
public:
  void Setup();
  void StartAdvertising(const Measurements& measurements);
  void StopAdvertising();
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

void BLE::StartAdvertising(const Measurements& measurements) {
  if (!m_pServer) return;
  BLEAdvertising *pAdvertising = m_pServer->getAdvertising(); // アドバタイズオブジェクトを取得
  SetAdvData(pAdvertising, measurements); // アドバタイジングデーターをセット
  pAdvertising->start(); // アドバタイズ起動
}

void BLE::StopAdvertising() {
  if (!m_pServer) return;
  BLEAdvertising *pAdvertising = m_pServer->getAdvertising();
  pAdvertising->stop(); // アドバタイズ停止
}


void BLE::SetAdvData(BLEAdvertising *pAdvertising, const Measurements& measurements) {
  BLEAdvertisementData oAdvertisementData = BLEAdvertisementData();
  oAdvertisementData.setFlags(BLE_AD_FLAG);
  String strServiceData = "";
  strServiceData += static_cast<char>(BLE_AD_LENGTH);
  strServiceData += static_cast<char>(BLE_AD_TYPE_MANUFACTURER);
  strServiceData += static_cast<char>(BLE_MANUFACTURER_ID_LOW);
  strServiceData += static_cast<char>(BLE_MANUFACTURER_ID_HIGH);
  strServiceData += static_cast<char>(m_seq++); // シーケンス番号
  // 風速(int16_tの範囲内に収める)
  double windSpeed = std::clamp(measurements.windSpeed, 0.0, WIND_SPEED_MAX);
  const uint16_t windSpeedInt = static_cast<uint16_t>(windSpeed * 100.0);
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

