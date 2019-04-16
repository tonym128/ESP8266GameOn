#include "myGame.hpp"
// Tutorial and sample code, modified to my needs from here
// https://lodev.org/cgtutor/files/raycaster_flat.cpp
// Check the full tutorial here
// https://lodev.org/cgtutor/raycasting.html

struct Player1Keys {
	bool up = false;
	bool down = false;
	bool right = false;
	bool left = false;
	bool open = false;
};

#define mapWidth 25
#define mapHeight 25

struct GameStateMaze {
	int currentState = 0;
	int previousState = -1;
	int frameCounter = 0;
	int stageTime = 120;
	int endTimer = 0;
	int endSeconds = 3;
	Player1Keys p1keys;
	bool running = false;
	bool restart = false;
	bool win = false;
	bool exit = false;
	bool traversal[mapWidth][mapHeight];
	int winX = 0;
	int winY = 0;

} gameStateMaze;


int worldMap[mapWidth][mapHeight] =
{
  {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1},
  {1,0,0,0,0,0,2,2,2,2,2,0,0,0,0,3,0,3,0,3,0,0,0,1,1},
  {1,0,0,0,0,0,2,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,1,1},
  {1,0,0,0,0,0,2,0,0,0,2,0,0,0,0,3,0,0,0,3,0,0,0,1,1},
  {1,0,0,0,0,0,2,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,1,1},
  {1,0,0,0,0,0,2,2,0,2,2,0,0,0,0,3,0,3,0,3,0,0,0,1,1},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1},
  {1,0,0,0,0,0,0,0,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1},
  {1,0,0,0,0,0,0,0,6,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1},
  {1,4,4,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1},
  {1,4,0,4,0,0,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1},
  {1,4,0,0,0,0,5,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1},
  {1,4,0,4,0,0,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1},
  {1,4,0,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1},
  {1,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1},
  {1,4,4,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1},
  {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};

FIXPOINT posX = INT_TO_FIXP(22), posY = INT_TO_FIXP(12);  //x and y start position
FIXPOINT dirX = INT_TO_FIXP(-1), dirY = INT_TO_FIXP(0); //initial direction vector
FIXPOINT planeX = INT_TO_FIXP(0), planeY = FLOAT_TO_FIXP(0.66); //the 2d raycaster version of camera plane

void mazeRunnerInit() {
  posX = INT_TO_FIXP(22), posY = INT_TO_FIXP(12);  //x and y start position
  dirX = INT_TO_FIXP(-1), dirY = INT_TO_FIXP(0); //initial direction vector
  planeX = INT_TO_FIXP(0), planeY = FLOAT_TO_FIXP(0.66); //the 2d raycaster version of camera plane

	initTime();
	Maze maze = Maze();
	maze.generateMaze();
	maze.copyMaze(worldMap);
	worldMap[maze.endX][maze.endY] = 6;
	worldMap[maze.startX][maze.startY] = 2;

	gameStateMaze.winX = maze.endX;
	gameStateMaze.winY = maze.endY;

	// Setup start pos
	posX = FLOAT_TO_FIXP(maze.startX - 0.2);
	posY = FLOAT_TO_FIXP(maze.startY - 0.2);

  for (int i = 0; i < mapWidth; i++) {
    for (int j = 0; j < mapHeight; j++) {
      gameStateMaze.traversal[i][j] = 0;
    }
  }
}

bool updateScroller(GameStateMaze* gameState, ScreenBuff* screenBuff) {
	return true;
}

bool displayScroller(GameStateMaze* gameState, ScreenBuff* screenBuff) {
	gameState->frameCounter += 1;
	char scrollerText[9][17];
	strcpy(scrollerText[0],"You're here!");
	strcpy(scrollerText[1],"You're rested!");
	strcpy(scrollerText[2],"But u only have");
	strcpy(scrollerText[3],"2 minutes to get");
	strcpy(scrollerText[4],"to the keynote");
	strcpy(scrollerText[5],"");
	strcpy(scrollerText[6],"   Good Luck!  ");
	strcpy(scrollerText[7],"");
	strcpy(scrollerText[8]," -= mRunnr =-  ");

	return drawScroller(screenBuff, gameStateMaze.frameCounter, scrollerText);
}

bool displayWinScroller(GameStateMaze* gameState, ScreenBuff* screenBuff) {
	gameState->frameCounter += 1;
	char scrollerText[9][17];
	strcpy(scrollerText[0],"You did it!");
	strcpy(scrollerText[1],"");
	strcpy(scrollerText[2],"Grab a seat and");
	strcpy(scrollerText[3],"let these talks");
	strcpy(scrollerText[4],"blow your mind");
	strcpy(scrollerText[5],"     DefCon     ");
	strcpy(scrollerText[6],"  Accomplished  ");
	strcpy(scrollerText[7],"");
	strcpy(scrollerText[8]," -= WINNER =-  ");

	return drawScroller(screenBuff, gameStateMaze.frameCounter, scrollerText);
}

bool displayLoseScroller(GameStateMaze* gameState, ScreenBuff* screenBuff) {
	gameState->frameCounter += 1;
	char scrollerText[9][17];
	strcpy(scrollerText[0],"U didn't make it");
	strcpy(scrollerText[1],"");
	strcpy(scrollerText[2],"Make your way to");
	strcpy(scrollerText[3],"the next talk");
	strcpy(scrollerText[4],"it's a good 1");
	strcpy(scrollerText[5],"");
	strcpy(scrollerText[6]," ;-)     (-; ");
	strcpy(scrollerText[7],"");
	strcpy(scrollerText[8],"-= TRY AGAIN =-");

	return drawScroller(screenBuff, gameStateMaze.frameCounter, scrollerText);
}

bool displayLevelBars(GameStateMaze* gameState, ScreenBuff* screenBuff) {
	gameState->frameCounter += 1;
	displayClear(screenBuff, 1, false);
	char fps[16] = "Final Level";

	for (int i = 0; i < static_cast<int>(strlen(fps)); i++) {
		drawCharacter(screenBuff, fps[i], 16 + 8 * i, 10);
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

	return gameState->frameCounter > 120;
}

bool update(GameStateMaze* gameStateMaze) {
	// Check for win state // Win is in the top left
	if (!gameStateMaze->win) {
		gameStateMaze->win = posX - INT_TO_FIXP(gameStateMaze->winX) < FLOAT_TO_FIXP(1.5) && posY - INT_TO_FIXP(gameStateMaze->winY) < FLOAT_TO_FIXP(1.5);
	}
	else if (gameStateMaze->win) {
		if (gameStateMaze->endSeconds == 0) { gameStateMaze->endSeconds = getElapsedSeconds();}
		else if (getElapsedSeconds() - gameStateMaze->endSeconds > gameStateMaze->endTimer) {gameStateMaze->currentState = 3;}
	}
	
	if (checkTime(gameStateMaze->stageTime)) {
		// If we have run out of time, stop updating the game
		gameStateMaze->win = false;
		if (gameStateMaze->endSeconds == 0) {gameStateMaze->endSeconds = getElapsedSeconds();}
		else if (getElapsedSeconds() - gameStateMaze->endSeconds > gameStateMaze->endTimer) {gameStateMaze->currentState = 5;}
		return false;
	}

	gameStateMaze->traversal[FIXP_INT_PART(posX)][FIXP_INT_PART(posY)] = true;

	//speed modifiers
	FIXPOINT moveSpeed = FLOAT_TO_FIXP(0.1);
	FIXPOINT rotSpeed = FLOAT_TO_FIXP(0.05);

	//move forward if no wall in front of you
	if (gameStateMaze->p1keys.up)
	{
		if (worldMap[FIXP_INT_PART(posX + FIXP_MULT(dirX, moveSpeed))][FIXP_TO_INT(posY)] == false) posX += FIXP_MULT(dirX, moveSpeed);
		if (worldMap[FIXP_INT_PART(posX)][FIXP_INT_PART(posY + FIXP_MULT(dirY, moveSpeed))] == false) posY += FIXP_MULT(dirY, moveSpeed);
		char sound[] = "data/walking.wav";
		audioPlay(sound);
	}
	//move backwards if no wall behind you
	if (gameStateMaze->p1keys.down)
	{
		if (worldMap[FIXP_INT_PART(posX - FIXP_MULT(dirX, moveSpeed))][FIXP_INT_PART(posY)] == false) posX -= FIXP_MULT(dirX, moveSpeed);
		if (worldMap[FIXP_INT_PART(posX)][FIXP_INT_PART(posY - FIXP_MULT(dirY, moveSpeed))] == false) posY -= FIXP_MULT(dirY, moveSpeed);
		char sound[] = "data/walking.wav";
		audioPlay(sound);
	}
	//rotate to the right
	if (gameStateMaze->p1keys.right)
	{
		//both camera direction and camera plane must be rotated
		FIXPOINT oldDirX = dirX;
		dirX = FIXP_MULT(dirX, FIXPOINT_COS(-rotSpeed)) - FIXP_MULT(dirY, FIXPOINT_SIN(-rotSpeed));
		dirY = FIXP_MULT(oldDirX, FIXPOINT_SIN(-rotSpeed)) + FIXP_MULT(dirY, FIXPOINT_COS(-rotSpeed));
		FIXPOINT oldPlaneX = planeX;
		planeX = FIXP_MULT(planeX, FIXPOINT_COS(-rotSpeed)) - FIXP_MULT(planeY, FIXPOINT_SIN(-rotSpeed));
		planeY = FIXP_MULT(oldPlaneX, FIXPOINT_SIN(-rotSpeed)) + FIXP_MULT(planeY, FIXPOINT_COS(-rotSpeed));
	}
	//rotate to the left
	if (gameStateMaze->p1keys.left)
	{
		//both camera direction and camera plane must be rotated
		FIXPOINT oldDirX = dirX;
		dirX = FIXP_MULT(dirX, FIXPOINT_COS(rotSpeed)) - FIXP_MULT(dirY, FIXPOINT_SIN(rotSpeed));
		dirY = FIXP_MULT(oldDirX, FIXPOINT_SIN(rotSpeed)) + FIXP_MULT(dirY, FIXPOINT_COS(rotSpeed));
		FIXPOINT oldPlaneX = planeX;
		planeX = FIXP_MULT(planeX, FIXPOINT_COS(rotSpeed)) - FIXP_MULT(planeY, FIXPOINT_SIN(rotSpeed));
		planeY = FIXP_MULT(oldPlaneX, FIXPOINT_SIN(rotSpeed)) + FIXP_MULT(planeY, FIXPOINT_COS(rotSpeed));
	}

	return true;
}

void processInput(GameStateMaze* gameStateMaze, byte buttonVals) {
	gameStateMaze->p1keys.up = processKey(buttonVals, P1_Top);
	gameStateMaze->p1keys.down = processKey(buttonVals, P1_Bottom);
	gameStateMaze->p1keys.left = processKey(buttonVals, P1_Left);
	gameStateMaze->p1keys.right = processKey(buttonVals, P1_Right);
	gameStateMaze->p1keys.open = processKey(buttonVals, P2_Left);

	if (processKey(buttonVals, P2_Top)) {
		gameStateMaze->running = false;
		gameStateMaze->restart = true;
	}
}

void display(ScreenBuff* screenBuff, GameStateMaze* gameStateMaze) {
	displayClear(screenBuff, 1, false);
	
	Dimensions floor;
	floor.x = 0;
	floor.y = screenBuff->HEIGHT/2;
	floor.height = screenBuff->HEIGHT-floor.y;
	floor.width = screenBuff->WIDTH;

	drawBlock(screenBuff, floor, true);

	for (int x = 0; x < screenBuff->WIDTH; x++)
	{
		//which box of the map we're in
		FIXPOINT fmapX = FIXP_FIXP_INT_PART(posX);
		FIXPOINT fmapY = FIXP_FIXP_INT_PART(posY); 

		//calculate ray position and direction
		FIXPOINT fcameraX = FIXP_DIV(INT_TO_FIXP(2 * x), INT_TO_FIXP(screenBuff->WIDTH)) - FIXP_1; //x-coordinate in camera space
		FIXPOINT frayDirX = dirX + FIXP_MULT(planeX, fcameraX);
		FIXPOINT frayDirY = dirY + FIXP_MULT(planeY, fcameraX);

		//length of ray from current position to next x or y-side
		FIXPOINT fsideDistX;
		FIXPOINT fsideDistY;

		//length of ray from one x or y-side to next x or y-side
		FIXPOINT fdeltaDistX = frayDirX == 0 ? INT32_MAX : FIXP_DIV(FIXP_1, frayDirX);
		FIXPOINT fdeltaDistY = frayDirY == 0 ? INT32_MAX : FIXP_DIV(FIXP_1, frayDirY);

		if (fdeltaDistX < 0) fdeltaDistX = -fdeltaDistX;
		if (fdeltaDistY < 0) fdeltaDistY = -fdeltaDistY;

		FIXPOINT fperpWallDist;

		//what direction to step in x or y-direction (either +1 or -1)
		int stepX;
		int stepY;

		int hit = 0; //was there a wall hit?
		int side; //was a NS or a EW wall hit?

	   //calculate step and initial sideDist
		if (frayDirX < 0)
		{
			stepX = -1;
			fsideDistX = FIXP_MULT((posX - fmapX), fdeltaDistX);
		}
		else
		{
			stepX = 1;
			fsideDistX = FIXP_MULT((fmapX + FIXP_1 - posX), fdeltaDistX);
		}
		if (frayDirY < 0)
		{
			stepY = -1;
			fsideDistY = FIXP_MULT((posY - fmapY), fdeltaDistY);
		}
		else
		{
			stepY = 1;
			fsideDistY = FIXP_MULT((fmapY + FIXP_1 - posY), fdeltaDistY);
		}

		//perform DDA
		while (hit == 0)
		{
			//jump to next map square, OR in x-direction, OR in y-direction
			if (fsideDistX < fsideDistY)
			{
				fsideDistX += fdeltaDistX;
				fmapX += INT_TO_FIXP(stepX);
				side = 0;
			}
			else
			{
				fsideDistY += fdeltaDistY;
				fmapY += INT_TO_FIXP(stepY);
				side = 1;
			}
			//Check if ray has hit a wall
			if (worldMap[FIXP_TO_INT(fmapX)][FIXP_TO_INT(fmapY)] > 0) hit = 1;
		}

		//Calculate distance projected on camera direction (Euclidean distance will give fisheye effect!)
		if (side == 0) fperpWallDist = frayDirX == 0 ? INT32_MAX : FIXP_DIV(fmapX - posX + FIXP_DIV(INT_TO_FIXP(1 - stepX) , INT_TO_FIXP(2)), frayDirX);
		else           fperpWallDist = frayDirY == 0 ? INT32_MAX : FIXP_DIV(fmapY - posY + FIXP_DIV(INT_TO_FIXP(1 - stepY) , INT_TO_FIXP(2)), frayDirY);

		//Calculate height of line to draw on screen
		int lineHeight = fperpWallDist == 0 ? INT32_MAX : FIXP_TO_INT(FIXP_DIV(INT_TO_FIXP(screenBuff->HEIGHT) , fperpWallDist));

		//calculate lowest and highest pixel to fill in current stripe
		int drawStart = -lineHeight / 2 + screenBuff->HEIGHT / 2;
		if (drawStart < 0)drawStart = 0;
		int drawEnd = lineHeight / 2 + screenBuff->HEIGHT / 2;
		if (drawEnd >= screenBuff->HEIGHT) drawEnd = screenBuff->HEIGHT - 1;

		//give x and y sides different brightness
		// if (side == 1) {color = color / 2;}
		int pattern = worldMap[FIXP_TO_INT(fmapX)][FIXP_TO_INT(fmapY)] - 1;

		//draw the pixels of the stripe as a vertical line
		if (pattern < 5) {
			// drawVertLine(screenBuff, x, drawStart, drawEnd - drawStart, true, pattern);
			FIXPOINT fwallX; //where exactly the wall was hit

			if (side == 0) {
				fwallX = (FIXP_MULT(fperpWallDist, frayDirY)) + posY;
			}
			else {
				fwallX = (FIXP_MULT(fperpWallDist, frayDirX)) + posX;
			}   

			fwallX = FIXP_DEC_PART((fwallX));

			//x coordinate on the texture
			int ftexX = FIXP_TO_INT(fwallX * wall_width);
			if (side == 0 && frayDirX > 0) ftexX = wall_width - 1 - ftexX;
			if (side == 1 && frayDirY < 0) ftexX = wall_width - 1 - ftexX;

			for (int y = drawStart; y < drawEnd; y++)
			{
				int d = y * 256 - wall_height * 128 + lineHeight * 128;  //256 and 128 factors to avoid floats
				// TODO: avoid the division to speed this up
				int texY = ((d * wall_width) / lineHeight) / 256;
				bool color = wall_image[ftexX + wall_height * texY];
				screenBuff->consoleBuffer[x + y * screenBuff->WIDTH] = color;
			}

		}
		else {
			FIXPOINT fwallX; //where exactly the wall was hit

			if (side == 0) {
				fwallX = (FIXP_MULT(fperpWallDist, frayDirY)) + posY;
			}
			else {
				fwallX = (FIXP_MULT(fperpWallDist, frayDirX)) + posX;
			}   

			fwallX = FIXP_DEC_PART((fwallX));

			//x coordinate on the texture
			int ftexX = FIXP_TO_INT(fwallX * defcon_width);
			if (side == 0 && frayDirX > 0) ftexX = defcon_width - 1 - ftexX;
			if (side == 1 && frayDirY < 0) ftexX = defcon_width - 1 - ftexX;

			for (int y = drawStart; y < drawEnd; y++)
			{
				int d = y * 256 - defcon_height * 128 + lineHeight * 128;  //256 and 128 factors to avoid floats
				// TODO: avoid the division to speed this up
				int texY = ((d * defcon_width) / lineHeight) / 256;
				bool color = defcon_image[ftexX + defcon_height * texY];
				screenBuff->consoleBuffer[x + y * screenBuff->WIDTH] = color;
			}
		}
	}

	// Draw Progress bar
	Dimensions dim;
	int seconds = getElapsedSeconds();
	int percTimeLeft = 128 - seconds * 128/ gameStateMaze->stageTime;
	dim.x = 0;
	dim.y = 0;
	dim.height = 5;
	dim.width = percTimeLeft;
	drawBlock(screenBuff, dim, 1);

	// Draw Win!
	if (gameStateMaze->win) {
		char fps[30];
		//sprintf(fps, "YOU WIN!");
		for (int i = 0; i < static_cast<int>(strlen(fps)); i++) {
			drawCharacter(screenBuff, fps[i], 32 + 8 * i, 30);
		}
	} else if (getElapsedSeconds() > gameStateMaze->stageTime) {
		char fps[30];
		//sprintf(fps, "GAME OVER!");
		for (int i = 0; i < static_cast<int>(strlen(fps)); i++) {
			drawCharacter(screenBuff, fps[i], 24 + 8 * i, 30);
		}
	}

	//Draw map
	dim.height = mapHeight;
	dim.width = mapWidth;
	dim.x = screenBuff->WIDTH - mapWidth - 1;
	dim.y = screenBuff->HEIGHT - mapHeight - 1;
	bool traverseMap[mapWidth * mapHeight];
	for (int i = 0; i < mapWidth; i++) {
		for (int j = 0; j < mapHeight; j++) {
			traverseMap[i + j * mapWidth] = gameStateMaze->traversal[i][j];
		}
	}

	//which box of the map we're in
	int mapX = int(posX);
	int mapY = int(posY);

	for (int i = -1; i < 2; i++) {
		for (int j = -1; j < 2; j++) {
			if (((mapX + i) > 0) && ((mapX + i) < mapWidth)
				&& ((mapY + j) > 0) && ((mapY + j) < mapHeight)
				)
				traverseMap[(mapX + i) + (mapY + j) * mapWidth] = 1;

		}
	}

	drawObject(screenBuff, dim, traverseMap);
}

bool myGameLoop(ScreenBuff* screenBuff, byte buttonVals) {
	switch (gameStateMaze.currentState) {
		case 0:
		// Start Story scroller
		if (gameStateMaze.previousState != gameStateMaze.currentState) {
			mazeRunnerInit();
			gameStateMaze.previousState = gameStateMaze.currentState;
			gameStateMaze.frameCounter = 0;
		}

		updateScroller(&gameStateMaze, screenBuff);
		if (!displayScroller(&gameStateMaze, screenBuff)) {
			gameStateMaze.currentState = 1;
		}
		break;
		case 1:
			// Level bars
			if (gameStateMaze.previousState != gameStateMaze.currentState) {
				gameStateMaze.previousState = gameStateMaze.currentState;
				gameStateMaze.frameCounter = 0;
			}

			updateScroller(&gameStateMaze, screenBuff);
			if (displayLevelBars(&gameStateMaze, screenBuff)) {
				gameStateMaze.currentState = 2;
			}
		break;
		case 2:
			// Play
			if (gameStateMaze.previousState != gameStateMaze.currentState) {
				gameStateMaze.previousState = gameStateMaze.currentState;
				gameStateMaze.frameCounter = 0;
				initTime();
			}

			processInput(&gameStateMaze, buttonVals);
			update(&gameStateMaze);
			display(screenBuff, &gameStateMaze);
		break;
		case 3:
			// Win Story Scroller
			if (gameStateMaze.previousState != gameStateMaze.currentState) {
				gameStateMaze.previousState = gameStateMaze.currentState;
				gameStateMaze.frameCounter = 0;
			}

			updateScroller(&gameStateMaze, screenBuff);
			if (!displayWinScroller(&gameStateMaze, screenBuff)) {
				gameStateMaze.currentState = 4;
			}
		break;
		case 5:
			// Lose Story Scroller
			if (gameStateMaze.previousState != gameStateMaze.currentState) {
				gameStateMaze.previousState = gameStateMaze.currentState;
				gameStateMaze.frameCounter = 0;
			}

			updateScroller(&gameStateMaze, screenBuff);
			if (!displayLoseScroller(&gameStateMaze, screenBuff)) {
				gameStateMaze.currentState = 1;
	      mazeRunnerInit();
    	}
		break;
		case 4:
			// Done and Exit
			gameStateMaze.exit = true;
		break;
	}

	return gameStateMaze.exit;
}
