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

  SerialBT.begin("ESP32_BT_Slave");
  Serial.println("ESP32 Bluetooth slave ready");
  Serial.println("Use the master with this ESP32's Bluetooth MAC address");
  Serial.println(SerialBT.getBtAddressString());
}

void loop() {
  if (SerialBT.available()) {
    char command = SerialBT.read();

    if (command == '1') {
      digitalWrite(ledPin, HIGH);
      SerialBT.println("LED ON");
    } else if (command == '0') {
      digitalWrite(ledPin, LOW);
      SerialBT.println("LED OFF");
    } else if (command == 'S' || command == 's') {
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

  delay(20);
}
