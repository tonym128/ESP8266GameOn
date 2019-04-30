#include "myGame.hpp"

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

	gameState->player1.dim.width = 10;
	gameState->player1.dim.height = 10;
	gameState->player1.dim.x = 16;
	gameState->player1.dim.y = 27;
}

void updateGame(GameState *gameState, ScreenBuff *screenBuff)
{
}

void displayGame(GameState *gameState, ScreenBuff *screenBuff)
{
	displayClear(screenBuff, 1, false);
	bool rotatedShip[100];
	rotateObject(gameState->player1.dim,,1.0,Ship10x10,rotatedShip);
	drawObject(screenBuff,gameState->player1.dim, rotatedShip);
}

void processAttractMode(GameState *gameState, ScreenBuff *screenBuff)
{
	// Press A or B to starts
	if (gameState->p1keys.a || gameState->p1keys.b)
	{
		gameState->scene = 1;
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
	// Clear the screen
	displayClear(screenBuff, 1, false);

	// Alternate press button text on and off every second
	if (getTimeInMillis() / 1000 % 2 == 0)
	{
		char scrollerText[17] = "Press a button";
		drawString(screenBuff, scrollerText, 8, 38, false);
		char scrollerText2[17] = "to start";
		drawString(screenBuff, scrollerText2, 32, 45, false);
	}

	char hiScore[17];
	sprintf(hiScore,"%d",gameState->hiScore);
	drawString(screenBuff, hiScore, 0, 0, true);
}

bool myGameLoop(ScreenBuff *screenBuff, byte buttonVals)
{
	processInput(&gameState, buttonVals);

	switch (gameState.scene)
	{
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
	case 1: // Flappy Bird in a Ship!
		if (gameState.lastscene != gameState.scene)
		{
			gameState.lastscene = gameState.scene;
			resetGameState(&gameState, screenBuff);
		}

		updateGame(&gameState, screenBuff);
		displayGame(&gameState, screenBuff);
		break;
	case 2: // Game Over
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

	break;
	}

	return false;
}
