#include <TFT_eSPI.h>
#include <WiFi.h>
 
#include "FS.h"

static int TOUCH_SENSE = 20;

#ifdef SCREEN_ST7789
TFT_eSPI tft = TFT_eSPI();   // Invoke library
TFT_eSprite spr = TFT_eSprite(&tft); // Declare Sprite object "spr" with pointer to "tft" object
#else
#include "SSD1306.h"
SSD1306 display(0x3c, 21, 22); 
#endif

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

#ifdef SCREEN_ST7789
void sendToScreen()
{
  spr.fillScreen(TFT_BLACK);

  int x = 0;
  int y = 0;

  for (int i = 0; i < screenBuff.WIDTH * screenBuff.HEIGHT; i++)
  {
    if (screenBuff.consoleBuffer[i])
    {
      int x = i % screenBuff.WIDTH;
      int y = i / screenBuff.WIDTH;
      spr.drawPixel(x,y,TFT_WHITE);
    }
  }

  spr.pushSprite(0, 0);
}

void gameInit()
{
  Serial.println("gameInit");
  tft.begin();     // initialize a ST7789 chip
  tft.setSwapBytes(true); // Swap the byte order for pushImage() - corrects endianness
  tft.fillScreen(TFT_BLACK);

  spr.setColorDepth(1);
  spr.createSprite(128, 64);
  spr.fillScreen(TFT_BLACK);
}
#else
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
#endif