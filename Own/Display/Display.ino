//
// Based on:
// https://blog.3d-logic.com/2015/01/10/using-a-tm1638-based-board-with-arduino/
//

const int stb = 8;
const int clk = 9;
const int dio = 10;

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
  pinMode(stb, OUTPUT);
  pinMode(clk, OUTPUT);
  pinMode(dio, OUTPUT);

  Serial.begin(115200);
  
  turnOn(low);
  resetDisplay();

  setDigit(0, numberValue(0));
  setDigit(2, numberValue(1, true));
  setDigit(4, numberValue(2));
  setDigit(6, numberValue(3, false));
  setDigit(8, numberValue(4, true));
  setDigit(10, numberValue(5, false));
  setDigit(12, numberValue(6));
  setDigit(14, emptyValue);
}

void loop() {
  uint8_t buttons = readButtons();
  handleButtonInfo(buttons);

  delay(300);
}

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
  digitalWrite(stb, LOW);
  shiftOut(dio, clk, LSBFIRST, 0x42); // Command for input
 
  pinMode(dio, INPUT);

  for (uint8_t i = 0; i < 4; i++) {
    uint8_t value = shiftIn(dio, clk, LSBFIRST) << i;
    buttons = buttons | value;
  }
 
  pinMode(dio, OUTPUT);
  digitalWrite(stb, HIGH);
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
  digitalWrite(stb, LOW);
  shiftOut(dio, clk, LSBFIRST, 0xc0);   // set starting address to 0
  for(uint8_t i = 0; i < 16; i++) {
    shiftOut(dio, clk, LSBFIRST, 0x00);
  }
  digitalWrite(stb, HIGH);
}

void setDigit(int position, uint8_t value) {
  sendCommand(0x44);

  uint8_t cPos = (12 << 4) | position;
  
  digitalWrite(stb, LOW);
  shiftOut(dio, clk, LSBFIRST, cPos); // last digit
  shiftOut(dio, clk, LSBFIRST, value);
  digitalWrite(stb, HIGH);
}

void setLedState(int position, LedState state) {
  sendCommand(0x44);

  uint8_t cPos = (12 << 4) | position;

  setLedState(cPos, state);
}

void setLedState(uint8_t position, LedState state) {
  digitalWrite(stb, LOW);
  shiftOut(dio, clk, LSBFIRST, position);
  shiftOut(dio, clk, LSBFIRST, state == on ? 0x01 : 0x00);
  digitalWrite(stb, HIGH);
}

void turnOn(Brightness brightness) {
  sendCommand(brightness == low ? 0x88 : 0x8F);
}

void turnOff() {
  sendCommand(0x80);
}

void sendCommand(uint8_t value) {
  pinMode(dio, OUTPUT);
  digitalWrite(stb, LOW);
  shiftOut(dio, clk, LSBFIRST, value);
  digitalWrite(stb, HIGH);
}

