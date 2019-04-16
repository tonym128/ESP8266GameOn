#include "myGame.hpp"

int FrameCounter = 0;
Dimensions imageDim;
Dimensions imageDimWinner;
bool direction = false;
const int zoomFactor = 180;
double zoomWinner = 0.8;
bool zoomWinnerDirection = false;
double PI_180 = PI / 180;
bool firstFrame = true;

bool rotatedLogo[defcon_height * defcon_width];
bool rotatedLogo2[defconWinner_height * defconWinner_width];

void myGameInit()
{
	imageDim.width = defcon_width;
	imageDim.height = defcon_height;

	imageDimWinner.width = defconWinner_width;
	imageDimWinner.height = defconWinner_height;

	imageDim.y = 0;
	imageDim.x = 32;
}

bool myGameLoop(ScreenBuff *screenBuff, byte buttonVals)
{
	if (firstFrame) {
		firstFrame = false;
		myGameInit();
	}

	double zoom = (FrameCounter % zoomFactor) / (double)zoomFactor;
	if (FrameCounter % zoomFactor == 0)
		direction = !direction;

	if (direction)
		zoom = (zoomFactor / double(zoomFactor)) - zoom;

	if (zoomWinner > 1.1 || zoomWinner < 0.6)
	{
		zoomWinnerDirection = !zoomWinnerDirection;
	}

	zoomWinner += zoomWinnerDirection ? 0.012 : -0.012;

	rotateObject(imageDim, FrameCounter * PI_180, zoom, defcon_image, rotatedLogo);
	displayClear(screenBuff, 1, false);
	drawObject(screenBuff, imageDim, (const bool *)rotatedLogo);
	imageDimWinner.x = 0;
	rotateObject(imageDimWinner, PI_180, zoomWinner, defconWinner_image, rotatedLogo2);
	drawObject(screenBuff, imageDimWinner, rotatedLogo2);
	imageDimWinner.x = 96;
	flipObject(imageDimWinner, rotatedLogo2, rotatedLogo);
	drawObject(screenBuff, imageDimWinner, rotatedLogo);

	FrameCounter++;
	return false;
}
