#ifndef PLATFORM_AUDIO_H
#define PLATFORM_AUDIO_H 1
#include <Arduino.h>
#include "AudioFileSourceSPIFFS.h"
#include "AudioGeneratorWAV.h"
#include "AudioOutputI2SNoDAC.h"

void audioInit();
void audioDestroy();
void audioLoop();
bool audioPlay(char *);

#endif // PLATFORM_AUDIO_H
