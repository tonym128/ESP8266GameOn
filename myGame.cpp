#include "myGame.hpp"
#include "image.h"

GameState gameState;

void processInput(GameState *gameState, byte buttonVals)
{
	gameState->p1keys.up = processKey(buttonVals, P1_Top);
	gameState->p1keys.down = processKey(buttonVals, P1_Bottom);
	gameState->p1keys.left = processKey(buttonVals, P1_Left);
	gameState->p1keys.right = processKey(buttonVals, P1_Right);
	gameState->p1keys.a = processKey(buttonVals, P2_Right);
	gameState->p1keys.b = processKey(buttonVals, P2_Left);

	// This is only monitoring for a keypress on false
	if (processKey(buttonVals, P2_Top))
	{
		gameState->running = false;
		gameState->restart = true;
	}
}

void resetGameState(GameState *gameState, ScreenBuff *screenBuff)
{
	srand((unsigned int)time(0));
	gameState->win = false;

	gameState->player1.collision = false;
	gameState->player1.inPlay = true;

	gameState->score = 0;
	gameState->level = 1;

	gameState->player1.dim.width = 10;
	gameState->player1.dim.height = 10;
}

void startLevel(GameState *gameState, ScreenBuff *screenBuff)
{

}

void updateGame(GameState *gameState, ScreenBuff *screenBuff)
{

}

void displayGame(GameState *gameState, ScreenBuff *screenBuff)
{
	displayClear(screenBuff, 1, false);
}

void processAttractMode(GameState *gameState, ScreenBuff *screenBuff)
{
	// Press A or B to starts
	if (gameState->p1keys.a || gameState->p1keys.b)
	{
		gameState->scene = 1;
	}

	if (gameState->p1keys.left) {
		gameState->player1.X -= FLOAT_TO_FIXP(0.25);
		if (FIXP_TO_INT(gameState->player1.X) < 0) gameState->player1.X = INT_TO_FIXP(0);
	}

	if (gameState->p1keys.right) {
		gameState->player1.X += FLOAT_TO_FIXP(0.25);
		if (FIXP_TO_INT(gameState->player1.X) > 256 - 128) gameState->player1.X = INT_TO_FIXP(256 - 128);
	}

	if (gameState->p1keys.up) {
		gameState->player1.Y -= FLOAT_TO_FIXP(0.25);
		if (FIXP_TO_INT(gameState->player1.Y) < 0) gameState->player1.Y = INT_TO_FIXP(0);

	}

	if (gameState->p1keys.down) {
		gameState->player1.Y += FLOAT_TO_FIXP(0.25);
		if (FIXP_TO_INT(gameState->player1.Y) > 128 - 64) gameState->player1.Y = INT_TO_FIXP(128 - 64);
	}

}

void initAttractMode(GameState *gameState)
{

}

void updateAttractMode(GameState *gameState, ScreenBuff *screenBuff)
{

}

void displayAttractMode(GameState *gameState, ScreenBuff *screenBuff)
{
	// Alternate press button text on and off every second
	Dimensions map;
	map.height = 128;
	map.width = 256;
	map.x = FIXP_TO_INT(gameState->player1.X);
	map.y = FIXP_TO_INT(gameState->player1.Y);
	map.endx = FIXP_TO_INT(gameState->player1.X) + 128;
	map.endy = FIXP_TO_INT(gameState->player1.Y) + 64;
	map.screenx = 0;
	map.screeny = 0;

	drawObjectPartial(screenBuff,map,(bool *)level1, true);

	map.height = 8;
	map.width = 64;
	map.x = 8;
	map.y = 0;
	map.endx = map.x + 8;
	map.endy = 8;
	map.screenx = 60;
	map.screeny = 20;
	drawObjectPartial(screenBuff,map,(bool *)car1, true);
	if (getTimeInMillis() / 1000 % 2 == 0)
	{
		char startText[17] = "Press a button";
		drawString(screenBuff, startText, 8, 38, false);
		char startText2[17] = "to start";
		drawString(screenBuff, startText2, 32, 45, false);
	}

	char hiScore[17];
	sprintf(hiScore,"%d %d",FIXP_TO_INT(gameState->player1.X),FIXP_TO_INT(gameState->player1.Y));
	drawString(screenBuff, hiScore, 0, 0, true);
}

bool myGameLoop(ScreenBuff *screenBuff, byte buttonVals)
{
	processInput(&gameState, buttonVals);

	switch (gameState.scene)
	{
	case -1: // Load Screen
		if (gameState.lastscene != gameState.scene)
		{
			gameState.lastscene = gameState.scene;
			//showLogo(mygame_image, screenBuff);
		}
		else
		{
			updateMinTime(100);
			gameState.scene++;
		}
		break;
	case 0: // Attract screen
		if (gameState.lastscene != gameState.scene)
		{
			gameState.lastscene = gameState.scene;
			initAttractMode(&gameState);
		}

		processAttractMode(&gameState, screenBuff);
		updateAttractMode(&gameState, screenBuff);
		displayAttractMode(&gameState, screenBuff);
		break;
	case 1: // Intro
		if (gameState.lastscene != gameState.scene)
		{
			gameState.lastscene = gameState.scene;
			gameState.frameCounter = 0;
			resetGameState(&gameState, screenBuff);
		}

		gameState.scene = 4;

		break;
	case 3: // Asteroid!
		if (gameState.lastscene != gameState.scene)
		{
			gameState.lastscene = gameState.scene;
			if (gameState.level == 1)
			{
				resetGameState(&gameState, screenBuff);
				startLevel(&gameState, screenBuff);
			}
		}

		if (gameState.win)
		{
			if (gameState.level >= 10)
			{
				gameState.scene = 2;
				gameState.win = false;
			}
			else
			{
				startLevel(&gameState, screenBuff);
				gameState.scene = 4;
			}
			return false;
		}
		updateGame(&gameState, screenBuff);
		displayGame(&gameState, screenBuff);
		break;
	case 2: // Outro
		if (gameState.lastscene != gameState.scene)
		{
			gameState.lastscene = gameState.scene;
			gameState.frameCounter = 0;
		}

		gameState.scene = 0;
		gameState.win = false;
		return false;

		break;
	case 5: // Game Over
	{
		if (gameState.lastscene != gameState.scene)
		{
			gameState.frameCounter = 0;
			gameState.lastscene = gameState.scene;
		}

		if (gameState.frameCounter == 100)
		{
			gameState.scene = 0;
		}

		char gameOver[16];
		sprintf(gameOver, "Game Over");
		for (int i = 0; i < static_cast<int>(strlen(gameOver)); i++)
		{
			drawCharacter(screenBuff, gameOver[i], 40 + 8 * i, 25);
		}

		// Sliders
		int cnt = 0;
		for (int i = gameState.frameCounter; i > 0; i--)
		{
			if (cnt < screenBuff->WIDTH / 2)
			{
				cnt++;
				screenBuff->consoleBuffer[screenBuff->WIDTH * 20 + i] = 1;
				screenBuff->consoleBuffer[screenBuff->WIDTH * 35 - i] = 1;
			}
		}

		gameState.frameCounter++;
	}
	break;
	case 4: // Level Slider
	{
		if (gameState.lastscene != gameState.scene)
		{
			gameState.frameCounter = 0;
			gameState.lastscene = gameState.scene;
			displayClear(screenBuff, 0, false);
		}

		if (gameState.frameCounter == 100)
		{
			gameState.scene = 3;
		}

		displayClear(screenBuff, 0, false);

		char fps[16];
		sprintf(fps, "Level %i", gameState.level);
		for (int i = 0; i < static_cast<int>(strlen(fps)); i++)
		{
			drawCharacter(screenBuff, fps[i], 40 + 8 * i, 25);
		}

		// Sliders
		int counter = 0;
		for (int i = gameState.frameCounter; i > 0; i--)
		{
			if (counter < screenBuff->WIDTH / 2)
			{
				counter++;
				screenBuff->consoleBuffer[screenBuff->WIDTH * 20 + i] = 1;
				screenBuff->consoleBuffer[screenBuff->WIDTH * 35 - i] = 1;
			}
		}
		gameState.frameCounter++;
	}
	break;
	}

	return false;
}
