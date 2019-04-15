#include "gameCommon.hpp"

bool processKey(byte buttonVals, int key)
{
	return buttonVals & (1 << key);
}

void displayNoise(ScreenBuff *screenBuff, int amountInverse = 0)
{
	for (int y = 0; y < screenBuff->HEIGHT; ++y)
		for (int x = 0; x < screenBuff->WIDTH; ++x)
			if (amountInverse == 0 || x * y % amountInverse == 0)
			{
				screenBuff->consoleBuffer[x + screenBuff->WIDTH * y] = rand() % 2;
			}
}

bool drawScroller(ScreenBuff *screenBuff, int frameCounter, char scrollerText[9][17])
{
	displayClear(screenBuff, 1, 0);

	int y = screenBuff->HEIGHT - frameCounter + 10;

	int i = 0;
	for (int j = 0; j < 7; j++)
	{
		if (y > -8 || y < screenBuff->HEIGHT)
		{
			drawString(screenBuff, scrollerText[i++], 0, y, false);
			y += 8;
		}
	}

	drawString(screenBuff, scrollerText[i], 0, 0, false);
	drawString(screenBuff, scrollerText[i++], 0, 8, false);
	drawString(screenBuff, scrollerText[i++], 5, 2, true);

	if (y < -8)
		return false;
	return true;
}

void displayNoise(ScreenBuff *screenBuff, Dimensions dim, int amountInverse = 0)
{
	int counter = 0;
	for (int j = dim.y; j < dim.y + dim.height; j++)
	{
		int dimXmod = dim.x <= 0 ? 1 : dim.x;
		int firstLine = (dimXmod + screenBuff->WIDTH * j) / screenBuff->WIDTH;
		for (int i = dim.x; i < dim.x + dim.width; i++)
		{
			int pixel = i + screenBuff->WIDTH * j;
			if (pixel >= 0 && pixel < screenBuff->MAXPIXEL && firstLine == pixel / screenBuff->WIDTH)
			{
				screenBuff->consoleBuffer[pixel] = rand() % 2;
			}
			counter++;
		}
	}
}

FIXPOINT xVec(FIXPOINT speed, FIXPOINT direction) {

	if (direction == INT_TO_FIXP(90)) return speed;
	if (direction == INT_TO_FIXP(270)) return -speed;
	
	if (direction == INT_TO_FIXP(0) || direction == INT_TO_FIXP(180)) return 0;
	FIXPOINT sign = FIXP_1;

	if (direction < INT_TO_FIXP(90)) {
	}
	else if (direction < INT_TO_FIXP(180)) {
		direction = direction - INT_TO_FIXP(90);
		direction = INT_TO_FIXP(90) - direction;
	}
	else if (direction < INT_TO_FIXP(270)) {
		// X Value is -'ve
		sign = INT_TO_FIXP(-1);
		direction = direction - INT_TO_FIXP(180);
	}
	else {
		// X Value is -'ve
		direction = direction - INT_TO_FIXP(270);
		direction = INT_TO_FIXP(90) - direction;
		sign = INT_TO_FIXP(-1);
	}

	return FIXP_MULT(FIXP_MULT(speed,sign),FIXP_DIV(direction, INT_TO_FIXP(90)));
}

FIXPOINT yVec(FIXPOINT speed, FIXPOINT direction) {
	if (direction == INT_TO_FIXP(90) || direction == INT_TO_FIXP(270)) return 0;
	
	if (direction == INT_TO_FIXP(0)) return -speed; 
	if (direction == INT_TO_FIXP(180)) return speed;
	FIXPOINT sign = FIXP_1;

	if (direction < INT_TO_FIXP(90)) {
		direction = INT_TO_FIXP(90)-direction;
		sign = INT_TO_FIXP(-1);
	}
	else if (direction < INT_TO_FIXP(180)) {
		direction = direction - INT_TO_FIXP(90);
	}
	else if (direction < INT_TO_FIXP(270)) {
		direction = direction - INT_TO_FIXP(180);
		direction = INT_TO_FIXP(90)-direction;
	}
	else {
		direction = direction - INT_TO_FIXP(270);
		sign = INT_TO_FIXP(-1);
	}
	
	return FIXP_MULT(FIXP_MULT(speed,sign),FIXP_DIV(direction, INT_TO_FIXP(90)));
}

void rotateObject(Dimensions dim, double angle, double zoom, const bool *object, bool *rotated)
{
	int xt;
	int xs;

	int yt;
	int ys;

	FIXPOINT cosmax;
	FIXPOINT sinmax;

	#ifdef __EMSCRIPTEN__
	FIXPOINT sinma = FIXP_MULT(FIXPOINT_SIN(FLOAT_TO_FIXP(-angle)), FLOAT_TO_FIXP(zoom));
	FIXPOINT cosma = FIXP_MULT(FIXPOINT_COS(FLOAT_TO_FIXP(-angle)), FLOAT_TO_FIXP(zoom));
	#else
	FIXPOINT sinma = FLOAT_TO_FIXP(sin(-angle) * zoom);
	FIXPOINT cosma = FLOAT_TO_FIXP(cos(-angle) * zoom);
	#endif

	int hwidth = dim.width / 2;
	int hheight = dim.height / 2;

	for (int x = 0; x < dim.width; x++)
	{
		xt = x - hwidth;
		cosmax = cosma * xt;
		sinmax = sinma * xt;

		for (int y = 0; y < dim.height; y++)
		{
			yt = y - hheight;

			xs = FIXP_INT_PART((cosmax - (sinma * yt))) + hwidth;
			ys = FIXP_INT_PART((sinmax + (cosma * yt))) + hheight;

			if (xs >= 0 && xs < dim.width && ys >= 0 && ys < dim.height)
			{
				rotated[x + y * dim.width] = object[xs + ys * dim.width];
			}
			else
			{
				rotated[x + y * dim.width] = 0;
			}
		}
	}
}

bool rectCollisionCheck(Dimensions rect1, Dimensions rect2)
{
	if (rect1.x < rect2.x + rect2.width &&
		rect1.x + rect1.width > rect2.x &&
		rect1.y < rect2.y + rect2.height &&
		rect1.y + rect1.height > rect2.y)
	{
		return true;
	}
	return false;
}

bool maskCollisionCheck(Dimensions rect1, Dimensions rect2, const bool *object1, const bool *object2)
{
	for (int x1 = rect1.x; x1 < rect1.x + rect1.width; x1++)
		for (int x2 = rect2.x; x2 < rect2.x + rect2.width; x2++)
			if (x2 == x1)
				for (int y1 = rect1.y; y1 < rect1.y + rect1.height; y1++)
					for (int y2 = rect2.y; y2 < rect2.y + rect2.height; y2++)
						if (y2 == y1)
						{
							int y1Loc = (y1 - rect1.y) * rect1.width;
							int y2Loc = (y2 - rect2.y) * rect2.width;
							if (1 == object1[x1 - rect1.x + y1Loc] && object1[x1 - rect1.x + y1Loc] == object2[x2 - rect2.x + y2Loc])
							{
								return true;
							}
						}
	return false;
}

void displayClear(ScreenBuff *screenBuff, int frameMod, bool colour)
{
	for (int y = 0; y < screenBuff->HEIGHT; ++y)
		for (int x = 0; x < screenBuff->WIDTH; ++x)
			screenBuff->consoleBuffer[x + screenBuff->WIDTH * y] = colour;
}

void displayInvert(ScreenBuff *screenBuff)
{
	int pixel = 0;
	for (int y = 0; y < screenBuff->HEIGHT; ++y)
		for (int x = 0; x < screenBuff->WIDTH; ++x)
		{
			pixel = x + screenBuff->WIDTH * y;
			screenBuff->consoleBuffer[pixel] = !screenBuff->consoleBuffer[pixel];
		}
}

void drawObject(ScreenBuff *screenBuff, Dimensions dim, const bool *objectArray)
{
	int counter = 0;
	for (int j = dim.y; j < dim.y + dim.height; j++)
	{
		int dimXmod = dim.x <= 0 ? 1 : dim.x;
		int firstLine = (dimXmod + screenBuff->WIDTH * j) / screenBuff->WIDTH;
		for (int i = dim.x; i < dim.x + dim.width; i++)
		{
			int pixel = i + screenBuff->WIDTH * j;
			if (objectArray[counter] && pixel >= 0 && pixel < screenBuff->MAXPIXEL && firstLine == pixel / screenBuff->WIDTH)
			{
				screenBuff->consoleBuffer[pixel] = 1;
			}
			counter++;
		}
	}
}

void drawObjectWrap(ScreenBuff *screenBuff, Dimensions dim, const bool *objectArray)
{
	int counter = 0;
	for (int j = dim.y; j < dim.y + dim.height; j++)
	{
		for (int i = dim.x; i < dim.x + dim.width; i++)
		{
			int pixel = i%screenBuff->WIDTH + screenBuff->WIDTH * (j%screenBuff->HEIGHT);
			if (objectArray[counter]) screenBuff->consoleBuffer[pixel] = 1;
			counter++;
		}
	}
}

void drawObjectWavy(ScreenBuff *screenBuff, Dimensions dim, int ylow, int yhigh, int ystart, int frameCount, bool yup, const bool *objectArray)
{
	if (ystart > yhigh)
		ystart = yhigh;
	if (ystart < ylow)
		ystart = ylow;

	bool originalYup = yup;
	int internalCounter = frameCount;
	int counter = 0;
	int yOffset = ystart;
	for (int j = dim.y; j < dim.y + dim.height; j++)
	{
		yup = originalYup;
		yOffset = ystart;
		internalCounter = frameCount;

		for (int i = dim.x; i < dim.x + dim.width; i++)
		{
			int pixel = i + screenBuff->WIDTH * (j + yOffset);
			if (j + yOffset > 0 && j + yOffset < screenBuff->HEIGHT && i > 0 && i < screenBuff->WIDTH && objectArray[counter] && pixel >= 0 && pixel < screenBuff->MAXPIXEL)
			{
				screenBuff->consoleBuffer[pixel] = 1;
			}
			counter++;

			if (yOffset == yhigh || yOffset == ylow)
			{
				yup = yOffset == yhigh ? true : false;

				if (internalCounter == 4)
				{
					yOffset += yup ? -1 : +1;
					internalCounter = 0;
				}
				else
				{
					internalCounter += 1;
				}
			}
			else
			{
				if (internalCounter == 1)
				{
					yOffset += yup ? -1 : +1;
				}
				else
				{
					internalCounter += 1;
				}
			}
		}
	}
}

void drawObjectFill(ScreenBuff *screenBuff, Dimensions dim, const bool *objectArray, bool drawColor)
{
	int counter = 0;
	for (int j = dim.y; j < dim.y + dim.height; j++)
	{
		int dimXmod = dim.x <= 0 ? 1 : dim.x;
		int firstLine = (dimXmod + screenBuff->WIDTH * j) / screenBuff->WIDTH;
		for (int i = dim.x; i < dim.x + dim.width; i++)
		{
			int pixel = i + screenBuff->WIDTH * j;
			if (objectArray[counter] == drawColor && pixel >= 0 && pixel < screenBuff->MAXPIXEL && firstLine == pixel / screenBuff->WIDTH)
			{
				screenBuff->consoleBuffer[pixel] = drawColor;
			}
			counter++;
		}
	}
}

void drawObject(ScreenBuff *screenBuff, Dimensions dim, bool *objectArray, bool backFill)
{
	int counter = 0;
	for (int j = dim.y; j < dim.y + dim.height; j++)
	{
		int dimXmod = dim.x <= 0 ? 1 : dim.x;
		int firstLine = (dimXmod + screenBuff->WIDTH * j) / screenBuff->WIDTH;
		for (int i = dim.x; i < dim.x + dim.width; i++)
		{
			int pixel = i + screenBuff->WIDTH * j;
			if (objectArray[counter] && pixel >= 0 && pixel < screenBuff->MAXPIXEL && firstLine == pixel / screenBuff->WIDTH)
			{
				screenBuff->consoleBuffer[pixel] = 1;
			}
			else if (backFill)
			{
				screenBuff->consoleBuffer[pixel] = 0;
			}

			counter++;
		}
	}
}

void drawVertLine(ScreenBuff *screenBuff, int x, int y, int length, bool colour, int pattern)
{
	if (x < 0 || x > screenBuff->WIDTH) return;
	if (y < 0 || y > screenBuff->HEIGHT) return;
	int pixel = x + screenBuff->WIDTH * y;

	switch (pattern)
	{
	case 0:
		// Top and bottom pixels
		screenBuff->consoleBuffer[x + screenBuff->WIDTH * y] = colour;
		screenBuff->consoleBuffer[x + (y + length) * screenBuff->WIDTH] = colour;
		break;
	case 1:
		// All pixels
		while (pixel <= screenBuff->MAXPIXEL && pixel <= x + (y + length) * screenBuff->WIDTH)
		{
			screenBuff->consoleBuffer[pixel] = colour;
			pixel += screenBuff->WIDTH;
		}
		break;
	case 2:
		// Alternate lines
		while (pixel <= x + (y + length) * screenBuff->WIDTH)
		{
			screenBuff->consoleBuffer[pixel] = colour && pixel % 3 == 0;
			pixel += screenBuff->WIDTH;
		}
		break;
	case 3:
		// Horizontal lines
		while (pixel <= x + (y + length) * screenBuff->WIDTH)
		{
			screenBuff->consoleBuffer[pixel] = colour && pixel % 3 == 0;
			pixel += screenBuff->WIDTH;
		}
		break;
	case 4:
		// Moire pattern
		bool flip = x % 2;
		bool alt = (pixel % 2) && flip;

		while (pixel <= x + (y + length) * screenBuff->WIDTH)
		{
			screenBuff->consoleBuffer[pixel] = colour && alt;
			alt = !alt;
			pixel += screenBuff->WIDTH;
		}
		break;
	}
}

void drawBlock(ScreenBuff *screenBuff, Dimensions dim, bool colour)
{
	for (int j = dim.y; j <= dim.y + dim.height; j++)
	{
		int firstLine = (dim.x + screenBuff->WIDTH * j) / screenBuff->WIDTH;
		for (int i = dim.x; i <= dim.x + dim.width; i++)
		{
			int pixel = i + screenBuff->WIDTH * j;
			if (pixel >= 0 && pixel < screenBuff->MAXPIXEL && firstLine == pixel / screenBuff->WIDTH && j > 0 && j < screenBuff->HEIGHT && i > 0 && i < screenBuff->WIDTH)
			{
				screenBuff->consoleBuffer[pixel] = colour;
			}
		}
	}
}

void drawMoire(ScreenBuff *screenBuff, Dimensions dim, bool colour)
{
	bool pixelColour = colour;
	for (int j = dim.y; j <= dim.y + dim.height; j++)
	{
		int firstLine = (dim.x + screenBuff->WIDTH * j) / screenBuff->WIDTH;
		pixelColour = j % 2 == 0 ? colour : !colour;
		for (int i = dim.x; i <= dim.x + dim.width; i++)
		{
			int pixel = i + screenBuff->WIDTH * j;
			pixelColour = !pixelColour;
			if (pixel >= 0 && pixel < screenBuff->MAXPIXEL && firstLine == pixel / screenBuff->WIDTH && j > 0 && j < screenBuff->HEIGHT && i > 0 && i < screenBuff->WIDTH)
			{
				screenBuff->consoleBuffer[pixel] = pixelColour;
			}
		}
	}
}

void reverseObject(Dimensions dim, const bool *objectArray, bool *newObjectArray)
{
	int counter = 0;
	for (int j = 0; j < dim.height; j++)
	{
		for (int i = dim.width - 1; i >= 0; i--)
		{
			int pixel = i + dim.width * j;
			newObjectArray[counter] = objectArray[pixel];
			counter++;
		}
	}
}

void invertObject(Dimensions dim, const bool *objectArray, bool *newObjectArray)
{
	int counter = 0;
	for (int j = 0; j < dim.height; j++)
	{
		for (int i = dim.width - 1; i >= 0; i--)
		{
			newObjectArray[counter] = !objectArray[counter];
			counter++;
		}
	}
}

void flipObject(Dimensions dim, const bool *objectArray, bool *newObjectArray)
{
	int counter = 0;
	for (int j = dim.height - 1; j >= 0; j--)
	{
		for (int i = dim.width - 1; i >= 0; i--)
		{
			int pixel = i + dim.width * j;
			newObjectArray[counter] = objectArray[pixel];
			counter++;
		}
	}
}

void copyObject(Dimensions dim, const bool *objectArray, bool *newObjectArray)
{
	int counter = 0;
	for (int j = dim.height - 1; j >= 0; j--)
	{
		for (int i = dim.width - 1; i >= 0; i--)
		{
			newObjectArray[counter] = objectArray[counter];
			counter++;
		}
	}
}

void drawCharacter(ScreenBuff *screenBuff, char charPos, int x, int y, bool backFill)
{
	bool *character = font(charPos);
	Dimensions dim;
	dim.x = x;
	dim.y = y;
	dim.width = 8;
	dim.height = 8;

	drawObject(screenBuff, dim, character, backFill);
	free(character);
}

void drawString(ScreenBuff *screenBuff, char *scrollText, int x, int y, bool backFill)
{
	for (int i = 0; i < static_cast<int>(strlen(scrollText)); i++)
	{
		drawCharacter(screenBuff, scrollText[i], x + 8 * i, y, backFill);
	}
}

FIXPOINT fpsArray[10];
int fpsItem = 0;
int fpsItems = 0;
int fpsMaxItems = 10;

FIXPOINT getCurrentFPS() {
	return fpsArray[fpsItem];
}

double currentFPS()
{
	// Calculate Moving Average Time
	FIXPOINT averageFPS = 0;

	for (int i = 0; i < fpsItems; i++)
	{
		averageFPS += fpsArray[i];
	}

	if (fpsItems > 0)
		averageFPS /= fpsItems;

	return FIXP_TO_FLOAT(averageFPS);
}

void calcFPS()
{
	// Move pointer forwards
	fpsItem += 1;
	if (fpsItems < fpsMaxItems)
	{
		fpsItems += 1;
	}

	// If we have hit the max items loop around
	if (fpsItem == fpsMaxItems)
	{
		fpsItem = 0;
	}

	fpsTimer2 = fpsTimer1;
	fpsTimer1 = getTimeInMillis();

	// Calc Diff MS
	int diff = fpsTimer1 - fpsTimer2;
	if (diff > 0)
	{
		// Add Item to Array
		fpsArray[fpsItem] = INT_TO_FIXP(diff);
	}
}

void drawFPS(ScreenBuff *screenBuff)
{
	char fpsString[17];
	#ifdef ARDUINO
	sprintf(fpsString, "%3.2f %d", currentFPS(), ESP.getFreeHeap());
	#else
	sprintf(fpsString, "%3.2f ms", currentFPS());
	#endif
	drawString(screenBuff, fpsString, 0, screenBuff->HEIGHT - 8, true);
}

void setCurrentTime()
{
	frameTime = currentTime;
	currentTime = getTimeInMillis();
}

void initTime()
{
	setCurrentTime();
	startTime = currentTime;
	frameTime = currentTime;
}

void updateMinTime(unsigned int sleepMiliseconds)
{
	setCurrentTime();
	if (currentTime - frameTime < sleepMiliseconds)
	{
		gameSleep(sleepMiliseconds - (currentTime - frameTime));
	}

	frameTime = currentTime;
}

bool checkTime(unsigned int Seconds)
{
	return (currentTime - startTime > Seconds * 1000);
}

int getElapsedSeconds()
{
	return (int)(currentTime - startTime) / 1000;
}

void showLogo(const bool logo[],ScreenBuff *screenBuff)
{
  Dimensions dim;
  dim.height = logo_height;
  dim.width = logo_width;
  dim.x = 0;
  dim.y = 0;
  
  displayClear(screenBuff,0,false);
  drawObject(screenBuff, dim, logo);
  initTime();
}
