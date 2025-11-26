import asyncio
import struct
from datetime import datetime
from bleak import BleakScanner
from bleak.backends.device import BLEDevice
from bleak.backends.scanner import AdvertisementData

# bluepyのScanDelegateクラスの役割を担うクラス
class BleakScannerDelegate:
    def __init__(self):
        # 最後に受信したシーケンス番号（bytes）
        self.lastseq = None
        # 最後に受信した時刻
        self.lasttime = datetime.fromtimestamp(0)
        # 処理対象のカンパニーID (bluepyの 'ffff')
        self.target_company_id = 0xFFFF

    # BleakScannerがデバイスを検出するたびに呼び出されるコールバック関数。
    def detection_callback(self, device: BLEDevice, advertisement_data: AdvertisementData):
        # Manufacturerデータがあるか確認
        # 指定したカンパニーID (0xFFFF) が含まれているか確認
        if self.target_company_id in advertisement_data.manufacturer_data:
            # 0xFFFF に紐づくペイロード(実データ)をbytesで取得
            data = advertisement_data.manufacturer_data[self.target_company_id]
            # 期待するデータ長 (Seq 1byte + Data 8bytes = 9 bytes) か確認
            if len(data) >= 9:
                # シーケンス番号 (1 byte)
                current_seq = data[0:1] # スライスで1byteをbytesとして取得
                # センサーデータ (8 bytes)
                payload = data[1:9]
                # 経過時間（timedelta）
                delta = datetime.now() - self.lasttime
                # シーケンス番号が前回と違う かつ、前回の受信から11秒以上経過している
                if current_seq != self.lastseq and delta.total_seconds() > 11:
                    # 状態を更新
                    self.lastseq = current_seq
                    self.lasttime = datetime.now()
                    try:
                        # データをアンパック
                        (wind_speed, co2, temp, humid) = struct.unpack('<hhhh', payload)
                        # データを整形して出力
                        print(f"風速={wind_speed/100}[m/s]、CO2={co2}[ppm]、 温度={temp/100}[%]、湿度={humid/100}[%]")
                    except struct.error as e:
                        print(f"データ展開エラー: {e}")


# スキャナを起動し、ループ処理を実行します。
async def main():
    # スキャン処理クラスのインスタンスを作成
    delegate = BleakScannerDelegate()
    # スキャナを作成し、コールバック関数（クラスのメソッド）を登録
    scanner = BleakScanner(detection_callback=delegate.detection_callback)
    print("スキャンを開始します... (Ctrl+Cで停止)")
    # bluepyの scanner.scan(5.0) をループする動作を再現
    while True:
        try:
            # スキャンを開始 (バックグラウンドで実行)
            await scanner.start()
            # 5秒間スキャン状態を維持 (bluepyの scan(5.0) に相当)
            await asyncio.sleep(5.0)
            # スキャンを停止 (リソースを一旦解放)
            await scanner.stop()
        except Exception as e:
            print(f"BLEスキャンエラー: {e}")
            # エラー発生時に少し待機
            await asyncio.sleep(1.0)


if __name__ == "__main__":
    try:
        # asyncioイベントループを実行
        asyncio.run(main())
    except KeyboardInterrupt:
        print("スキャンを停止しました。")