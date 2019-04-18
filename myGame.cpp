#include "myGame.hpp"

GameState gameState;

void displayScroller(GameState *gameState, ScreenBuff *screenBuff)
{
	// Stabalise characters by doing initial seed and pulling characters from there, looping
	gameState->frameCounter += 1;

	bool *Zero = font('0');
	bool *One = font('1');
	bool *Space = font(' ');

	Dimensions dim;
	dim.width = 8;
	dim.height = 8;

	int x = 0;
	int yinit = -8 + gameState->frameCounter % 16/2;
	for (int i = 0; i < 17; i++) {
		dim.x = x;
		int y = yinit;
		for (int j = 0; j < 9; j++) {
			y += 8;
			dim.y = y;
			int random = rand()%3;

			switch (random) {
				case 0: drawObject(screenBuff,dim, Zero, true);
				break;
				case 1: drawObject(screenBuff,dim, One, true);
				break;
				case 2: drawObject(screenBuff,dim, Space, true);
				break;
			}
		}
		x += 8;
	}
}

bool myGameLoop(ScreenBuff *screenBuff, byte buttonVals)
{
	// Optimisations to do
	// Only draw the first top 0-8 y lines, copy the rest of the display downwards x pixels.
	// To introduce line speed do this for each of the 16 columns
	displayClear(screenBuff, 1, 0);
	displayScroller(&gameState, screenBuff);
	return false;
}
