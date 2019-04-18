#ifndef MYGAME_H
#define MYGAME_H 1

#ifndef GAMECOMMON_H
#include "gameCommon.hpp"
#endif

struct GameState {
  int frameCounter = 0;
};

bool myGameLoop(ScreenBuff*, byte);
#endif //MYGAME_H