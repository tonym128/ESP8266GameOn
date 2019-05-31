#include "SSD1306.h"
#include <WiFi.h>

#include "FS.h"

SSD1306 display(0x3c, 21, 22); 

int inputVal = 0;
bool readAnalogSensor(int pin)
{
  inputVal = 0;
  digitalWrite(pin, LOW);

  return inputVal > 20;
}

#ifdef ANALOG
int readAnalogSensorRaw(int pin) {
  inputVal = 0;
  return inputVal;
}

byte getReadShiftAnalog()
{
  byte buttonVals = 0;
  return buttonVals;
}
#elif
byte getReadShiftDigital()
{
  int inputPin = 1;
  int buttonPressedVal = 1; //Depending on how buttons are wired
  byte buttonVals = 0;
  return buttonVals;
}
#endif

std::array<int,8> getRawInput() {
  std::array<int,8> rawValues;
	for (int i = 0; i < 8; i++) {
		rawValues[i] = 0;
	}

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
  Serial.println("gameInit");
  display.init();
  display.displayOn();
  display.flipScreenVertically();
}
