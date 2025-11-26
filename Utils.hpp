#pragma once

#include <limits>
#include <cstdint>

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
constexpr size_t PERIOD_AD = 10;
// BLE送信を行う間隔の秒数
constexpr size_t PERIOD_SEND = 30;

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
template<class T>
T CalcDiffWithOverflow(const T& start, const T& end) {
  if (start > end) {
    const T maxValue = std::numeric_limits<T>::max();
    return end + (maxValue - start);
  }
  return end - start;
}
