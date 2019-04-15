#ifndef _PLATFORM_AUDIO_H
#define _PLATFORM_AUDIO_H 1
#include <AL/al.h>
#include <AL/alc.h>
#include <stdio.h>
#include <iostream>
#define AUDIO_DEBUG_MESSAGES

ALuint source;
ALint state;
ALuint buffers[5];

inline void audioInit() {
    // Initialize the environment
    ALCdevice *device = alcOpenDevice(NULL);
    ALCcontext *context = alcCreateContext(device, NULL);
    alcMakeContextCurrent(context);

    int major, minor;
    alcGetIntegerv(NULL, ALC_MAJOR_VERSION, 1, &major);
    alcGetIntegerv(NULL, ALC_MINOR_VERSION, 1, &minor);

    //printf("ALC version: %i.%i\n", major, minor);
    //printf("Default device: %s\n", alcGetString(NULL, ALC_DEFAULT_DEVICE_SPECIFIER));
    //printf("OpenAL version: %s\n", alGetString(AL_VERSION));
    //printf("OpenAL vendor: %s\n", alGetString(AL_VENDOR));
    //printf("OpenAL renderer: %s\n", alGetString(AL_RENDERER));
        
    // Capture errors
    alGetError();
    // std::cout << "OpenAL Init" << "\r\n";
}

inline void audioDestroy() 
{
}

inline void audioLoop()
{
}

inline bool audioPlay(char* filename)
{
    alGetSourcei(source, AL_SOURCE_STATE, &state);
    if (state == AL_PLAYING) return false;

    // std::cout << "OpenAL Play : " << filename << "\r\n";

    // Create sound source (use buffer to fill source)
    FILE *fsource = fopen(filename, "rb");
    fseek(fsource, 0, SEEK_END);
    int size = ftell(fsource);
    fseek(fsource, 0, SEEK_SET);

    unsigned char *buffer = (unsigned char *)malloc(size);
    fread(buffer, size, 1, fsource);

    unsigned offset = 12; // ignore the RIFF header
    offset += 8;          // ignore the fmt header
    offset += 2;          // ignore the format type

    unsigned channels = buffer[offset + 1] << 8;
    channels |= buffer[offset];
    offset += 2;
    //printf("Channels: %u\n", channels);

    unsigned frequency = buffer[offset + 3] << 24;
    frequency |= buffer[offset + 2] << 16;
    frequency |= buffer[offset + 1] << 8;
    frequency |= buffer[offset];
    offset += 4;
    //printf("Frequency: %u\n", frequency);

    offset += 6; // ignore block size and bps

    unsigned bits = buffer[offset + 1] << 8;
    bits |= buffer[offset];
    offset += 2;
    //printf("Bits: %u\n", bits);

    ALenum format = 0;
    if (bits == 8)
    {
        if (channels == 1)
            format = AL_FORMAT_MONO8;
        else if (channels == 2)
            format = AL_FORMAT_STEREO8;
    }
    else if (bits == 16)
    {
        if (channels == 1)
            format = AL_FORMAT_MONO16;
        else if (channels == 2)
            format = AL_FORMAT_STEREO16;
    }

    offset += 8; // ignore the data chunk

    //printf("Start offset: %d\n", offset);

    alGenBuffers(1, buffers);
    alBufferData(buffers[0], format, &buffer[offset], size - offset, frequency);

    // Load pcm data into buffer
    alGenSources(1, &source);

    alSourcei(source, AL_BUFFER, buffers[0]);

    // Play
    alSourcePlay(source);

    return true;
}

#endif // _PLATFORM_AUDIO_H
