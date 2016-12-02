//
// Based on:
// https://blog.3d-logic.com/2015/01/10/using-a-tm1638-based-board-with-arduino/
//

// Display PINs
const int DisplayStb = 8;
const int DisplayClk = 9;
const int DisplayDio = 10;
// DistanceSensor PINs
const int DistanceTrigPin = 2;
const int DistanceEchoPin = 4;

const uint8_t numberValues[10] = {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f};
const uint8_t dashValue = 0x40;
const uint8_t emptyValue = 0x00;

bool ledStates[8] = {false, false, false, false, false, false, false, false};

enum Brightness {
  low,
  high,
};

enum LedState {
  on,
  off,
};


void setup() {
  pinMode(DisplayStb, OUTPUT);
  pinMode(DisplayClk, OUTPUT);
  pinMode(DisplayDio, OUTPUT);

  pinMode(DistanceTrigPin, OUTPUT);
  pinMode(DistanceEchoPin, INPUT);

  turnOn(low);
  resetDisplay();

  for (int i = 0; i < 15; i = i + 2) {
    setDigit(i, emptyValue);
  }

  /*
  setLedState(1, on);
  setLedState(3, on);
  setLedState(5, on);
  setLedState(7, on);
  setLedState(9, on);
  setLedState(11, on);
  setLedState(13, on);
  setLedState(15, on);
  */

  Serial.begin(115200);

}

void loop() {
  /*
  // Use These for button handling if needed in the future
  uint8_t buttons = readButtons();
  handleButtonInfo(buttons);

  delay(300);
  */

  handleDistanceSensor();
  delay(100);
}

// -----------------------------
// Display Methods
// -----------------------------

void handleButtonInfo(uint8_t buttons) {
  for(uint8_t position = 0; position < 8; position++) {
    uint8_t mask = 0x1 << position;
    if (buttons & mask) {
      ledStates[position] = !ledStates[position];
    }
  }

  for (int i = 0; i < 8; i++) {
    setLedState((i * 2) + 1, ledStates[i] ? on : off);
  }
}

uint8_t readButtons(void) {
  uint8_t buttons = 0;
  digitalWrite(DisplayStb, LOW);
  shiftOut(DisplayDio, DisplayClk, LSBFIRST, 0x42); // Command for input
 
  pinMode(DisplayDio, INPUT);

  for (uint8_t i = 0; i < 4; i++) {
    uint8_t value = shiftIn(DisplayDio, DisplayClk, LSBFIRST) << i;
    buttons = buttons | value;
  }
 
  pinMode(DisplayDio, OUTPUT);
  digitalWrite(DisplayStb, HIGH);
  return buttons;
}
uint8_t numberValue(int index) {
  return numberValue(index, false);
}

uint8_t numberValue(int index, bool isDotOn) {
  uint8_t value = 0x00;
  if (index > 9 || index < 0) {
    value = dashValue; 
  }
  else {
    value = numberValues[index];
  }

  if (isDotOn) {
    value = value | 0x80;
  }

  return value;
}

void resetDisplay() {
  sendCommand(0x40); // set auto increment mode
  digitalWrite(DisplayStb, LOW);
  shiftOut(DisplayDio, DisplayClk, LSBFIRST, 0xc0);   // set starting address to 0
  for(uint8_t i = 0; i < 16; i++) {
    shiftOut(DisplayDio, DisplayClk, LSBFIRST, 0x00);
  }
  digitalWrite(DisplayStb, HIGH);
}

void setDigit(int position, uint8_t value) {
  sendCommand(0x44);

  uint8_t cPos = (12 << 4) | position;
  
  digitalWrite(DisplayStb, LOW);
  shiftOut(DisplayDio, DisplayClk, LSBFIRST, cPos); // last digit
  shiftOut(DisplayDio, DisplayClk, LSBFIRST, value);
  digitalWrite(DisplayStb, HIGH);
}

void setLedState(int position, LedState state) {
  sendCommand(0x44);

  uint8_t cPos = (12 << 4) | position;

  setLedState(cPos, state);
}

void setLedState(uint8_t position, LedState state) {
  digitalWrite(DisplayStb, LOW);
  shiftOut(DisplayDio, DisplayClk, LSBFIRST, position);
  shiftOut(DisplayDio, DisplayClk, LSBFIRST, state == on ? 0x01 : 0x00);
  digitalWrite(DisplayStb, HIGH);
}

void turnOn(Brightness brightness) {
  sendCommand(brightness == low ? 0x88 : 0x8F);
}

void turnOff() {
  sendCommand(0x80);
}

void sendCommand(uint8_t value) {
  pinMode(DisplayDio, OUTPUT);
  digitalWrite(DisplayStb, LOW);
  shiftOut(DisplayDio, DisplayClk, LSBFIRST, value);
  digitalWrite(DisplayStb, HIGH);
}

// -----------------------------
// Distance Sensor Methods
// -----------------------------

void handleDistanceSensor() {
  long distanceInCM = distanceSensorCM();

  for (int i = 0; i < 8; ++i) {
    int value = (int)distanceInCM / (int)pow(10, i) % 10;
    setDigit(14 - (i * 2), value == 0 ? emptyValue : numberValue(value));
  }

}

long distanceSensorCM() {
  long duration, cm;
  
  digitalWrite(DistanceTrigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(DistanceTrigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(DistanceTrigPin, LOW);

  duration = pulseIn(DistanceEchoPin, HIGH);

  cm = microsecondsToCentimeters(duration);

  Serial.print(cm);
  Serial.println("cm");

  return cm;
}

long microsecondsToCentimeters(long microseconds) {
  // The speed of sound is 340 m/s or 29 microseconds per centimeter.
  // The ping travels out and back, so to find the distance of the
  // object we take half of the distance travelled.
  return microseconds / 29 / 2;
}
