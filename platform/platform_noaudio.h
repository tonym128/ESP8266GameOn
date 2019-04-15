#ifndef PLATFORM_NOAUDIO_H
#define PLATFORM_NOAUDIO_H 1

#ifndef ARDUINO
#include <stdio.h>
#include <iostream>
#endif

inline void audioInit() {}
inline void audioDestroy() {}
inline void audioLoop() {}
inline bool audioPlay(char *fileName) {
#ifndef ARDUINO
    std::cout << "Playing " << fileName << "\r\n";
#endif
    return true;
    }
#endif // PLATFORM_NOAUDIO_H
