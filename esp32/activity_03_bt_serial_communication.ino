#include <BluetoothSerial.h>
#include <DHT.h>

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error "Bluetooth is not enabled! Please enable Bluetooth in the ESP32 project configuration."
#endif

#define DHTPIN 4
#define DHTTYPE DHT11

BluetoothSerial SerialBT;
DHT dht(DHTPIN, DHTTYPE);
const int ledPin = 25;

void setup() {
  pinMode(ledPin, OUTPUT);
  Serial.begin(115200);
  dht.begin();

  SerialBT.begin("ESP32_BT_Activity");
  Serial.println("ESP32 Bluetooth started");
  Serial.println("Send: 1=ON, 0=OFF, S=Sensor");
}

void loop() {
  if (SerialBT.available()) {
    char command = SerialBT.read();

    switch (command) {
      case '1':
        digitalWrite(ledPin, HIGH);
        SerialBT.println("LED ON");
        break;

      case '0':
        digitalWrite(ledPin, LOW);
        SerialBT.println("LED OFF");
        break;

      case 'S':
      case 's': {
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
        break;
      }
    }
  }

  delay(100);
}
