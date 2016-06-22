const int DistanceTrigPin = 2;
const int DistanceEchoPin = 4;
const int LedPin =  13;

void setup() {
  pinMode(LedPin, OUTPUT);
  pinMode(DistanceTrigPin, OUTPUT);
  pinMode(DistanceEchoPin, INPUT);

  Serial.begin(115200);
}

void loop() {
  long duration, cm;
  
  digitalWrite(DistanceTrigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(DistanceTrigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(DistanceTrigPin, LOW);

  duration = pulseIn(DistanceEchoPin, HIGH);

  cm = microsecondsToCentimeters(duration);

  digitalWrite(LedPin, cm < 5);

  Serial.print(cm);
  Serial.println("cm");

  delay(100);
}

long microsecondsToCentimeters(long microseconds) {
  // The speed of sound is 340 m/s or 29 microseconds per centimeter.
  // The ping travels out and back, so to find the distance of the
  // object we take half of the distance travelled.
  return microseconds / 29 / 2;
}
