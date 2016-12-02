#include <PinChangeInt.h>
// #include <PinChangeIntConfig.h>

#define RPM_PIN 3
#define TPS_PIN A0

volatile byte half_revolutions;
unsigned long timeold;
uint8_t rpm;
float tps;

void setup() {
  // RPM Sensor Base Values
  rpm = 0;
  half_revolutions = 0;
  timeold = 0;

  // TPS Sensor Values
  tps = 0.0;

  // Interrupt Setup For RPM sensor
  pinMode(RPM_PIN, INPUT);
  digitalWrite(RPM_PIN, HIGH);
  PCintPort::attachInterrupt(RPM_PIN, &quicfunc, CHANGE); // RISING, FALLING, CHANGE

  //analogWrite(TPS_PIN, HIGH);

  Serial.begin(115200);
  Serial.print("Start");
}

void loop() {
  delay(100);

  int tpsValue = analogRead(TPS_PIN);
  float tpsVoltage= tpsValue * (5.0 / 1023.0);

  Serial.print("TPS: ");
  Serial.println(tpsVoltage, 4);

  if (half_revolutions >= 20) { 
     //Update RPM every 20 counts, increase this for better RPM resolution,
     //decrease for faster update
     rpm = 30*1000/(millis() - timeold)*half_revolutions;
     timeold = millis();
     half_revolutions = 0;
     Serial.print("RPM: ");
     Serial.println(rpm,DEC);

     // Serial.print("TPS: ");
     // Serial.println(tpsVoltage, 4);
   }
}

void quicfunc() {
  // latest_interrupted_pin=PCintPort::arduinoPin;
  // interrupt_count[latest_interrupted_pin]++;
  half_revolutions++;
  //Serial.print(".");
}
