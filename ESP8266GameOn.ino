#include "game.hpp"

void setup() {
  Serial.begin(921600);
  Serial.println("setup");
  
  audioSetup();
  gameInit();
  gameSetup();
}

void loop() {
  gameLoop();
}
