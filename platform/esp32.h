#include "SSD1306.h"
#include <WiFi.h>

#include "FS.h"

SSD1306 display(0x3c, 21, 22); 
static int TOUCH_SENSE = 12;

int inputVal = 0;
bool readAnalogSensor(int pin)
{
  inputVal = touchRead(pin);

  Serial.print("Touch value is Pin");
  Serial.print(pin);
  Serial.print(" = ");
  Serial.println( inputVal);

  return inputVal < TOUCH_SENSE;
}

int readAnalogSensorRaw(int pin) {
  inputVal = touchRead(pin);

  Serial.print("Touch value is Pin");
  Serial.print(pin);
  Serial.print(" = ");
  Serial.println( inputVal);

  return inputVal;
}

byte getReadShiftAnalog()
{
  readAnalogSensorRaw(T8);

  byte buttonVals = 0;
  buttonVals = buttonVals | (readAnalogSensor(T8) << P1_Left);
  buttonVals = buttonVals | (0 << P1_Top);
  buttonVals = buttonVals | (readAnalogSensor(T2) << P1_Right);
  buttonVals = buttonVals | (0 << P1_Bottom);
  buttonVals = buttonVals | (readAnalogSensor(T0) << P2_Left);
  buttonVals = buttonVals | (readAnalogSensor(T1) << P2_Right);
  buttonVals = buttonVals | (0 << P2_Top);
  buttonVals = buttonVals | (0 << P2_Bottom);
  return buttonVals;
}

std::array<int,8> getRawInput() {
  std::array<int,8> rawValues;
	for (int i = 0; i < 8; i++) {
		rawValues[i] = 0;
	}

  int i = 0;
  rawValues[i++] = touchRead(T8); // Left
  rawValues[i++] = 0; // Up
  rawValues[i++] = 0; // Right
  rawValues[i++] = 0; // Down
  rawValues[i++] = 0; // B
  rawValues[i++] = 0; // Select
  rawValues[i++] = 0; // A
  rawValues[i++] = 0; // Start

  return rawValues;
}

byte getReadShift()
{
    return getReadShiftAnalog();
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
  Serial.println("gameInit");
  display.init();
  display.displayOn();
  display.flipScreenVertically();
}
