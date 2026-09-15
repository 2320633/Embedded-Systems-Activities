#include <DHT.h>

#define DHTPIN 4
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);
const int ledPin = 25;

void setup() {
  pinMode(ledPin, OUTPUT);
  Serial.begin(115200);
  dht.begin();
  Serial.println("ESP32 Serial Control Ready");
  Serial.println("Commands: 1=ON, 0=OFF, S=Status");
}

void loop() {
  if (Serial.available()) {
    char command = Serial.read();

    switch (command) {
      case '1':
        digitalWrite(ledPin, HIGH);
        Serial.println("LED ON");
        break;

      case '0':
        digitalWrite(ledPin, LOW);
        Serial.println("LED OFF");
        break;

      case 'S':
      case 's': {
        float humidity = dht.readHumidity();
        float temperature = dht.readTemperature();

        if (isnan(humidity) || isnan(temperature)) {
          Serial.println("Failed to read from DHT sensor");
        } else {
          Serial.print("Humidity: ");
          Serial.print(humidity);
          Serial.print(" % | Temperature: ");
          Serial.print(temperature);
          Serial.println(" C");
        }
        break;
      }
    }
  }

  delay(100);
}
