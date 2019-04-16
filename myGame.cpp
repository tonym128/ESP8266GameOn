#include "myGame.hpp"

GameState gameState;

void processInput(GameState* gameState, byte buttonVals) {
	gameState->p1keys.up = processKey(buttonVals, P1_Top);
	gameState->p1keys.down = processKey(buttonVals, P1_Bottom);
	gameState->p1keys.left = processKey(buttonVals, P1_Left);
	gameState->p1keys.right = processKey(buttonVals, P1_Right);
	gameState->p1keys.kick = processKey(buttonVals, P2_Left);
	gameState->p1keys.punch = processKey(buttonVals, P2_Right);
	gameState->p1keys.jump = processKey(buttonVals, P2_Bottom);

	// This is only monitoring for a keypress on false
	if (processKey(buttonVals, P2_Top)) {
		gameState->running = false;
		gameState->restart = true;
	}
}

void initStar(GameState* gameState, ScreenBuff* screenBuff, Star* star, bool allowX = false) {
	int x = screenBuff->HEIGHT + 1;
	while (x > screenBuff->HEIGHT) x = 1 + rand() % screenBuff->HEIGHT;
	star->dim.y = x;

	if (allowX) {
		x = screenBuff->WIDTH + 1;
		while (x > screenBuff->WIDTH) x = 1 + rand() % screenBuff->WIDTH;
		star->dim.x = x;
	}
	else star->dim.x = screenBuff->WIDTH - 1;

	x = gameState->starMaxVelocity + 1;
	while (x > gameState->starMaxVelocity) x = 1 + rand() % gameState->starMaxVelocity;
	star->velocity = x;

	x = gameState->STARTYPES + 1;
	while (x > gameState->STARTYPES) x = 1 + rand() % gameState->STARTYPES;
	switch (x) {
	case 1:
		star->dim.width = 30;
		star->dim.height = 11;
		break;
	case 2:
		star->dim.width = 20;
		star->dim.height = 6;
		break;
	case 3:
		star->dim.width = 10;
		star->dim.height = 4;
		break;
	}

	star->collider = true;

	x = gameState->STARMAXSPAWNDELAY + 1;
	while (x > gameState->STARMAXSPAWNDELAY) x = 1 + rand() % gameState->STARMAXSPAWNDELAY;
	star->spawnDelay = x;
}

void updateStar(GameState* gameState, ScreenBuff* screenBuff, Star* star) {
	if (star->velocity == 0) initStar(gameState, screenBuff, star);

	if (star->spawnDelay > 0) {
		star->spawnDelay -= 1;
	}
	else if (gameState->frameCounter % star->velocity == 0) {
		star->dim.x -= 1;
		if (star->dim.x + star->dim.width == 0) initStar(gameState, screenBuff, star);
	}
}

void resetGameState(ScreenBuff* screenBuff) {
	// Reset Random Seed
	time_t now = time(nullptr);
	std::srand((int)now); // use current time as seed for random generator

	// Reset Game Objects
	gameState.starField = true;
	gameState.player1.dim.height = 12;
	gameState.player1.dim.width = 16;
	gameState.player1.inPlay = true;
	gameState.player1.dim.x = 0;
	gameState.player1.dim.y = (screenBuff->HEIGHT - gameState.player1.dim.height) / 2;

	// Reset State
	gameState.frameCounter = 0;
	gameState.running = true;
	gameState.restart = false;
}

void updateFly(GameState* gameState, ScreenBuff* screenBuff) {
	if (gameState->restartTimer) {
		gameState->frameCounter += 1;
		if (gameState->frameCounter > gameState->restartFrameCounter)
			gameState->restart = true;
	}

	if (gameState->restart) {
		resetGameState(screenBuff);
		gameState->level = 1;
		gameState->scene = 3;
		gameState->lastscene = 0;
		gameState->running = true;
		gameState->restart = false;
		gameState->player1.inPlay = true;
		gameState->collision = false;
		gameState->frameCounter = 0;
		gameState->restartTimer = false;
		return;
	}

	// Screen Logic
	if (!gameState->player1.inPlay || !gameState->running) return;

	gameState->frameCounter += 1;

	gameState->collision = false;

	// StarField
	if (gameState->starField == true) {
		for (int i = 0; i < gameState->starCount; i++) {
			updateStar(gameState, screenBuff, &gameState->stars[i]);
			if (!gameState->collision && gameState->stars[i].collider) {
				const bool* star;
				if (rectCollisionCheck(gameState->player1.dim, gameState->stars[i].dim)) {
					if (gameState->stars[i].dim.width == 30)
						star = star30x11;
					else if (gameState->stars[i].dim.width == 20)
						star = star20x6;
					else // (gameState->stars[i].dim.width == 10)
						star = star10x4;

					if (maskCollisionCheck(gameState->player1.dim, gameState->stars[i].dim, player, star)) {
						gameState->collision = true;

						char sound[] = "data/cloud2.wav";
						audioPlay(sound);
						
						break; // Only single collision needed / supported
					}
				}
			}
		}
	}

	int completion = (gameState->frameCounter * 100 / gameState->distanceTarget) / 25;

	if (gameState->collision) {
		if (gameState->player1.dim.y + gameState->player1.dim.height >= screenBuff->HEIGHT) {
			gameState->player1.inPlay = false;
			gameState->win = false;
			gameState->restartTimer = true;
			gameState->restartFrameCounter += gameState->frameCounter;
		}
	}

	if (completion == 4) {
		gameState->player1.inPlay = false;
		gameState->win = true;
		gameState->level += 1;
		gameState->scene = 4;

		if (gameState->level == 3)
		{
			gameState->level = 2;
			gameState->scene = 2;
		}
	}

	// Player movement
	if (gameState->collision) {
		gameState->player1.dim.y += 1;
		gameState->player1.dim.x += 1;
		if (gameState->player1.dim.y >= screenBuff->HEIGHT) {
			gameState->player1.inPlay = false;
			gameState->win = false;
		}
	}
	else {
		if (gameState->p1keys.down) { gameState->player1.dim.y += 1; }
		if (gameState->p1keys.up) { gameState->player1.dim.y -= 1; }
		if (gameState->p1keys.left) { gameState->player1.dim.x -= 1; }
		if (gameState->p1keys.right) { gameState->player1.dim.x += 1; }
	}

	// Clamp
	if (gameState->player1.dim.x + gameState->player1.dim.width > screenBuff->WIDTH) gameState->player1.dim.x = screenBuff->WIDTH - gameState->player1.dim.width;
	if (gameState->player1.dim.y + gameState->player1.dim.height > screenBuff->HEIGHT) gameState->player1.dim.y = screenBuff->HEIGHT - gameState->player1.dim.height;
	if (gameState->player1.dim.x < 0) gameState->player1.dim.x = 0;
	if (gameState->player1.dim.y < 0) gameState->player1.dim.y = 0;
	// Punch, kick, jump
}

bool updateScroller(GameState* gameState, ScreenBuff* screenBuff) {
	return true;
}

bool displayScroller(GameState* gameState, ScreenBuff* screenBuff) {
	gameState->frameCounter += 1;
	char scrollerText[9][17];
	strcpy(scrollerText[0],"The journey to");
	strcpy(scrollerText[1],"DefCon starts");
	strcpy(scrollerText[2],"with a single");
	strcpy(scrollerText[3],"step.");
	strcpy(scrollerText[4],"");
	strcpy(scrollerText[5],"  Fly To Vegas  ");
	strcpy(scrollerText[6],"   Good Luck!  ");
	strcpy(scrollerText[7],"");
	strcpy(scrollerText[8]," -= dFlight =-  ");

	return drawScroller(screenBuff, gameState->frameCounter, scrollerText);
}

bool updateOutroScroller(GameState* gameState, ScreenBuff* screenBuff) {
	return true;
}

bool displayOutroScroller(GameState* gameState, ScreenBuff* screenBuff) {
	gameState->frameCounter += 1;
	char scrollerText[9][17];
	strcpy(scrollerText[0],"Well done on");
	strcpy(scrollerText[1],"completing the");
	strcpy(scrollerText[2],"first leg of");
	strcpy(scrollerText[3],"your journey.");
	strcpy(scrollerText[4],"");
	strcpy(scrollerText[5],"Keep your head");
	strcpy(scrollerText[6],"high & go onward");
	strcpy(scrollerText[7],"");
	strcpy(scrollerText[8]," -= Congrats =- ");

	return drawScroller(screenBuff, gameState->frameCounter, scrollerText);
}

bool flyin(GameState* gameState, ScreenBuff* screenBuff) {
	gameState->frameCounter += 1;

	// Fly up at an angle till it hits middle screen height, then back
	if (gameState->player1.dim.y == (screenBuff->HEIGHT - gameState->player1.dim.height) / 2) {
		if (gameState->player1.dim.x == 0) {
			// Done
			return false;
		}
		gameState->player1.dim.x -= 1;
		return true;
	}

	gameState->player1.dim.x += 1;
	gameState->player1.dim.y -= 1;

	return true;
}

bool flyout(GameState* gameState, ScreenBuff* screenBuff) {
	gameState->frameCounter += 1;

	// Fly out the screen down at an angle.
	if (gameState->player1.dim.y > screenBuff->HEIGHT) {
		return false;
	}

	gameState->player1.dim.x += 1;
	gameState->player1.dim.y += 1;

	return true;
}

void displayFly(GameState* gameState, ScreenBuff* screenBuff) {
	if (gameState->running == false) return;

	displayClear(screenBuff, 1, 0);
	drawObject(screenBuff, gameState->player1.dim, player);

	if (gameState->starField == true) {

		for (int i = 0; i < gameState->starCount; i++) {
			// drawBlock(screenBuff, gameState->stars[i].dim, 1);
			if (gameState->stars[i].dim.width == 30)
				drawObject(screenBuff, gameState->stars[i].dim, star30x11);
			else if (gameState->stars[i].dim.width == 20)
				drawObject(screenBuff, gameState->stars[i].dim, star20x6);
			else // (gameState->stars[i].dim.width == 10)
				drawObject(screenBuff, gameState->stars[i].dim, star10x4);
		}
	}

	if (gameState->scene == 6) {
		if (gameState->running && gameState->collision) {
			displayNoise(screenBuff, 9);
			displayInvert(screenBuff);
		}

		// Display Text
		char fps[16];
		int percent = gameState->frameCounter * 100 / gameState->distanceTarget;
		int drawline = (percent * screenBuff->WIDTH) / 100;

		// Draw Progress Bar
		for (int draw = 0; draw < drawline; draw++) {
			screenBuff->consoleBuffer[draw] = 1;
			screenBuff->consoleBuffer[draw + screenBuff->WIDTH] = 1;
			screenBuff->consoleBuffer[draw + screenBuff->WIDTH * 2] = 1;
		}

		if (!gameState->player1.inPlay) {
			if (gameState->win) {
				sprintf(fps, "YOU WIN!");
				for (int i = 0; i < static_cast<int>(strlen(fps)); i++) {
					drawCharacter(screenBuff, fps[i], 32 + 8 * i, 30);
					gameState->player1.inPlay = false;
					gameState->running = false;
				}
			}
			else {
				sprintf(fps, "GAME OVER");
				for (int i = 0; i < static_cast<int>(strlen(fps)); i++) {
					drawCharacter(screenBuff, fps[i], 32 + 8 * i, 30);
				}
				gameState->player1.inPlay = false;
				gameState->running = false;
			}
		}
	}
	if (gameState->scene == 3) {
		char fps[16];
		sprintf(fps, "Level %i", gameState->level);
		for (int i = 0; i < static_cast<int>(strlen(fps)); i++) {
			drawCharacter(screenBuff, fps[i], 40 + 8 * i, 10);
		}

		// Sliders
		int counter = 0;
		for (int i = gameState->frameCounter; i > 0; i--) {
			if (counter < screenBuff->WIDTH / 2)
			{
				counter++;
				screenBuff->consoleBuffer[screenBuff->WIDTH * 5 + i] = 1;
				screenBuff->consoleBuffer[screenBuff->WIDTH * 20 - i] = 1;
			}
		}
	}
}

bool myGameLoop(ScreenBuff* screenBuff, byte buttonVals) {
	processInput(&gameState, buttonVals);

	switch (gameState.scene) {
	case 1: // Intro
		if (gameState.lastscene != gameState.scene) {
			gameState.lastscene = gameState.scene;
			gameState.frameCounter = 0;
		}

		updateScroller(&gameState, screenBuff);
		if (!displayScroller(&gameState, screenBuff)) {
			gameState.scene = 3;
		}

		break;
	case 2:  // Outro
		if (gameState.lastscene != gameState.scene) {
			gameState.lastscene = gameState.scene;
			gameState.frameCounter = 0;
		}

		updateOutroScroller(&gameState, screenBuff);
		if (!displayOutroScroller(&gameState, screenBuff)) {
			gameState.frameCounter = 0;
			gameState.level = 1;
			gameState.scene = 1;
			return true;
		}

		break;
	case 3: // Take Off
		if (gameState.lastscene != gameState.scene) {
			gameState.starCount = 0;
			resetGameState(screenBuff);
			gameState.lastscene = gameState.scene;
			gameState.player1.dim.x = gameState.player1.dim.width * -1;
			gameState.player1.dim.y = screenBuff->HEIGHT + gameState.player1.dim.y;
		}

		if (!flyin(&gameState, screenBuff)) {
			gameState.scene = 6;
		}

		displayFly(&gameState, screenBuff);

		break;
	case 4: // Land
		if (gameState.lastscene != gameState.scene) {
			gameState.lastscene = gameState.scene;
		}

		if (!flyout(&gameState, screenBuff))
		{
			gameState.scene = 3;
		}

		displayFly(&gameState, screenBuff);
		break;
	case 5: // Crash
		if (gameState.lastscene != gameState.scene) {
			gameState.lastscene = gameState.scene;
		}
		break;
	case 6: // Stage
		if (gameState.lastscene != gameState.scene) {
			gameState.lastscene = gameState.scene;
			switch (gameState.level) {
			case 1: gameState.starCount = 5;
				gameState.starMaxVelocity = 20;
				gameState.distanceTarget = 2000;
				free(gameState.stars);
				gameState.stars = new Star[gameState.starCount];
				for (int i = 0; i < gameState.starCount; i++) {
					initStar(&gameState, screenBuff, &gameState.stars[i]);
				}
				break;
			case 2: gameState.starCount = 10;
				gameState.starMaxVelocity = 30;
				gameState.distanceTarget = 3000;
				free(gameState.stars);
				gameState.stars = new Star[gameState.starCount];
				for (int i = 0; i < gameState.starCount; i++) {
					initStar(&gameState, screenBuff, &gameState.stars[i]);
				}
				break;
			case 3: gameState.starCount = 20;
				gameState.starMaxVelocity = 50;
				gameState.distanceTarget = 4000;
				free(gameState.stars);
				gameState.stars = new Star[gameState.starCount];
				for (int i = 0; i < gameState.starCount; i++) {
					initStar(&gameState, screenBuff, &gameState.stars[i]);
				}
				break;
			case 4:
				// Game has ended we switch to outro
				gameState.scene = 2;
				break;
			default:
				// Random logic for continuing if we wanted
				gameState.starCount = 10 * gameState.level;
				gameState.starMaxVelocity = 20 + 10 * gameState.level;
				gameState.distanceTarget = 1000 * gameState.level;
				free(gameState.stars);
				gameState.stars = new Star[gameState.starCount];
				for (int i = 0; i < gameState.starCount; i++) {
					initStar(&gameState, screenBuff, &gameState.stars[i]);
				}
				break;
			}

			resetGameState(screenBuff);
		}

		updateFly(&gameState, screenBuff);
		displayFly(&gameState, screenBuff);
		break;
	}

	return false;
}
