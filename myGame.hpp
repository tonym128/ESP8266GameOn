#ifndef MYGAME_H
#define MYGAME_H 1

#ifndef GAMECOMMON_H
#include "gameCommon.hpp"
#endif

struct GameState {
  int frameCounter = 0;
  char line1[120] = "10000000001011010011001011110110100101101101111000011101000010110101111100101010000001001101100111100110000010101100100";
  char line2[120] = "11101001001100100001110100101111101011110110101001100000000100001110011011011000100101111101011111010110100011000011110";
  char line3[120] = "10000000001011010011001011110110100101101101111000011101000010110101111100101010000001001101100111100110000010101100100";
  char line4[120] = "10000000001011010011001011110110100101101101111000011101000010110101111100101010000001001101100111100110000010101100100";
  char line5[120] = "10000000001011010011001011110110100101101101111000011101000010110101111100101010000001001101100111100110000010101100100";
  char line6[120] = "10000000001011010011001011110110100101101101111000011101000010110101111100101010000001001101100111100110000010101100100";
  char line7[120] = "10000000001011010011001011110110100101101101111000011101000010110101111100101010000001001101100111100110000010101100100";
  char line8[120] = "10000000001011010011001011110110100101101101111000011101000010110101111100101010000001001101100111100110000010101100100";
};

bool myGameLoop(ScreenBuff*, byte);
#endif //MYGAME_H