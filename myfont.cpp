#include "myfont.hpp"
using namespace std;

bool* font(int x) {
	int line = x / 16 * 8;
	int col = x % 16 * 8;

	int width = 128;

	bool* character = new bool[64];
	int counter = 0;

	// Space
	if (x == 99) {
		for (int i = 0; i < 64; i++) character[i] = false;
	}
	// Everything Else
	else {
		for (int j = line; j < line + 8; j++) {
			for (int i = col; i < col + 8; i++) {
				int pixel = i + j * width;
				character[counter++] = fontData[pixel];
			}
		}
	}

	return character;
}

bool* font(char x) {
	map<char, int> mapObject;
	int i;

	// Incomplete mapping (Still have non alphanumeric characters to map.
	mapObject.insert(pair<char, int>(' ', 99));
	mapObject.insert(pair<char, int>('!', 0));
	mapObject.insert(pair<char, int>('"', 1));
	mapObject.insert(pair<char, int>('#', 2));
	mapObject.insert(pair<char, int>('$', 3));
	mapObject.insert(pair<char, int>('%', 4));
	mapObject.insert(pair<char, int>('&', 5));
	mapObject.insert(pair<char, int>('\'', 6));
	mapObject.insert(pair<char, int>('(', 7));
	mapObject.insert(pair<char, int>(')', 8));
	mapObject.insert(pair<char, int>('*', 9));
	mapObject.insert(pair<char, int>('+', 10));
	mapObject.insert(pair<char, int>(',', 11));
	mapObject.insert(pair<char, int>('-', 12));
	mapObject.insert(pair<char, int>('.', 13));
	mapObject.insert(pair<char, int>('/', 14));
	mapObject.insert(pair<char, int>('<', 27));
	mapObject.insert(pair<char, int>('=', 28));
	mapObject.insert(pair<char, int>('>', 28));

	for (i = 0; i < 26; i++) {
		mapObject.insert(pair<char, int>('A' + i, 32 + i));
	}

	for (i = 0; i < 26; i++) {
		mapObject.insert(pair<char, int>('a' + i, 65 + i));
	}

	for (i = 0; i < 10; i++) {
		mapObject.insert(pair<char, int>('0' + i, 15 + i));
	}

	map<char, int>::iterator p;

	p = mapObject.find(x);
	if (p != mapObject.end())
		return font(p->second);

	return font('!');
}