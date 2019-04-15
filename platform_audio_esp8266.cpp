#include "platform_audio_esp8266.h"

AudioGeneratorWAV *awav;
AudioFileSourceSPIFFS *afile;
AudioOutputI2SNoDAC *aout;

void audioInit()
{
  Serial.println("Audio init");
  aout = new AudioOutputI2SNoDAC();
  awav = new AudioGeneratorWAV();
}

void audioDestroy()
{
}

void audioLoop()
{
    if (awav->isRunning())
    {
        if (!awav->loop())
            awav->stop();
    }
}

bool audioPlay(char *filename)
{
    Serial.println("audioPlay");
    Serial.println(filename);
    if (awav->isRunning()) return false;
    
    char sfilename[15];
    strncpy(sfilename, filename+4, strlen(filename)-4);
    sfilename[strlen(filename)-4] = '\0';
    Serial.println(sfilename);
    afile = new AudioFileSourceSPIFFS(sfilename);
    awav->begin(afile, aout);
    return true;
}
