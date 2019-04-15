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

	gameState->player1.collision = false;
	gameState->player1.inPlay = true;

	gameState->score = 0;
	gameState->level = 1;

	gameState->player1.dim.width = 10;
	gameState->player1.dim.height = 10;
}

void startLevel(GameState *gameState, ScreenBuff *screenBuff)
{
	gameState->win = false;
	gameState->player1.collision = false;

	// Initialise player
	gameState->player1.direction = 0;
	gameState->player1.rotation = 0;
	gameState->player1.thrust = FLOAT_TO_FIXP(0.05 + gameState->level / (double)100);
	gameState->player1.dim.x = screenBuff->WIDTH / 2;
	gameState->player1.dim.y = screenBuff->HEIGHT / 2;

	gameState->player1.fixX = INT_TO_FIXP(gameState->player1.dim.x);
	gameState->player1.fixY = INT_TO_FIXP(gameState->player1.dim.y);

	gameState->player1.movX = INT_TO_FIXP(0);
	gameState->player1.movY = INT_TO_FIXP(0);

	// No weapons fire
	for (int i = 0; i < FIRECOUNT; i++)
	{
		gameState->player1.fire[i].life = 0;
	}

	// 1 Asteroid per stage
	for (int i = 0; i < ASTEROIDS; i++)
	{
		gameState->asteroids[i].dim.width = 0;
		gameState->asteroids[i].dim.height = 0;
	}

	for (int i = 0; i < gameState->level; i++)
	{
		int size = 20;
		gameState->asteroids[i].speed = FLOAT_TO_FIXP((double)(rand() % 10) / (double)10);
		gameState->asteroids[i].size = FLOAT_TO_FIXP(size);
		gameState->asteroids[i].direction = INT_TO_FIXP(rand() % 360);
		gameState->asteroids[i].rotateAmount = rand() % 4000;
		gameState->asteroids[i].dim.width = size;
		gameState->asteroids[i].dim.height = size;

		gameState->asteroids[i].dim.x = rand() % 2 == 0 ? 0 : screenBuff->WIDTH;
		gameState->asteroids[i].dim.y = rand() % 2 == 0 ? 0 : screenBuff->HEIGHT;
		gameState->asteroids[i].fixX = INT_TO_FIXP(gameState->asteroids[i].dim.x);
		gameState->asteroids[i].fixY = INT_TO_FIXP(gameState->asteroids[i].dim.y);
	}
}

bool updateScroller(GameState *gameState, ScreenBuff *screenBuff)
{
	return true;
}

bool displayScroller(GameState *gameState, ScreenBuff *screenBuff)
{
	gameState->frameCounter += 1;
	char scrollerText[9][17];
	strcpy(scrollerText[0], "The longest");
	strcpy(scrollerText[1], "journey starts");
	strcpy(scrollerText[2], "with a single");
	strcpy(scrollerText[3], "step.");
	strcpy(scrollerText[4], "");
	strcpy(scrollerText[5], "  Or Asteroid  ");
	strcpy(scrollerText[6], "   Good Luck!  ");
	strcpy(scrollerText[7], "");
	strcpy(scrollerText[8], " -= Asteroid =- ");

	return drawScroller(screenBuff, gameState->frameCounter, scrollerText);
}

bool updateOutroScroller(GameState *gameState, ScreenBuff *screenBuff)
{
	return true;
}

bool displayOutroScroller(GameState *gameState, ScreenBuff *screenBuff)
{
	gameState->frameCounter += 1;
	char scrollerText[9][17];
	strcpy(scrollerText[0], "Well done on");
	strcpy(scrollerText[1], "saving earth");
	strcpy(scrollerText[2], "Your mission");
	strcpy(scrollerText[3], "is done.");
	strcpy(scrollerText[4], "Get back to");
	strcpy(scrollerText[5], "earth & get some");
	strcpy(scrollerText[6], "R & R");
	strcpy(scrollerText[7], "                ");
	strcpy(scrollerText[8], " -= Congrats =- ");

	return drawScroller(screenBuff, gameState->frameCounter, scrollerText);
}

void updateGame(GameState *gameState, ScreenBuff *screenBuff)
{
	// If there's no Asteroids left ... WIN!
	gameState->win = true;
	for (int i = 0; i < ASTEROIDS; i++)
	{
		if (gameState->asteroids[i].dim.height != 0)
		{
			gameState->win = false;
		}
	}

	if (gameState->win)
	{
		gameState->level++;
	}

	// Proces player input
	if (gameState->p1keys.left)
	{
		gameState->player1.rotation -= 0.1;
		if (gameState->player1.rotation < 0)
			gameState->player1.rotation = 6;
		gameState->player1.direction = FLOAT_TO_FIXP(gameState->player1.rotation * 60);
	}

	if (gameState->p1keys.right)
	{
		gameState->player1.rotation += 0.1;
		if (gameState->player1.rotation > 6)
			gameState->player1.rotation = 0;
		gameState->player1.direction = FLOAT_TO_FIXP(gameState->player1.rotation * 60);
	}

	if (gameState->p1keys.up || gameState->p1keys.b)
	{
		gameState->player1.movX += xVec(gameState->player1.thrust, gameState->player1.direction);
		gameState->player1.movY += yVec(gameState->player1.thrust, gameState->player1.direction);
	}

	if (gameState->p1keys.a)
	{
		if (gameState->player1.firetimeout < getTimeInMillis())
		{
			for (int i = 0; i < FIRECOUNT; i++)
			{
				if (gameState->player1.fire[i].life > 0)
					continue;
				gameState->player1.firetimeout = getTimeInMillis() + FIREPACING;
				gameState->player1.fire[i].dim.height = 1;
				gameState->player1.fire[i].dim.width = 1;
				gameState->player1.fire[i].dim.x = gameState->player1.dim.x + gameState->player1.dim.width / 2;
				gameState->player1.fire[i].dim.y = gameState->player1.dim.y + gameState->player1.dim.height / 2;
				gameState->player1.fire[i].fixX = gameState->player1.fixX + INT_TO_FIXP(gameState->player1.dim.width / 2);
				gameState->player1.fire[i].fixY = gameState->player1.fixY + INT_TO_FIXP(gameState->player1.dim.height / 2);
				gameState->player1.fire[i].movX = gameState->player1.movX;
				gameState->player1.fire[i].movY = gameState->player1.movY;

				gameState->player1.fire[i].movX += xVec(FIREPOWER, gameState->player1.direction);
				gameState->player1.fire[i].movY += yVec(FIREPOWER, gameState->player1.direction);

				gameState->player1.fire[i].life = INT_TO_FIXP(1000);
				break;
			}
		}
	}

	// Update ship position
	gameState->player1.fixX += gameState->player1.movX;
	gameState->player1.fixY += gameState->player1.movY;

	if (gameState->player1.fixX < INT_TO_FIXP(0))
		gameState->player1.fixX += INT_TO_FIXP(screenBuff->WIDTH);
	if (gameState->player1.fixX > INT_TO_FIXP(screenBuff->WIDTH))
		gameState->player1.fixX -= INT_TO_FIXP(screenBuff->WIDTH);

	if (gameState->player1.fixY < INT_TO_FIXP(0))
		gameState->player1.fixY += INT_TO_FIXP(screenBuff->HEIGHT);
	if (gameState->player1.fixY > INT_TO_FIXP(screenBuff->HEIGHT))
		gameState->player1.fixY -= INT_TO_FIXP(screenBuff->HEIGHT);

	gameState->player1.dim.x = FIXP_TO_INT(gameState->player1.fixX) % screenBuff->WIDTH;
	gameState->player1.dim.y = FIXP_TO_INT(gameState->player1.fixY) % screenBuff->HEIGHT;

	// Update the Asteroids
	for (int i = 0; i < ASTEROIDS; i++)
	{
		if (gameState->asteroids[i].dim.width)
		{
			gameState->asteroids[i].fixX += xVec(gameState->asteroids[i].speed, gameState->asteroids[i].direction);
			gameState->asteroids[i].fixY += yVec(gameState->asteroids[i].speed, gameState->asteroids[i].direction);

			if (gameState->asteroids[i].fixX < INT_TO_FIXP(0))
				gameState->asteroids[i].fixX += INT_TO_FIXP(screenBuff->WIDTH);
			if (gameState->asteroids[i].fixX > INT_TO_FIXP(screenBuff->WIDTH))
				gameState->asteroids[i].fixX -= INT_TO_FIXP(screenBuff->WIDTH);

			if (gameState->asteroids[i].fixY < INT_TO_FIXP(0))
				gameState->asteroids[i].fixY += INT_TO_FIXP(screenBuff->HEIGHT);
			if (gameState->asteroids[i].fixY > INT_TO_FIXP(screenBuff->HEIGHT))
				gameState->asteroids[i].fixY -= INT_TO_FIXP(screenBuff->HEIGHT);

			gameState->asteroids[i].dim.x = FIXP_TO_INT(gameState->asteroids[i].fixX) % screenBuff->WIDTH;
			gameState->asteroids[i].dim.y = FIXP_TO_INT(gameState->asteroids[i].fixY) % screenBuff->HEIGHT;
			gameState->asteroids[i].rotation = ((double)getTimeInMillis()) / gameState->asteroids[i].rotateAmount;
		}
	}

	// Update the weapons
	for (int i = 0; i < FIRECOUNT; i++)
	{
		if (gameState->player1.fire[i].life <= 0)
			continue;

		gameState->player1.fire[i].fixX += gameState->player1.fire[i].movX;
		gameState->player1.fire[i].fixY += gameState->player1.fire[i].movY;

		if (gameState->player1.fire[i].fixX < INT_TO_FIXP(0))
			gameState->player1.fire[i].fixX += INT_TO_FIXP(screenBuff->WIDTH);
		if (gameState->player1.fire[i].fixX > INT_TO_FIXP(screenBuff->WIDTH))
			gameState->player1.fire[i].fixX -= INT_TO_FIXP(screenBuff->WIDTH);

		if (gameState->player1.fire[i].fixY < INT_TO_FIXP(0))
			gameState->player1.fire[i].fixY += INT_TO_FIXP(screenBuff->HEIGHT);
		if (gameState->player1.fire[i].fixY > INT_TO_FIXP(screenBuff->HEIGHT))
			gameState->player1.fire[i].fixY -= INT_TO_FIXP(screenBuff->HEIGHT);

		gameState->player1.fire[i].dim.x = FIXP_TO_INT(gameState->player1.fire[i].fixX) % screenBuff->WIDTH;
		gameState->player1.fire[i].dim.y = FIXP_TO_INT(gameState->player1.fire[i].fixY) % screenBuff->HEIGHT;

		gameState->player1.fire[i].life -= getCurrentFPS();
	}

	// Collision detect ship vs Asteroids
	bool rotShip[100];
	bool rotAst[400];
	rotateObject(gameState->player1.dim, gameState->player1.rotation, 1, Ship10x10, rotShip);
	for (int i = 0; i < ASTEROIDS; i++)
	{
		if (gameState->player1.collision)
			break;
		// If it's a valid asteroid
		if (gameState->asteroids[i].dim.width)
		{
			// If we have a collision on the bounding box
			if (rectCollisionCheck(gameState->player1.dim, gameState->asteroids[i].dim))
			{
				//Do a mask collision check
				switch (gameState->asteroids[i].dim.height)
				{
				case 20:
					rotateObject(gameState->asteroids[i].dim, gameState->asteroids[i].rotation, 1, Asteroid20x20, rotAst);
					break;
				case 10:
					rotateObject(gameState->asteroids[i].dim, gameState->asteroids[i].rotation, 1, Asteroid10x10, rotAst);
					break;
				case 5:
					rotateObject(gameState->asteroids[i].dim, gameState->asteroids[i].rotation, 1, Asteroid5x5, rotAst);
					break;
				}

				if (maskCollisionCheck(gameState->player1.dim, gameState->asteroids[i].dim, rotShip, rotAst))
				{
					gameState->player1.collision = true;
				}
			}
		}
	}

	// Collider Check
	if (gameState->player1.collision)
	{
		gameState->player1.inPlay = false;
		gameState->scene = 5;
		if (gameState->score > gameState->hiScore) {
			gameState->hiScore = gameState->score;
		}
	}

	// Collision check bullets vs Asteroids
	for (int i = 0; i < ASTEROIDS; i++)
	{
		// If it's a valid asteroid
		if (gameState->asteroids[i].dim.width)
		{
			// If we have a collision on the bounding box
			for (int j = 0; j < FIRECOUNT; j++)
			{
				if (gameState->player1.fire[j].life <= 0)
					continue;
				if (rectCollisionCheck(gameState->player1.fire[j].dim, gameState->asteroids[i].dim))
				{
					//Do a mask collision check
					switch (gameState->asteroids[i].dim.height)
					{
					case 20:
						rotateObject(gameState->asteroids[i].dim, gameState->asteroids[i].rotation, 1, Asteroid20x20, rotAst);
						break;
					case 10:
						rotateObject(gameState->asteroids[i].dim, gameState->asteroids[i].rotation, 1, Asteroid10x10, rotAst);
						break;
					case 5:
						rotateObject(gameState->asteroids[i].dim, gameState->asteroids[i].rotation, 1, Asteroid5x5, rotAst);
						break;
					}

					if (maskCollisionCheck(gameState->player1.fire[j].dim, gameState->asteroids[i].dim, Bullet, rotAst))
					{
						//Explode this one!
						// If the score time multiplier wasn't over (1 second default) then give bonus points.
						if (gameState->scoreTimeMultiplier > getTimeInMillis())
						{
							gameState->score += gameState->asteroids[i].dim.width;
						}
						// Reset score multiplier
						gameState->scoreTimeMultiplier = getTimeInMillis() + SCORETIMEMULTTIMEOUT;

						// If the size is the same as the last asteroid, multiplier goes up.
						if (gameState->lastScore == gameState->asteroids[i].dim.width)
						{
							// Multiplier code
							if (gameState->multiplier > gameState->maxMultiplier)
							{
								gameState->multiplier = gameState->maxMultiplier;
							}
							else
							{
								gameState->multiplier += 1;
							}
						}
						else
						{
							// Reset the multiplier
							gameState->multiplier = 1;
						}

						gameState->score += gameState->asteroids[i].dim.width * gameState->multiplier;

						int counter = 0;
						int size = 0;

						switch (gameState->asteroids[i].dim.width)
						{
						case 10:
							//Spawn 3 x 5
							counter = 2;
							size = 5;
							break;
						case 20:
							//Spawn 3 x 10
							counter = 2;
							size = 10;
							break;
						}

						// Spawn Asteroids
						while (counter > 0)
						{
							for (int emptyAsteroid = 0; emptyAsteroid < ASTEROIDS; emptyAsteroid++)
							{
								if (gameState->asteroids[emptyAsteroid].dim.width == 0)
								{
									counter--;
									gameState->asteroids[emptyAsteroid].fixX = gameState->asteroids[i].fixX;
									gameState->asteroids[emptyAsteroid].fixY = gameState->asteroids[i].fixY;

									gameState->asteroids[emptyAsteroid].speed = FLOAT_TO_FIXP((double)(rand() % 10) / (double)10);
									gameState->asteroids[emptyAsteroid].size = FLOAT_TO_FIXP(size);
									gameState->asteroids[emptyAsteroid].direction = INT_TO_FIXP(rand() % 360);
									gameState->asteroids[emptyAsteroid].rotateAmount = rand() % 4000;
									gameState->asteroids[emptyAsteroid].dim.width = size;
									gameState->asteroids[emptyAsteroid].dim.height = size;
									break;
								}
							}
						}

						gameState->asteroids[i].dim.width = 0;
						gameState->asteroids[i].dim.height = 0;

						// Bullet is done
						gameState->player1.fire[j].life = 0;
					}
				}
			}
		}
	}
}

void displayGame(GameState *gameState, ScreenBuff *screenBuff)
{
	displayClear(screenBuff, 1, false);

	// temporary rotation variable.
	bool rotAst[400];

	// Draw ship
	rotateObject(gameState->player1.dim, gameState->player1.rotation, 1, Ship10x10, rotAst);
	drawObjectWrap(screenBuff, gameState->player1.dim, rotAst);

	//Draw asteroids
	for (int i = 0; i < ASTEROIDS; i++)
	{
		if (gameState->asteroids[i].dim.height > 0)
		{
			switch (gameState->asteroids[i].dim.height)
			{
			case 20:
				rotateObject(gameState->asteroids[i].dim, gameState->asteroids[i].rotation, 1, Asteroid20x20, rotAst);
				break;
			case 10:
				rotateObject(gameState->asteroids[i].dim, gameState->asteroids[i].rotation, 1, Asteroid10x10, rotAst);
				break;
			case 5:
				rotateObject(gameState->asteroids[i].dim, gameState->asteroids[i].rotation, 1, Asteroid5x5, rotAst);
				break;
			}
			drawObjectWrap(screenBuff, gameState->asteroids[i].dim, rotAst);
		}
	}
	char topline[17];
	sprintf(topline, "Scr %5d Lvl %2d", gameState->score, gameState->level);
	//sprintf(topline, "%3.1f %3.1f %3.1f",FIXP_TO_FLOAT(gameState->player1.direction),	FIXP_TO_FLOAT(xVec(FIXP_1, gameState->player1.direction)),FIXP_TO_FLOAT(yVec(FIXP_1, gameState->player1.direction)));
	drawString(screenBuff, topline, 0, 0, true);

	// Draw bullets
	for (int i = 0; i < FIRECOUNT; i++)
	{
		if (gameState->player1.fire[i].life <= 0)
			continue;
		drawObjectWrap(screenBuff, gameState->player1.fire[i].dim, Bullet);
	}
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
	for (int i = 0; i < ASTEROIDS; i++)
	{
		gameState->asteroids[i].dim.width = 0;
		gameState->asteroids[i].dim.height = 0;
	}

	for (int i = 0; i < rand() % 8 + 2; i++)
	{
		int size = rand() % 3;
		switch (size)
		{
		case 0:
			size = 5;
			break;
		case 1:
			size = 10;
			break;
		case 2:
			size = 20;
			break;
		}

		gameState->asteroids[i].speed = FLOAT_TO_FIXP((double)(rand() % 10) / (double)10);
		gameState->asteroids[i].size = FLOAT_TO_FIXP(size);
		gameState->asteroids[i].direction = INT_TO_FIXP(rand() % 360);
		gameState->asteroids[i].rotateAmount = rand() % 4000;
		gameState->asteroids[i].dim.width = size;
		gameState->asteroids[i].dim.height = size;
	}
}

void updateAttractMode(GameState *gameState, ScreenBuff *screenBuff)
{
	// Draw some asteroids floating in the background
	for (int i = 0; i < ASTEROIDS; i++)
	{
		if (gameState->asteroids[i].dim.width)
		{
			gameState->asteroids[i].fixX += xVec(gameState->asteroids[i].speed, gameState->asteroids[i].direction);
			gameState->asteroids[i].fixY += yVec(gameState->asteroids[i].speed, gameState->asteroids[i].direction);

			if (gameState->asteroids[i].fixX < INT_TO_FIXP(0))
				gameState->asteroids[i].fixX += INT_TO_FIXP(screenBuff->WIDTH);
			if (gameState->asteroids[i].fixX > INT_TO_FIXP(screenBuff->WIDTH))
				gameState->asteroids[i].fixX -= INT_TO_FIXP(screenBuff->WIDTH);

			if (gameState->asteroids[i].fixY < INT_TO_FIXP(0))
				gameState->asteroids[i].fixY += INT_TO_FIXP(screenBuff->HEIGHT);
			if (gameState->asteroids[i].fixY > INT_TO_FIXP(screenBuff->HEIGHT))
				gameState->asteroids[i].fixY -= INT_TO_FIXP(screenBuff->HEIGHT);

			gameState->asteroids[i].dim.x = FIXP_TO_INT(gameState->asteroids[i].fixX) % screenBuff->WIDTH;
			gameState->asteroids[i].dim.y = FIXP_TO_INT(gameState->asteroids[i].fixY) % screenBuff->HEIGHT;
			gameState->asteroids[i].rotation = ((double)getTimeInMillis()) / gameState->asteroids[i].rotateAmount;
		}
	}
}

void displayAttractMode(GameState *gameState, ScreenBuff *screenBuff)
{
	// Clear the screen
	displayClear(screenBuff, 1, false);

	//Draw asteroids
	bool rotAst[400];
	for (int i = 0; i < ASTEROIDS; i++)
	{
		if (gameState->asteroids[i].dim.height > 0)
		{
			switch (gameState->asteroids[i].dim.height)
			{
			case 20:
				rotateObject(gameState->asteroids[i].dim, gameState->asteroids[i].rotation, 1, Asteroid20x20, rotAst);
				break;
			case 10:
				rotateObject(gameState->asteroids[i].dim, gameState->asteroids[i].rotation, 1, Asteroid10x10, rotAst);
				break;
			case 5:
				rotateObject(gameState->asteroids[i].dim, gameState->asteroids[i].rotation, 1, Asteroid5x5, rotAst);
				break;
			}
			drawObjectWrap(screenBuff, gameState->asteroids[i].dim, rotAst);
		}
	}

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
	case -1: // Load Screen
		if (gameState.lastscene != gameState.scene)
		{
			gameState.lastscene = gameState.scene;
			showLogo(mygame_image, screenBuff);
		}
		else
		{
			updateMinTime(2000);
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

		updateScroller(&gameState, screenBuff);
		if (!displayScroller(&gameState, screenBuff))
		{
			gameState.scene = 4;
		}

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

		updateOutroScroller(&gameState, screenBuff);
		if (!displayOutroScroller(&gameState, screenBuff))
		{
			gameState.scene = 0;
			gameState.win = false;
			return false;
		}

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
