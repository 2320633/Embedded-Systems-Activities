#include <BluetoothSerial.h>
#include <DHT.h>

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error "Bluetooth is not enabled! Please enable Bluetooth in the ESP32 project configuration."
#endif

#define DHTPIN 4
#define DHTTYPE DHT11

BluetoothSerial SerialBT;
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  dht.begin();

  SerialBT.begin("ESP32_BT_DHT");
  Serial.println("ESP32 Bluetooth DHT monitor ready");
  Serial.println("Send: S to read sensor data");
}

void loop() {
  if (SerialBT.available()) {
    char command = SerialBT.read();

    if (command == 'S' || command == 's') {
      float humidity = dht.readHumidity();
      float temperature = dht.readTemperature();

      if (isnan(humidity) || isnan(temperature)) {
        SerialBT.println("DHT read failed");
      } else {
        SerialBT.print("Humidity: ");
        SerialBT.print(humidity);
        SerialBT.print(" % | Temperature: ");
        SerialBT.print(temperature);
        SerialBT.println(" C");
      }
    }
  }

  delay(100);
}
