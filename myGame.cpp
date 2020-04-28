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

void resetGameState(GameState *gameState)
{
	srand((unsigned int)time(0));
	gameState->win = false;

	gameState->score = 0;
	gameState->level = 1;

	gameState->player1.rotation = 4;
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

	if (gameState->p1keys.left)
	{
		gameState->player1.rotation -= 0.1;
	}

	if (gameState->p1keys.right)
	{
		gameState->player1.rotation += 0.1;
	}

	if (gameState->player1.rotation < 0) gameState->player1.rotation = PI * 2;
	if (gameState->player1.rotation > PI * 2) gameState->player1.rotation = 0;
	//  Vector Movement
	int frameMs = currentTime - frameTime;
	if (frameMs == 0) frameMs = 1;

	if (gameState->p1keys.up)
	{
		gameState->player1.acceleration.force += FLOAT_TO_FIXP(0.10 / frameMs);
	}

	if (gameState->p1keys.down)
	{
		gameState->player1.acceleration.force -= FLOAT_TO_FIXP(0.04 / frameMs);
	}

	// Limits
	if (gameState->player1.acceleration.force > INT_TO_FIXP(2))
		gameState->player1.acceleration.force = INT_TO_FIXP(2);
	else if (gameState->player1.acceleration.force < INT_TO_FIXP(-1))
		gameState->player1.acceleration.force = INT_TO_FIXP(-1);

	// Drag
	if (gameState->player1.acceleration.force > INT_TO_FIXP(0))
	{
		gameState->player1.acceleration.force -= FLOAT_TO_FIXP(0.01 / frameMs);
	}
	else if (gameState->player1.acceleration.force < INT_TO_FIXP(0))
	{
		gameState->player1.acceleration.force += FLOAT_TO_FIXP(0.01 / frameMs);
	}

	if (gameState->player1.acceleration.force < FLOAT_TO_FIXP(0.01) && gameState->player1.acceleration.force > FLOAT_TO_FIXP(-0.01))
		gameState->player1.acceleration.force = INT_TO_FIXP(0);
}

int lastVal = 0;
int getCarDir(GameState *gameState)
{
	if (gameState->player1.rotation < 1. / 8. * PI)
		lastVal = 0;
	else if (gameState->player1.rotation < 3. / 8. * PI)
		lastVal = 1;
	else if (gameState->player1.rotation < 5. / 8. * PI)
		lastVal = 2;
	else if (gameState->player1.rotation < 7. / 8. * PI)
		lastVal = 3;
	else if (gameState->player1.rotation < 9. / 8. * PI)
		lastVal = 4;
	else if (gameState->player1.rotation < 11. / 8. * PI)
		lastVal = 5;
	else if (gameState->player1.rotation < 13. / 8. * PI)
		lastVal = 6;
	else if (gameState->player1.rotation < 15. / 8. * PI)
		lastVal = 7;
	else
		lastVal = 0;

	return lastVal;
}

void initAttractMode(GameState *gameState)
{
	gameState->player1.X = INT_TO_FIXP(60);
	gameState->player1.Y = INT_TO_FIXP(20);
	resetGameState(gameState);
}

void updateAttractMode(GameState *gameState, ScreenBuff *screenBuff)
{
	gameState->player1.acceleration.direction = FLOAT_TO_FIXP(gameState->player1.rotation * 57.2958);
	gameState->player1.X += xVec(gameState->player1.acceleration.force, gameState->player1.acceleration.direction);
	gameState->player1.Y += yVec(gameState->player1.acceleration.force, gameState->player1.acceleration.direction);
}

void displayAttractMode(GameState *gameState, ScreenBuff *screenBuff)
{
	// Alternate press button text on and off every second
	Dimensions screenmap;
	screenmap.height = 128;
	screenmap.width = 256;
	screenmap.x = FIXP_TO_INT(gameState->player1.X) - 64;
	screenmap.y = FIXP_TO_INT(gameState->player1.Y) - 32;
	if (screenmap.x < 0) screenmap.x = 0;
	if (screenmap.x > screenmap.width - 128) screenmap.x = screenmap.width - 128;

	if (screenmap.y < 0) screenmap.y = 0;
	if (screenmap.y > screenmap.height - 64) screenmap.y = screenmap.height - 64;

	screenmap.endx = screenmap.x + 128;
	screenmap.endy = screenmap.y + 64;
	screenmap.screenx = 0;
	screenmap.screeny = 0;

	drawObjectPartial(screenBuff, screenmap, (bool *)level1, true);

	Dimensions map;
	map.height = 8;
	map.width = 64;
	map.x = getCarDir(gameState) * 8;
	map.y = 0;
	map.endx = map.x + 8;
	map.endy = 8;
	map.screenx = FIXP_TO_INT(gameState->player1.X) - screenmap.x;
	map.screeny = FIXP_TO_INT(gameState->player1.Y) - screenmap.y;
	drawObjectPartial(screenBuff, map, (bool *)car1, true);

	// map.screenx = FIXP_TO_INT(gameState->player1.X);
	// map.screeny = FIXP_TO_INT(gameState->player1.Y) + 8;
	// drawObjectPartial(screenBuff, map, (bool *)car2, true);

	// if (getTimeInMillis() / 1000 % 2 == 0)
	// {
	// 	char startText[17] = "Press a button";
	// 	drawString(screenBuff, startText, 8, 38, false);
	// 	char startText2[17] = "to start";
	// 	drawString(screenBuff, startText2, 32, 45, false);
	// }

	char hiScore[17];
	sprintf(hiScore, "%d %d", FIXP_TO_INT(gameState->player1.X), FIXP_TO_INT(gameState->player1.Y));
	drawString(screenBuff, hiScore, 0, 0, true);
	//displayInvert(screenBuff);
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
		}
		else
		{
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
			resetGameState(&gameState);
		}

		gameState.scene = 4;

		break;
	}

	return false;
}
