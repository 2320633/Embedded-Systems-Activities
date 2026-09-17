#include <BluetoothSerial.h>

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error "Bluetooth is not enabled! Please enable Bluetooth in the ESP32 project configuration."
#endif

BluetoothSerial SerialBT;

// Replace this with the MAC address printed by the slave ESP32.
const char *slaveAddress = "24:6F:28:AA:BB:CC";

bool connectToSlave() {
  Serial.print("Connecting to slave ");
  Serial.println(slaveAddress);

  if (SerialBT.connect(slaveAddress)) {
    Serial.println("Connected to slave");
    return true;
  }

  Serial.println("Slave connection failed");
  return false;
}

void setup() {
  Serial.begin(115200);
  SerialBT.begin("ESP32_BT_Master", true);

  delay(1000);
  connectToSlave();
  Serial.println("Commands: 1 = LED ON, 0 = LED OFF, S = read DHT");
}

void loop() {
  if (!SerialBT.connected()) {
    delay(1000);
    connectToSlave();
    return;
  }

  if (Serial.available()) {
    char command = Serial.read();

    if (command == '1' || command == '0' || command == 'S' || command == 's') {
      SerialBT.write(command);
    }
  }

  while (SerialBT.available()) {
    Serial.write(SerialBT.read());
  }

  delay(20);
}
