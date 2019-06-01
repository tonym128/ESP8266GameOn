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
  byte buttonVals = 0;
  buttonVals = buttonVals | (readAnalogSensor(T8) << P1_Left);
  buttonVals = buttonVals | (readAnalogSensor(T0) << P1_Top);
  buttonVals = buttonVals | (readAnalogSensor(T6) << P1_Right);
  buttonVals = buttonVals | (readAnalogSensor(T3) << P1_Bottom);
  buttonVals = buttonVals | (readAnalogSensor(T7) << P2_Left);
  buttonVals = buttonVals | (readAnalogSensor(T5) << P2_Right);
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
  rawValues[i++] = readAnalogSensorRaw(T8); // Left
  rawValues[i++] = readAnalogSensorRaw(T0); // Up
  rawValues[i++] = readAnalogSensorRaw(T6); // Right
  rawValues[i++] = readAnalogSensorRaw(T3); // Down
  rawValues[i++] = readAnalogSensorRaw(T7); // B
  rawValues[i++] = 0;//readAnalogSensorRaw(T5); // Select
  rawValues[i++] = readAnalogSensorRaw(T5); // A
  rawValues[i++] = 0;//readAnalogSensorRaw(T7); // Start

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
