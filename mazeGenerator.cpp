#include "mazeGenerator.hpp"
// Found the original C code for carving the maze here - https://github.com/joewing/maze/
// I hacked it to CPP, I think his CPP code is neater :)

/*  Carve the maze starting at x, y. */
void Maze::carveMaze(int x, int y) {

	int x1, y1;
	int x2, y2;
	int dx, dy;
	int dir, count;

	dir = rand() % 4;
	count = 0;
	while (count < 4) {
		dx = 0; dy = 0;
		switch (dir) {
		case 0:  dx = 1;  break;
		case 1:  dy = 1;  break;
		case 2:  dx = -1; break;
		default: dy = -1; break;
		}
		x1 = x + dx;
		y1 = y + dy;
		x2 = x1 + dx;
		y2 = y1 + dy;
		if (x2 > 0 && x2 < WIDTH && y2 > 0 && y2 < HEIGHT
			&& maze[y1 * WIDTH + x1] == 1 && maze[y2 * WIDTH + x2] == 1) {
			maze[y1 * WIDTH + x1] = 0;
			maze[y2 * WIDTH + x2] = 0;
			x = x2; y = y2;
			dir = rand() % 4;
			count = 0;
		}
		else {
			dir = (dir + 1) % 4;
			count += 1;
		}
	}

}

/* Generate maze in matrix maze with size width, height. */
void Maze::generateMaze() {

	int x, y;

	/* Initialize the maze. */
	for (x = 0; x < WIDTH * HEIGHT; x++) {
		maze[x] = 1;
	}
	maze[1 * WIDTH + 1] = 0;

	/* Seed the random number generator. */
	srand((unsigned int)time(0));

	/* Carve the maze. */
	for (y = 1; y < HEIGHT; y += 2) {
		for (x = 1; x < WIDTH; x += 2) {
			carveMaze(x, y);
		}
	}

	/* Set up the entry and exit. */
	maze[0 * WIDTH + 1] = 0;
	maze[(HEIGHT - 1) * WIDTH + (WIDTH - 2)] = 0;
}

void Maze::copyMaze(int worldMap[WIDTH][HEIGHT]) {
	int x, y;
	for (y = 0; y < HEIGHT; y++) {
		for (x = 0; x < WIDTH; x++) {
			worldMap[x][y] = maze[y * WIDTH + x];
		}
	}
}
