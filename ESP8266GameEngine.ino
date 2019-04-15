#include "game.hpp"

void setup() {
  Serial.begin(114200);
  Serial.println("setup");
  
  audioSetup();
  gameInit();
  gameSetup();
}

void loop() {
  gameLoop();
}
