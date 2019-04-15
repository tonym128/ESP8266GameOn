#include <ncurses.h>

byte getReadShift() {
	byte buttonVals = 0;

	switch(getch()) {
	case 68:    // key left
		buttonVals = buttonVals | (1 << P1_Left);
		break;
	case 65:    // key up
		buttonVals = buttonVals | (1 << P1_Top);
		break;
	case 67:    // key right
		buttonVals = buttonVals | (1 << P1_Right);
		break;
	case 66:    // key down
		buttonVals = buttonVals | (1 << P1_Bottom);
		break;
	case 'd':
		buttonVals = buttonVals | (1 << P2_Right);
		break;
	case 's':
		buttonVals = buttonVals | (1 << P2_Bottom);
		break;
	case 'a':
		buttonVals = buttonVals | (1 << P2_Left);
		break;
	case 'w':
		buttonVals = buttonVals | (1 << P2_Top);
		break;
	case 'q':
		exit(0);
		break;
	}

	return buttonVals;
}

std::array<int,8> getRawInput() {
  std::array<int,8> rawValues;
  int i = 0;
	for (int i = 0; i < 8; i++) {
		rawValues[i] = 0;
	}
	
 	switch(getch()) {
	case 68:    // key left
		rawValues[0] = 1;
		break;
	case 65:    // key up
		rawValues[1] = 1;
		break;
	case 67:    // key right
		rawValues[2] = 1;
		break;
	case 66:    // key down
		rawValues[3] = 1;
		break;
	case 'd':
		rawValues[4] = 1;
		break;
	case 's':
		rawValues[5] = 1;
		break;
	case 'a':
		rawValues[6] = 1;
		break;
	case 'w':
		rawValues[7] = 1;
		break;
	}

	return rawValues;
}

void sendToScreen() {
  for (int i = 0; i < screenBuff.WIDTH * screenBuff.HEIGHT; i++) {
	  int x = i % screenBuff.WIDTH;
	  int y = i / screenBuff.WIDTH;
	  if (screenBuff.consoleBuffer[i]) {
		attron(COLOR_PAIR(3));
		mvaddch(y, x,' ');
		attroff(COLOR_PAIR(3));
	  } else {
		mvaddch(y, x,' ');
	  }
  }
}

void gameInit()
{
    setlocale(LC_ALL, "");
    initscr();
    start_color();

    init_pair(3, COLOR_YELLOW, COLOR_WHITE);

    curs_set(FALSE);
    raw();
    noecho();
    nodelay(stdscr, TRUE);
}

int main(int argc, char*argv[])
{
  audioSetup();
  gameInit();
  gameSetup();
  while (1)
  {
    gameLoop();
  }

  return 1;
}
