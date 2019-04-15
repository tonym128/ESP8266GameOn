#include <brzo_i2c.h>
#include "SSD1306Brzo.h"
#include <ESP8266WiFi.h>
#include "FS.h"

SSD1306Brzo display(0x3c, D1, D4);

/* Shift In  */
const int pinShcp = 15;   //Clock
const int pinStcp = 0;    //Latch
const int pinDataIn = 16; // Data

int inputVal = 0;
bool readAnalogSensor(int pin)
{
  digitalWrite(pin, HIGH);

  inputVal = analogRead(A0);

  digitalWrite(pin, LOW);

  return inputVal > 20;
}

int readAnalogSensorRaw(int pin) {
  digitalWrite(pin, HIGH);

  inputVal = analogRead(A0);

  digitalWrite(pin, LOW);
  return inputVal;
}

byte getReadShiftAnalog()
{
  byte buttonVals = 0;
  if (readAnalogSensor(D6))
  {
    Serial.println("[.] Button Left pressed!");
    buttonVals = buttonVals | (1 << P1_Left);
  }
  if (readAnalogSensor(D8))
  {
    Serial.println("[.] Button Up pressed!");
    buttonVals = buttonVals | (1 << P1_Top);
  }
  if (readAnalogSensor(D7))
  {
    Serial.println("[.] Button Right pressed!");
    buttonVals = buttonVals | (1 << P1_Right);
  }
  if (readAnalogSensor(D5))
  {
    Serial.println("[.] Button Down pressed!");
    buttonVals = buttonVals | (1 << P1_Bottom);
  }

  if (readAnalogSensor(D2))
  {
    Serial.println("[.] Button B pressed!");
    buttonVals = buttonVals | (1 << P2_Left);
  }
  if (readAnalogSensor(D3))
  {
    Serial.println("[.] Button A pressed!");
    buttonVals = buttonVals | (1 << P2_Right);
  }

  return buttonVals;
}

byte getReadShiftDigital()
{
  int inputPin = 1;
  int buttonPressedVal = 1; //Depending on how buttons are wired
  digitalWrite(pinStcp, LOW);
  delayMicroseconds(20);
  digitalWrite(pinStcp, HIGH);

  byte buttonVals = 0;
  for (int i = 0; i < 8; i++)
  {
    digitalWrite(pinShcp, LOW);
    delayMicroseconds(20);
    inputPin = digitalRead(pinDataIn);
    if (inputPin == buttonPressedVal)
    {
      buttonVals = buttonVals | (1 << i);
    }

    digitalWrite(pinShcp, HIGH);
  }

  return buttonVals;
}

std::array<int,8> getRawInput() {
  std::array<int,8> rawValues;
	for (int i = 0; i < 8; i++) {
		rawValues[i] = 0;
	}

#ifdef ANALOG
  int i = 0;
  rawValues[i++] = readAnalogSensorRaw(D6); // Left
  rawValues[i++] = readAnalogSensorRaw(D8); // Up
  rawValues[i++] = readAnalogSensorRaw(D7); // Right
  rawValues[i++] = readAnalogSensorRaw(D5); // Down
  rawValues[i++] = readAnalogSensorRaw(D2); // B
  rawValues[i++] = 0; // Missing
  rawValues[i++] = readAnalogSensorRaw(D3); // A
  rawValues[i++] = 0; // Missing
#else
  int inputPin = 1;
  int buttonPressedVal = 1; //Depending on how buttons are wired
  digitalWrite(pinStcp, LOW);
  delayMicroseconds(20);
  digitalWrite(pinStcp, HIGH);

  byte buttonVals = 0;
  for (int i = 0; i < 8; i++)
  {
    digitalWrite(pinShcp, LOW);
    delayMicroseconds(20);
    inputPin = digitalRead(pinDataIn);
    rawValues[i] = inputPin == buttonPressedVal ? 1 : 0;
    digitalWrite(pinShcp, HIGH);
  }
#endif
  return rawValues;
}

byte getReadShift()
{
#ifdef ANALOG
    return getReadShiftAnalog();
#else
  return getReadShiftDigital();
#endif
}

void sendToScreen()
{
  display.setColor(BLACK);
  display.clear();
  int x = 0;
  int y = 0;

  display.setColor(WHITE);
  for (int i = 0; i < screenBuff.WIDTH * screenBuff.HEIGHT; i++)
  {
    if (screenBuff.consoleBuffer[i])
    {
      int x = i % screenBuff.WIDTH;
      int y = i / screenBuff.WIDTH;
      display.setPixel(x, y);
    }
  }

  display.display();
}

void gameInit()
{
  //Serial.begin(114200);
  Serial.println("gameInit");
 /* shift in */
#ifdef ANALOG
    // PINS and Analog buttons
    pinMode(D5, OUTPUT);
    pinMode(D6, OUTPUT);
    pinMode(D7, OUTPUT);
    pinMode(D8, OUTPUT);
    pinMode(D2, OUTPUT);
    pinMode(D3, OUTPUT);
    digitalWrite(D5, LOW);
    digitalWrite(D6, LOW);
    digitalWrite(D7, LOW);
    digitalWrite(D8, LOW);
    digitalWrite(D2, LOW);
    digitalWrite(D3, LOW);
#else 
    // BSide Cape Town 2016 Badge
    pinMode(pinStcp, OUTPUT);
    pinMode(pinShcp, OUTPUT);
    pinMode(pinDataIn, INPUT);

    /* shift out */
    pinMode(latchPin, OUTPUT);
    pinMode(dataPin, OUTPUT);
    pinMode(clockPin, OUTPUT);
#endif

  // Startup SPIFFS Storage
  SPIFFS.begin();

  display.init();
  display.displayOn();
  display.flipScreenVertically();
}
