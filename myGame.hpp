#ifndef MYGAME_H
#define MYGAME_H 1

#ifndef GAMECOMMON_H
#include "gameCommon.hpp"
#endif

struct Player1Keys {
  bool up = false;
  bool down = false;
  bool right = false;
  bool left = false;
  bool a = false;
  bool b = false;
};

struct Player {
  Dimensions dim;
  bool inPlay = true;
};

struct GameState {
  int hiScore = 0;
  int score = 0;
  int scene = 0;
  int lastscene = -99;
  int frameCounter = 0;

  // Scoring mechanism variables
  int lastScore = 0;

  // Game mechanics
  Player1Keys p1keys;
  Player player1;

  bool win = false;
  bool running = true;
  bool restart = false;
};

bool myGameLoop(ScreenBuff*, byte);

const static bool Ship10x10[100] = {
  0,0,0,0,1,1,0,0,0,0,
  0,0,1,1,1,1,1,1,0,0,
  0,0,1,1,1,1,1,1,0,0,
  0,0,1,1,0,0,1,1,1,0,
  0,1,1,1,0,0,1,1,1,0,
  0,1,1,1,1,1,1,1,1,0,
  0,1,1,1,1,1,1,1,1,0,
  0,1,1,0,0,0,0,1,1,0,
  0,1,1,0,0,0,0,1,1,0,
  0,0,0,0,0,0,0,0,0,0
};

#endif
