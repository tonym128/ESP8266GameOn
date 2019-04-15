// #define FPS 1
#define ANALOG 1
// #define AUDIO 1
// #define DEBUG_OUTPUT

#include "game.hpp"
ScreenBuff screenBuff;
byte buttonVals;
std::array<int,8> buttonRaw;

#ifdef __EMSCRIPTEN__
#include "platform/plat_emscripten.h"
#elif SDL2_FOUND
#include "platform/sdl.h"
#elif _WIN32
#include "platform/win32.h"
#elif __linux
#include "platform/linux.h"
#elif ARDUINO
#include "platform/arduino.h"
#endif

void audioSetup()
{
  audioInit();
}

void gameSetup()
{
  showLogo(logo_image, &screenBuff);
  sendToScreen();
  updateMinTime(2000);
}

void gameLoop()
{
  audioLoop();

  // put your main code here, to run repeatedly:
  buttonVals = getReadShift();

  if (myGameLoop(&screenBuff, buttonVals))
  {
    gameSetup();
  }

  calcFPS();
#ifdef FPS // Define this to show the FPS for the game
  drawFPS(&screenBuff);
#endif
#ifdef DEBUG_OUTPUT
	std::array<int,8> rawValues = getRawInput();
  for(int i = 0; i < 8; i+=2) {
    	char fpsString[17];
    	sprintf(fpsString, "%d:%03d %d:%03d", i, rawValues[i],i+1, rawValues[i+1]);
    	drawString(&screenBuff, fpsString, 0, i * 4, true);
  }
#endif
  sendToScreen();
  updateMinTime(33);
}
