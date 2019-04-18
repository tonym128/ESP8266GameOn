#include "myGame.hpp"

GameState gameState;

void displayScroller(GameState *gameState, ScreenBuff *screenBuff)
{
	gameState->frameCounter += 1;
	char scrollerText[9][17];

	bool *Zero = font('0');
	bool *One = font('1');
	Dimensions dim;
	dim.width = 8;
	dim.height = 8;

	int x = 0;
	int y = -8 + gameState->frameCounter % 8;
	for (int i = 0; i < 17; i++) {
		x += 8;
		for (int j = 0; j < 9; j++) {
			y+=8;
			dim.x = x;
			dim.y = y;
			drawObject(screenBuff,dim, rand()%2 == 0 ? Zero : One);
		}
	}
}

bool myGameLoop(ScreenBuff *screenBuff, byte buttonVals)
{
	displayScroller(&gameState, screenBuff);
	return false;
}
