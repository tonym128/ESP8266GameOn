# ESP8266GameOn
Gaming on an ESP8266 with SSD1306 and buttons.

Hardware was originally inspired by the BSides Cape Town 2016 Badge and original source code is here 

https://github.com/AndrewMohawk/BSidesBadge2016/branches

For Arduino, I was using SSD1306 Brzo as well as ESP8266AUDIO and ESP8266SAM libraries and I think that's it. You will need to install the two audio libraries manually unfortunately. They can be located here https://github.com/earlephilhower/ESP8266Audio and 
https://github.com/earlephilhower/ESP8266SAM . The audio libraries reccomend running at 160mhz, so I bumped it up to there, but depending on how complicated your code is, you could get away with less.

For linux build you need cmake, gcc and either SDL or ncurses-dev

SDL - 'sudo apt-get install libsdl2-devel'
NCurses - Most systems can install this with a 'sudo apt-get install ncurses-dev'

For Windows console build you will need cmake as well as Visual Studio and optionally Visual Studio Code.
Console - Should require nothing additional.

SDL - I found https://github.com/Microsoft/vcpkg invaluable, the example install is even for SDL2.

For emscipten, there is only a build command currently, but if there's more demand I can clean this up.
```emcc game.cpp gameCommon.cpp myGame.cpp myfont.cpp platform_core.cpp -s WASM=1 -std=c++0x -DOPENAL_FOUND -s USE_SDL=2 -O3 --profiling -o index.html -s ```

There's no tests, tut - tut, but I did a lot of play testing :)

The hope is that people can fork this repo and start writing their own games. You can see the Asteroids demo on the master branch or check out some other branches for other games I maybe write / have written.

You can change the mygame.cpp and mygame.h, to write your own game inside the 'framework'

I imagined turning it into a library, but I haven't thought of a clean way to do that and still maintain the cross platform build files, which I hope I can one day turn into and online IDE for creating and playing the games, with the ability to use backing hardware to play it on the a physical device you made.
