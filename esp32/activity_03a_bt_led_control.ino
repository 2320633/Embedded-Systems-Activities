#include <BluetoothSerial.h>

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error "Bluetooth is not enabled! Please enable Bluetooth in the ESP32 project configuration."
#endif

BluetoothSerial SerialBT;
const int ledPin = 25;

void setup() {
  pinMode(ledPin, OUTPUT);
  Serial.begin(115200);

  SerialBT.begin("ESP32_BT_LED");
  Serial.println("ESP32 Bluetooth LED control ready");
  Serial.println("Send: 1 = LED ON, 0 = LED OFF");
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
    }
  }

  delay(100);
}
