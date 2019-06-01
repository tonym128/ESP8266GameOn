# ESP8266GameOn
Gaming on an ESP8266 with SSD1306 and buttons.

See the blog post about it here - https://ttech.mamacos.media/2019/04/21/building-your-own-game-console/

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

# Hardware I've used.
I have used some generic ESP8266-12E's but my most recent device is a Wesmos R1 Mini - https://wiki.wemos.cc/products:d1:d1_mini

With a SSD1306 screen, 128x64 Mono single colour 4pin - https://www.banggood.com/0_96-Inch-4Pin-White-IIC-I2C-OLED-Display-Module-12864-LED-For-Arduino-p-958196.html?rmmds=search&cur_warehouse=CN

And for the input I've used staples, with a 1 Mohm (Million) Resistor between the Output Pin to a staple, another staple across from it with a diode, to a Analog Input rail, which switched between in the input code (To Be Detailed Later)

There is the ability to run audio as well, which I will hopefully complete one day soon on my prototype badge, but the code has been partially tested on a few projects now, as per the instructions on the ESP8266Audio repo.
