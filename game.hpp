#ifndef _GAME_H
#define _GAME_H

// #define FPS 1
// #define DEBUG_OUTPUT

// #define ANALOG 1
#define AUDIO 1

// Define the screen you're using below, and define check the ESP32.h or ESP8266.h for pin settings
// This has only been setup for the ESP32, but it can be backported
#define SCREEN_ST7789
// #define SCREEN_SSD1306

#include "gameCommon.hpp"
#include "myGame.hpp"

void audioSetup();
void gameInit();
void gameSetup();
void gameLoop();

#endif
