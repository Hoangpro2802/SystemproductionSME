#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>


#define SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"
#define ESP32_DEVICE_NAME "ESP32_Fake_MPU6050"


BLEServer *pServer = NULL;
BLECharacteristic *pCharacteristic = NULL;
bool deviceConnected = false;

// Giả lập dữ liệu gia tốc
float generateFakeAcceleration() {
  // Tạo giá trị gia tốc ngẫu nhiên từ 0.0 đến 5.0 (g)
  return random(0, 500) / 100.0; // Giá trị từ 0.0 đến 5.0
}


class MyServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) {
    deviceConnected = true;
    Serial.println("Thiết bị đã kết nối!");
  }

  void onDisconnect(BLEServer* pServer) {
    deviceConnected = false;
    Serial.println("Thiết bị đã ngắt kết nối!");
    BLEDevice::startAdvertising();
  }
};

void setup() {
  Serial.begin(115200);
  Serial.println("Khởi động ESP32 BLE Server...");


  BLEDevice::init(ESP32_DEVICE_NAME);
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

 
  BLEService *pService = pServer->createService(SERVICE_UUID);


  pCharacteristic = pService->createCharacteristic(
    CHARACTERISTIC_UUID,
    BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY
  );

  
  pCharacteristic->addDescriptor(new BLE2902());


  pService->start();

  
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);  // Tối ưu cho kết nối nhanh
  BLEDevice::startAdvertising();
  Serial.println("Đang quảng bá BLE...");
}

void loop() {
  if (deviceConnected) {
    float accel = generateFakeAcceleration();
    String data = "VIBRATION:ERROR:" + String(accel, 2);

   
    pCharacteristic->setValue(data.c_str());
    pCharacteristic->notify();
    Serial.println("Gửi dữ liệu: " + data);

  
    if (accel > 2.5) { // Ngưỡng VIBRATION_THRESHOLD trong mã Python
      Serial.println("CẢNH BÁO: Rung bất thường!");
    }
  }

  delay(2000);
}
