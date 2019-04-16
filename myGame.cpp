#include "myGame.hpp"

struct Player1Keys {
	bool up = false;
	bool down = false;
	bool right = false;
	bool left = false;
	bool shoot = false;
	bool turbo = false;
	bool brake = false;
	bool accelerate = false;
};

struct BufferedDrawObject {
	bool draw = false;
	Dimensions dim;
	bool* object;
};

struct Car {
	unsigned char speed = 0;
	unsigned char y = 0;
	unsigned char lane = 0;
	bool visible = 0;
	int distance = 0;
	int lastdistance = 0;
};

struct GameStateDrive {
	const int MaxOffRoadSpeed = 25;
	const int maxRoadSpeed = 70;
	const int crashSpeed = 0;
	const int udpateSpeed = 30;

	bool win = false;
	int level = 1;
	int stageTime = 60;
	int stageDistance = 800;
	int maxCars = 1;

	int flagCount = 0;
	int flagFrameCounter = 0;
	bool flagUp = true;

	Car* cars = new Car[5];

	int scene = 0;

	int lastscene = 0;

	int frameCounter = 0;
	int distance = 0;
	int lastdistance = 0;
	int turnOffset = 0;

	bool turning = false;
	bool turningLeft = false;
	int turnDepth = 0;

	int carSpeed = 0;
	int roadSpeed = 0;
	bool evenLines = true;
	Player1Keys p1keys;
	Dimensions playerCar;
	bool collision = false;
	bool running = true;
	bool restart = false;
} gameStateDrive;

void processInput(GameStateDrive* gameStateDrive, byte buttonVals) {
	gameStateDrive->p1keys.up = processKey(buttonVals, P1_Top);
	gameStateDrive->p1keys.down = processKey(buttonVals, P1_Bottom);
	gameStateDrive->p1keys.left = processKey(buttonVals, P1_Left);
	gameStateDrive->p1keys.right = processKey(buttonVals, P1_Right);
	gameStateDrive->p1keys.brake = processKey(buttonVals, P2_Left) || processKey(buttonVals, P1_Top);
	gameStateDrive->p1keys.accelerate = processKey(buttonVals, P2_Right) || processKey(buttonVals, P1_Bottom);

	if (processKey(buttonVals, P2_Top)) {
		gameStateDrive->running = false;
		gameStateDrive->restart = true;
	}
}

void initCar(GameStateDrive* gameStateDrive, ScreenBuff* screenBuff, Car* car) {
	car->y = screenBuff->HEIGHT / 3;
	car->lane = rand() % 3;
	car->speed = rand() % gameStateDrive->maxRoadSpeed / 4;
	car->distance = gameStateDrive->distance;
	car->lastdistance = gameStateDrive->distance;
}

bool updateDrive(GameStateDrive* gameStateDrive, ScreenBuff* screenBuff) {
	gameStateDrive->frameCounter += 1;
	if (gameStateDrive->playerCar.x < screenBuff->WIDTH / 5 || gameStateDrive->playerCar.x + gameStateDrive->playerCar.width > screenBuff->WIDTH - screenBuff->WIDTH / 5) {
		if (gameStateDrive->MaxOffRoadSpeed < gameStateDrive->carSpeed) gameStateDrive->carSpeed -= 2;
	}

	// Collision
	if (gameStateDrive->collision) {
		gameStateDrive->carSpeed = 0;
		gameStateDrive->collision = false;
	}

	if (gameStateDrive->p1keys.accelerate) {
		// Accelerating
		gameStateDrive->carSpeed += 1;
	}
	else {
		// Organic slow down
		gameStateDrive->carSpeed -= gameStateDrive->frameCounter % 20 == 0 ? 1 : 0;
	}

	gameStateDrive->carSpeed -= gameStateDrive->p1keys.brake ? 1 : 0;
	if (gameStateDrive->carSpeed < 0) gameStateDrive->carSpeed = 0;
	if (gameStateDrive->carSpeed > gameStateDrive->maxRoadSpeed) gameStateDrive->carSpeed = gameStateDrive->maxRoadSpeed;

	if (gameStateDrive->p1keys.left) {
		gameStateDrive->playerCar.x -= 1;
	}
	if (gameStateDrive->p1keys.right) {
		gameStateDrive->playerCar.x += 1;
	}

	// Randomly Start Turning
	if (!gameStateDrive->turning) {
		gameStateDrive->turning = rand() % 100 == 0;
		if (gameStateDrive->turning) {
			gameStateDrive->turningLeft = !gameStateDrive->turningLeft;
			gameStateDrive->turnDepth = rand() % 100;
		}
	}

	// Setup Road speed and distance travelled.
	gameStateDrive->roadSpeed = gameStateDrive->maxRoadSpeed + 1 - gameStateDrive->carSpeed;
	gameStateDrive->distance += gameStateDrive->carSpeed / 8;

	// Do turning logic
	if (gameStateDrive->turning) {
		if (gameStateDrive->distance - gameStateDrive->lastdistance >= gameStateDrive->udpateSpeed) {
			// Pull Car
			gameStateDrive->playerCar.x += gameStateDrive->turningLeft ? +1 : -1;

			// Going up
			if (gameStateDrive->turnDepth > 0) {
				gameStateDrive->turnOffset += gameStateDrive->turningLeft ? -1 : +1;
				gameStateDrive->turnDepth -= 1;
			}

			// Going down / finished
			if (gameStateDrive->turnDepth == 0) {
				if (gameStateDrive->turnOffset == 0) {
					gameStateDrive->turning = false;
				}
				else {
					gameStateDrive->turnOffset += gameStateDrive->turningLeft ? +1 : -1;
				}
			}

			if (gameStateDrive->turnOffset < -47) { gameStateDrive->turnOffset = -47; }
			if (gameStateDrive->turnOffset > 47) { gameStateDrive->turnOffset = 47; }
		}
	}

	if (gameStateDrive->playerCar.x < 0) {
		gameStateDrive->playerCar.x = 0;
	} else if (gameStateDrive->playerCar.x + gameStateDrive->playerCar.width > screenBuff->WIDTH) {
		gameStateDrive->playerCar.x = screenBuff->WIDTH - gameStateDrive->playerCar.width;
	}

	if (gameStateDrive->distance - gameStateDrive->lastdistance >= gameStateDrive->udpateSpeed) {
		gameStateDrive->evenLines = !gameStateDrive->evenLines;
		gameStateDrive->lastdistance = gameStateDrive->distance;
	}

	// initialise / update cars
	for (int i = 0; i < gameStateDrive->maxCars; i++) {
		if (gameStateDrive->cars[i].y == 0 || gameStateDrive->cars[i].y == screenBuff->HEIGHT) {
			initCar(gameStateDrive, screenBuff, &gameStateDrive->cars[i]);
		}
		else {
			gameStateDrive->cars[i].distance += gameStateDrive->cars[i].speed;

			if (gameStateDrive->cars[i].distance - gameStateDrive->distance >= gameStateDrive->udpateSpeed) {
				gameStateDrive->cars[i].distance = gameStateDrive->distance;
				gameStateDrive->cars[i].y -= 1;
			}
			if (gameStateDrive->distance - gameStateDrive->cars[i].distance >= gameStateDrive->udpateSpeed) {
				gameStateDrive->cars[i].distance = gameStateDrive->distance;
				gameStateDrive->cars[i].y += 1;
			}
		}
	}

	// If we have run out of time
	if (checkTime(gameStateDrive->stageTime)) {
		gameStateDrive->win = false;
		return false;
	}
	// If we have hit the required distance
	if (gameStateDrive->stageDistance - gameStateDrive->distance / 8 < 0)
	{
		gameStateDrive->win = true;
		return false;
	}

	return true;
}

bool drawWavingFlag(GameStateDrive* gameStateDrive, ScreenBuff* screenBuff) {
	gameStateDrive->frameCounter += 1;

	displayClear(screenBuff, 0, 0);
	Dimensions dimFlag;
	dimFlag.x = 5;
	dimFlag.y = 5;
	dimFlag.height = flag_height;
	dimFlag.width = flag_width;
	
	if (gameStateDrive->flagCount == 0) {
		gameStateDrive->flagUp = true;

		if (gameStateDrive->flagFrameCounter == 4) {
			gameStateDrive->flagCount--;
			gameStateDrive->flagFrameCounter = 0;
		}
		else {
			gameStateDrive->flagFrameCounter++;
		}
	}
	else if (gameStateDrive->flagCount == -4) {
		gameStateDrive->flagUp = false;

		if (gameStateDrive->flagFrameCounter == 4) {
			gameStateDrive->flagCount++;
			gameStateDrive->flagFrameCounter = 0;
		}
		else {
			gameStateDrive->flagFrameCounter++;
		}
	}
	else
	{
		gameStateDrive->flagFrameCounter = gameStateDrive->flagFrameCounter == 0 ? 0 : 1;
		if (gameStateDrive->flagFrameCounter == 0) gameStateDrive->flagCount += gameStateDrive->flagUp ? -1 : +1;
	}

	drawObjectWavy(screenBuff, dimFlag, -4, 0, gameStateDrive->flagCount, gameStateDrive->flagFrameCounter, gameStateDrive->flagUp, flag);

	return !checkTime(3);
}

bool updateDriveScroller(GameStateDrive* gameStateDrive, ScreenBuff* screenBuff) {
	return true;
}

bool displayDriveScroller(GameStateDrive* gameStateDrive, ScreenBuff* screenBuff) {
	gameStateDrive->frameCounter += 1;
	char scrollerText[9][17];
	strcpy(scrollerText[0],"You made it to");
	strcpy(scrollerText[1],"Las Vegas,");
	strcpy(scrollerText[2],"America,");
	strcpy(scrollerText[3],"Hell Yeah!");
	strcpy(scrollerText[4],"Now get to the");
	strcpy(scrollerText[5],"Hotel for some");
	strcpy(scrollerText[6],"rest");
	strcpy(scrollerText[7],"");
	strcpy(scrollerText[8]," -= Road Rush =-");

	return drawScroller(screenBuff, gameStateDrive->frameCounter, scrollerText);
}

bool displayDriveOutroScroller(GameStateDrive* gameStateDrive, ScreenBuff* screenBuff) {
	gameStateDrive->frameCounter += 1;
	char scrollerText[9][17];
	strcpy(scrollerText[0],"Hotel mission");
	strcpy(scrollerText[1],"accomplished.");
	strcpy(scrollerText[2],"");
	strcpy(scrollerText[3],"Get some rest");
	strcpy(scrollerText[4],"then head out to");
	strcpy(scrollerText[5],"the conference!");
	strcpy(scrollerText[6],"");
	strcpy(scrollerText[7],"");
	strcpy(scrollerText[8]," -= Winner! =-");

	return drawScroller(screenBuff, gameStateDrive->frameCounter, scrollerText);
}

bool displayLevelSlider(GameStateDrive* gameStateDrive, ScreenBuff* screenBuff) {
	gameStateDrive->frameCounter++;

	displayClear(screenBuff, 0, 0);
	char fps[16];
	sprintf(fps, "Level %i", gameStateDrive->level);
	for (int i = 0; i < static_cast<int>(strlen(fps)); i++) {
		drawCharacter(screenBuff, fps[i], 40 + 8 * i, 10);
	}

	// Sliders
	int counter = 0;
	for (int i = gameStateDrive->frameCounter; i > 0; i--) {
		if (counter < screenBuff->WIDTH / 2)
		{
			counter++;
			screenBuff->consoleBuffer[screenBuff->WIDTH * 5 + i] = 1;
			screenBuff->consoleBuffer[screenBuff->WIDTH * 20 - i] = 1;
		}
	}

	return !checkTime(2);
}

bool displayWinLose(GameStateDrive* gameStateDrive, ScreenBuff* screenBuff) {
	char fps[16];
	if (gameStateDrive->win) {
		sprintf(fps, "YOU WIN!");
		for (int i = 0; i < static_cast<int>(strlen(fps)); i++) {
			drawCharacter(screenBuff, fps[i], 32 + 8 * i, 30);
		}
	}
	else {
		sprintf(fps, "GAME OVER");
		for (int i = 0; i < static_cast<int>(strlen(fps)); i++) {
			drawCharacter(screenBuff, fps[i], 32 + 8 * i, 30);
		}
	}

	return !checkTime(2);
}

void displayDrive(GameStateDrive* gameStateDrive, ScreenBuff* screenBuff) {
	displayClear(screenBuff, 0, 0);

	int width = screenBuff->WIDTH / 15;
	int startY = screenBuff->HEIGHT / 3;

	int height = 1;
	int j = 0;
	int y = startY;
	Dimensions dim;

	while (y < screenBuff->HEIGHT) {
		// Draw side of road
		dim.x = 0;
		dim.y = y;
		dim.height = height;
		dim.width = screenBuff->WIDTH;

		drawBlock(screenBuff, dim, j % 2 == 0 ? gameStateDrive->evenLines : !gameStateDrive->evenLines);

		y += height + 1;
		j += 1;

		if (j % 5 == 0) height += 1;
	}

	// Draw the road
	y = screenBuff->HEIGHT / 3;
	j = 0;

	int turnOffset = gameStateDrive->turnOffset;
	int fullHeight = screenBuff->HEIGHT - (screenBuff->HEIGHT / 3);
	BufferedDrawObject bufDraw[5];

	while (y < screenBuff->HEIGHT) {
		// Draw Off Road
		dim.x = (screenBuff->WIDTH  - width - y) /2 - 2 + turnOffset;
		dim.y = y;
		dim.height = 0;
		dim.width = width + y + 4;
		drawBlock(screenBuff, dim, y % 2 ? gameStateDrive->evenLines : !gameStateDrive->evenLines);
	
		// Draw Road
		dim.x += 2;
		dim.width -= 4;
		drawBlock(screenBuff, dim, true);

		// Draw Cars
		for (int i = 0; i < gameStateDrive->maxCars; i++) {
			// If this is the right line to draw the car
			if (gameStateDrive->cars[i].y == y) {
				Dimensions dimCar;
				dimCar.x = 0;
				dimCar.height = 0;
				dimCar.width = 0;

				bool* car = (bool*)playerCar;
				dimCar.y = gameStateDrive->cars[i].y;

				// And the car is on screen
				if (dimCar.y > 0 && dimCar.y < screenBuff->HEIGHT) {
					dimCar.x = dim.x;

					// Draw the correct size car
					switch (((gameStateDrive->cars[i].y - (screenBuff->HEIGHT / 3)) * 8) / fullHeight) {
					case 0: dim.height = playerCar1_height;
						dimCar.width = playerCar1_width;
						dimCar.x += 7;
						car = (bool*)playerCar1;
						break;
					case 1: dimCar.height = playerCar2_height;
						dimCar.width = playerCar2_width;
						dimCar.x += 7;
						car = (bool*)playerCar2;
						break;
					case 2: dimCar.height = playerCar4_height;
						dimCar.x += 6;
						dimCar.width = playerCar4_width;
						car = (bool*)playerCar4;
						break;
					case 3: dimCar.height = playerCar8_height;
						dimCar.width = playerCar8_width;
						dimCar.x += 4;
						car = (bool*)playerCar8;
						break;
					default: dimCar.height = playerCar_height;
						dimCar.width = playerCar_width;
						car = (bool*)playerCar;
						break;
					}

					int laneSpace = dim.width / 3;
					switch (gameStateDrive->cars[i].lane) {
					case 0: break;
					case 1: dimCar.x += laneSpace; break;
					case 2: dimCar.x += laneSpace * 2 + dimCar.width / 2; break;
					}

					bufDraw[i].dim = dimCar;
					bufDraw[i].draw = true;
					bufDraw[i].object = car;
				}
			}
		}

		// Progress Loop
		y += 1;
		j += 1;
		
		// Change the offset every second line (give a nice flow to the bottom)
		if (y % 2 == 0 && turnOffset != 0) {
			turnOffset = (int)(turnOffset * 0.9);
		}
	}

	// Draw Cars
	for (int i = 0; i < gameStateDrive->maxCars; i++)
	{
		if (bufDraw[i].draw) {
			drawObjectFill(screenBuff, bufDraw[i].dim, bufDraw[i].object, 0);
		}
	}

	// Draw Trees
	BufferedDrawObject player;
	player.dim = gameStateDrive->playerCar;

	// Draw Player
	if (gameStateDrive->p1keys.left) {
		player.object = (bool *)playerCarLeft;
	}
	else if (gameStateDrive->p1keys.right) {
		player.object = (bool *)playerCarRight;
	}
	else {
		player.object = (bool *)playerCar;
	}

	drawObjectFill(screenBuff, player.dim, player.object, 0);

	// Collision detection
	for (int i = 0; i < gameStateDrive->maxCars; i++) {
		if (bufDraw[i].draw) {
			if (rectCollisionCheck(bufDraw[i].dim, player.dim)) {
				if (maskCollisionCheck(bufDraw[i].dim, player.dim, bufDraw[i].object, player.object)) {
					gameStateDrive->collision = true;
					displayNoise(screenBuff, bufDraw[i].dim, 0);
					displayNoise(screenBuff, gameStateDrive->playerCar, 0);
				}
			}
		}
	}

	// Draw console
	char speed[20];
	sprintf(speed, "%i mph, %i feet", gameStateDrive->carSpeed, gameStateDrive->stageDistance - gameStateDrive->distance / 8);
	drawString(screenBuff, speed, 0, 0, false);
	sprintf(speed, "%i s", (int)gameStateDrive->stageTime - getElapsedSeconds());
	drawString(screenBuff, speed, 0, 8, false);
}

void startUpGame(GameStateDrive* gameStateDrive, ScreenBuff* screenBuff) {
	initTime();
	
	gameStateDrive->playerCar.x = screenBuff->WIDTH / 2 - playerCar_width / 2;
	gameStateDrive->playerCar.y = screenBuff->HEIGHT - playerCar_height - 4;
	gameStateDrive->playerCar.width = playerCar_width;
	gameStateDrive->playerCar.height = playerCar_height;

	
	switch (gameStateDrive->level)  {
	case 1:
		gameStateDrive->maxCars = 1;
		gameStateDrive->stageDistance = 800;
		gameStateDrive->stageTime = 60;
		break;
	case 2:
		gameStateDrive->maxCars = 2;
		gameStateDrive->stageDistance = 1000;
		gameStateDrive->stageTime = 70;
		break;
	}

	for (int i = 0; i < gameStateDrive->maxCars; i++) {
		initCar(gameStateDrive, screenBuff, &gameStateDrive->cars[i]);
	}
	
	gameStateDrive->frameCounter = 0;
	gameStateDrive->distance = 0;
	gameStateDrive->lastdistance = 0;
	gameStateDrive->turnOffset = 0;

	gameStateDrive->turning = false;
	gameStateDrive->turningLeft = false;
	gameStateDrive->turnDepth = 0;

	gameStateDrive->carSpeed = 0;
	gameStateDrive->roadSpeed = 0;
	gameStateDrive->evenLines = true;
	gameStateDrive->collision = false;
	gameStateDrive->running = true;
	gameStateDrive->restart = false;
}

bool firstRun = true;

bool myGameLoop(ScreenBuff* screenBuff, byte buttonVals) {
	processInput(&gameStateDrive, buttonVals);

	switch (gameStateDrive.scene) {
	case 0: //Start Up Image
		if (gameStateDrive.scene != gameStateDrive.lastscene) {
			gameStateDrive.lastscene = gameStateDrive.scene;
		}
		if (firstRun) {
			showLogo(mygame_image,screenBuff);
			firstRun = false;
		} else {
			updateMinTime(2000);
			gameStateDrive.scene = 1;
		}
		break;
	case 1: //Intro Flag 
		if (gameStateDrive.scene != gameStateDrive.lastscene) {
			startUpGame(&gameStateDrive, screenBuff);
			gameStateDrive.lastscene = gameStateDrive.scene;
		}

		if (!drawWavingFlag(&gameStateDrive, screenBuff)) {
			gameStateDrive.scene = 2;
		}

		break;

	case 2: //Intro
		if (gameStateDrive.scene != gameStateDrive.lastscene) {
			startUpGame(&gameStateDrive, screenBuff);
			gameStateDrive.frameCounter = 0;
			gameStateDrive.lastscene = gameStateDrive.scene;
		}

		updateDriveScroller(&gameStateDrive, screenBuff);
		if (!displayDriveScroller(&gameStateDrive, screenBuff)) {
			gameStateDrive.scene = 4;
		}

		break;
	case 4: // Level Slider 
		if (gameStateDrive.scene != gameStateDrive.lastscene) {
			startUpGame(&gameStateDrive, screenBuff);
			gameStateDrive.lastscene = gameStateDrive.scene;
		}

		if (!displayLevelSlider(&gameStateDrive, screenBuff)) {
			gameStateDrive.scene = 3;
		}
		break;

	case 3: //Drive
		if (gameStateDrive.scene != gameStateDrive.lastscene) {
			// Play the car starting sound
			char sound[] = "data/carStart.wav";
			audioPlay(sound);
			
			startUpGame(&gameStateDrive, screenBuff);
			gameStateDrive.lastscene = gameStateDrive.scene;
		}

		// Gameplay until done
		if (!updateDrive(&gameStateDrive, screenBuff)) {
			if (gameStateDrive.win) {
				gameStateDrive.level++;
				gameStateDrive.scene = 5;
			}
			else {
				// Lost the game
				gameStateDrive.level = 1;
				startUpGame(&gameStateDrive, screenBuff);
				gameStateDrive.scene = 5;
			}
		}

		// Normal gameplay
		displayDrive(&gameStateDrive, screenBuff);

		break;
	case 5: // Display Win Lose
		if (gameStateDrive.scene != gameStateDrive.lastscene) {
			gameStateDrive.lastscene = gameStateDrive.scene;
			initTime();
		}

		gameStateDrive.frameCounter++;

		if (!displayWinLose(&gameStateDrive, screenBuff)) {
			if (gameStateDrive.win) {
				if (gameStateDrive.level < 3) {
					// Move to next stage
					gameStateDrive.scene = 4;
				}
				else {
					// Won the game
					gameStateDrive.scene = 6;
				}
			}
			else {
				// Lost the game
				// Reset to start
				gameStateDrive.scene = 0;
			}
		}
		break;
	case 6: //Outro 
		if (gameStateDrive.scene != gameStateDrive.lastscene) {
			startUpGame(&gameStateDrive, screenBuff);
			gameStateDrive.frameCounter = 0;
			gameStateDrive.lastscene = gameStateDrive.scene;
		}

		updateDriveScroller(&gameStateDrive, screenBuff);
		if (!displayDriveOutroScroller(&gameStateDrive, screenBuff)) {
			gameStateDrive.scene = 0;
			return true;
		}

		break;
	}

	return false;
}
