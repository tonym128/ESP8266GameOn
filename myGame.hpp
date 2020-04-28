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

struct Vec3 {
  FIXPOINT x,y;
  FIXPOINT direction;
  FIXPOINT force;
  FIXPOINT time;
};

struct Player {
  FIXPOINT X,Y;
  double rotation;
  Dimensions dim;
  Vec3 acceleration;
  FIXPOINT drag = FLOAT_TO_FIXP(0.3);
};

struct GameState {
  int hiScore = 0;
  int score = 0;
  int level = 1;
  int scene = -1;
  int lastscene = -99;
  int frameCounter = 0;

  Player1Keys p1keys;
  Player player1;

  FIXPOINT StageX = FIXP_TO_INT(0), StageY = FIXP_TO_INT(0);

  bool win = false;
  bool running = true;
  bool restart = false;
};

bool myGameLoop(ScreenBuff*, byte);

#endif
