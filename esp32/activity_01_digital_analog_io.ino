const int potPin = 34;     // ADC input
const int ledPin = 25;     // PWM output

void setup() {
  pinMode(ledPin, OUTPUT);
  Serial.begin(115200);
}

void loop() {
  int potValue = analogRead(potPin);
  int brightness = map(potValue, 0, 4095, 0, 255);

  analogWrite(ledPin, brightness);

  Serial.print("Potentiometer: ");
  Serial.print(potValue);
  Serial.print(" | LED brightness: ");
  Serial.println(brightness);

  delay(100);
}
