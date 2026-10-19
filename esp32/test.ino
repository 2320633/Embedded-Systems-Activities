/* Alexander John Balagso
CPE4A 2320633 */

#include <Servo.h>

Servo myServo;   
int yung_Pin = A5; 
int Value_ng_pot;    
int Print_ng_Angle;       

void setup() {
  myServo.attach(9);         
  Serial.begin(9600);         
}

void loop() {
  Value_ng_pot = analogRead(yung_Pin);       
  Print_ng_Angle = map(Value_ng_pot, 0, 1023, 0, 180); 
  myServo.write(Print_ng_Angle);                
  Serial.print("Servo Angle: ");
  Serial.println(Print_ng_Angle);               
  delay(15);                           
}