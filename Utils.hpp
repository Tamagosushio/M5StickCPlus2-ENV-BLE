#pragma once

#include <limits>
#include <cstdint>
#include <cstddef>

// 使用するPINの番号
constexpr int GPIO_GREEN_WIRE = 0;
constexpr int GPIO_YELLOW_WIRE = 25;
constexpr int GPIO_BLUE_WIRE = 26;

// unsinged long型の最大値
constexpr unsigned long UNSIGNED_LONG_LIMIT = std::numeric_limits<unsigned long>::max();

// 風速を0.0とみなす周期閾値
constexpr double PERIOD_TIMEOUT_MS = 1000.0;
// 周期がタイムアウトしたときの風速
constexpr double WIND_SPEED_TIMEOUT = 0.0;
// 風速計を再起動する間隔
constexpr double PERIOD_REBOOT_ANEMOMETER_MS = 1000.0 * 60 * 10; // 10分
// アドバタイジングパケットを送信する秒数
constexpr size_t PERIOD_AD = 3;
// BLE送信を行う間隔の秒数
constexpr size_t PERIOD_SEND = 5;

// センサー計測範囲
constexpr uint16_t CO2_MIN = 400;
constexpr uint16_t CO2_MAX = 2000;
constexpr float TEMP_MIN = -10.0;
constexpr float TEMP_MAX = 60.0;
constexpr float HUMIDITY_MIN = 0.0;
constexpr float HUMIDITY_MAX = 95.0;
constexpr double WIND_SPEED_MIN = 0.0;
constexpr double WIND_SPEED_MAX = 30.0;

// BLE定数
constexpr uint8_t BLE_AD_FLAG = 0x06; // BR_EDR非対応 | 一般検出可能モード
constexpr uint8_t BLE_AD_TYPE_MANUFACTURER = 0xff; // AD Type 0xFF: Manufacturer specific data
constexpr uint8_t BLE_MANUFACTURER_ID_LOW = 0xff; // Test manufacture ID low byte
constexpr uint8_t BLE_MANUFACTURER_ID_HIGH = 0xff; // Test manufacture ID high byte
constexpr uint8_t BLE_AD_LENGTH = 0x0c;  // 長さ（12Byte）

// センサーの測定値
struct Measurements {
  double windSpeed = 0.0;
  uint16_t co2;
  float humidity;
  float temperature;
};

/// @brief オーバーフローを考慮した2値の差を求める
/// @param start 2値のうち小さいとされる値
/// @param end 2値のうち大きいとされる値
template<typename T>
T CalcDiffWithOverflow(const T& start, const T& end) {
  // オーバーフローしている場合
  if (start > end) {
    return (std::numeric_limits<T>::max() - start) + end + 1;
  }
  return end - start;
}
