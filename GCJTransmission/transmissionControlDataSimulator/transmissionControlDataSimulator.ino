#define RPMPIN 2

const int delays[10] = {0.1, 0.2, 0.4, 5, 10, 20, 5, 3, 2, 1};
long firstMilli = 0.0;

void setup() {
  // put your setup code here, to run once:
  pinMode(RPMPIN, OUTPUT);

  firstMilli = millis();

  // Serial.begin(115200);
}

void loop() {
  // put your main code here, to run repeatedly:
  
  long elapsedMillis = millis() - firstMilli;
  long currentMinuteMilli = elapsedMillis % 60000;

  int currentSecond = (int)(currentMinuteMilli / 1000.0);

  // Serial.println(currentSecond % 10 ,DEC);

  digitalWrite(RPMPIN, HIGH);
  digitalWrite(LED_BUILTIN, HIGH);
  digitalWrite(RPMPIN, LOW);
  digitalWrite(LED_BUILTIN, LOW);

  //Serial.print("Delay: ");
  //Serial.println(delays[(currentSecond % 10)] ,DEC);

  delay(delays[(currentSecond % 10)]);
}

