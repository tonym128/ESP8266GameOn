# ESP8266GameOn
Game for an ESP8266 with SSD1306 and buttons.

Hardware was originally inspired by the BSides Cape Town 2016 Badge and original source code is here 

https://github.com/AndrewMohawk/BSidesBadge2016/branches

For Arduino, I was using SSD1306 Brzo as well as ESP8266AUDIO and ESP8266SAM libraries and I think that's it. You will need to install the two audio libraries manually unfortunately. They can be located here https://github.com/earlephilhower/ESP8266Audio and 
https://github.com/earlephilhower/ESP8266SAM

For linux build you need cmake, gcc and either SDL or ncurses-dev

SDL - 'sudo apt-get install libsdl2-devel'
NCurses - Most systems can install this with a 'sudo apt-get install ncurses-dev'

For Windows console build you will need cmake as well as Visual Studio and optionally Visual Studio Code.
Console - Should require nothing additional.
SDL - I found https://github.com/Microsoft/vcpkg invaluable, the example install is even for SDL2.

For emscipten, there is only a build command currently, but if there's more demand I can clean this up.
```emcc game.cpp gameCommon.cpp myGame.cpp myfont.cpp platform_core.cpp -s WASM=1 -std=c++0x -DOPENAL_FOUND -s USE_SDL=2 -O3 --profiling -o index.html -s ```

There's no tests, tut - tut, but I did a lot of play testing :)
