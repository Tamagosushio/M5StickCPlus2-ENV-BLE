# UML

## Class Diagram
```mermaid
classDiagram
  DTeam *-- Anemometer
  DTeam *-- BLE
  DTeam *-- EnvSensor
  DTeam *-- Measurements
  EnvSensor ..> Measurements
  BLE ..> Measurements

  class DTeam {
    - EnvSensor m_envSensor
    - Anemometer m_anemometer
    - BLE m_ble
    - Measurements m_measurements
    - unsigned long m_preSendTimeMiliSec
    + Setup() void
    + Loop() void
    - Update() void
    - SetupPins() void
    - SetupDisplay() void
    - DisplayMeasurements() void
  }

  class Anemometer {
    - double m_windSpeed
    - unsigned long m_preBootTimeMiliSec
    + Setup() void
    + Reboot() void
    + Update() void
    + GetWindSpeed() double
    - PowerOn() void
    - PowerOff() void
    - CalcWindSpeed(double) double
  }

  class BLE {
    - uint8_t m_seq
    - BLEServer* m_pServer
    + Setup() void
    + Send(Measurements, function) void
    - SetAdvData(BLEAdvertising*, Measurements) void
  }

  class EnvSensor {
    - SCD4X m_scd4x
    - uint16_t m_co2
    - float m_temperature
    - float m_humidity
    + Setup() void
    + Update() void
    + GetMeasurements() Measurements
  }

  class Measurements {
    + double windSpeed
    + uint16_t co2
    + float humidity
    + float temperature
  }
```

## Sequence Diagram
```mermaid
sequenceDiagram
  participant Main
  participant DTeam
  participant EnvSensor
  participant Anemometer
  participant BLE

  Note over Main, BLE: Setup Phase
  Main->>DTeam: Setup()
  activate DTeam
  DTeam->>DTeam: SetupPins()
  DTeam->>DTeam: SetupDisplay()
  DTeam->>EnvSensor: Setup()
  DTeam->>Anemometer: Setup()
  DTeam->>BLE: Setup()
  deactivate DTeam

  Note over Main, BLE: Loop Phase
  loop Every Cycle
    Main->>DTeam: Loop()
    activate DTeam
    DTeam->>DTeam: Update()
    activate DTeam
    DTeam->>EnvSensor: Update()
    DTeam->>EnvSensor: GetMeasurements()
    EnvSensor-->>DTeam: Measurements
    DTeam->>Anemometer: Update()
    DTeam->>Anemometer: GetWindSpeed()
    Anemometer-->>DTeam: double
    DTeam->>DTeam: DisplayMeasurements()
    deactivate DTeam

    opt Period Send Reached
      DTeam->>BLE: Send(Measurements, callback)
      activate BLE
      loop During Advertising (10s)
        BLE->>DTeam: callback() (Update)
        activate DTeam
        DTeam->>EnvSensor: Update()
        DTeam->>EnvSensor: GetMeasurements()
        EnvSensor-->>DTeam: Measurements
        DTeam->>Anemometer: Update()
        DTeam->>Anemometer: GetWindSpeed()
        Anemometer-->>DTeam: double
        DTeam->>DTeam: DisplayMeasurements()
        deactivate DTeam
      end
      deactivate BLE
      DTeam->>Anemometer: Reboot()
    end
    deactivate DTeam
  end
```

## Hardware Wiring Diagram
```mermaid
graph TD
  ESP32["M5StickC Plus2"]
  Anemometer["Anemometer"]

  Anemometer -- "Pulse (GPIO 26)" --> ESP32
  ESP32 -- "Power Control (GPIO 0, 25)" --> Anemometer
```

