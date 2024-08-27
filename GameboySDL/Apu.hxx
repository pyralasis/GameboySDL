#pragma once
#include <SDL.h>
#include "Memory.hxx"

class Apu
{
private:
    Memory* memory = nullptr;
    SDL_AudioDeviceID channelOne;
    SDL_AudioSpec channelOneSpecs = {};

    int channelOneDutyCycles[4][8] =
    {
        {1, 1, 1, 1, 1, 1, 1 ,0},
        {0, 1, 1, 1, 1, 1, 1 ,0},
        {0, 1, 1, 1, 1, 0, 0 ,0},
        {1, 0, 0, 0, 0, 0, 0, 1}
    };

    int channelOnePeriod;
    bool channelOneTrigger;

public:
    void initChannelOne();
    void channelOneSweep();
    void updateChannelOnePeriod();
};
