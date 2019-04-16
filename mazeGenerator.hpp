#ifndef MAZEGENERATOR_H
#define MAZEGENERATOR_H

#ifndef GAMECOMMON_H
#include "gameCommon.hpp"
#endif

class Maze
{
private:
	const static int WIDTH = 25;
	const static int HEIGHT = 25;
	int maze[WIDTH*HEIGHT];
	void carveMaze(int x, int y);

public:
	int startX = WIDTH-2;
	int startY = HEIGHT-1;
	int endX = 1;
	int endY = 0;

	void generateMaze();
	void copyMaze(int worldMap[25][25]);
};

#endif //MAZEGENERATOR_H
