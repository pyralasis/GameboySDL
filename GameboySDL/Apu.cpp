#include "Apu.hxx"

void Apu::initChannelOne()
{
    channelOne = SDL_OpenAudioDevice(NULL, 0, &channelOneSpecs, nullptr, SDL_AUDIO_ALLOW_CHANNELS_CHANGE);
}

void Apu::channelOneSweep()
{
    int sweepRegister = memory->readMemory(0xFF10);
    int pace = (sweepRegister >> 4) & 7;
    int direction = (sweepRegister >> 3) & 1;
    int step = sweepRegister & 7;
    channelOnePeriod = direction ? channelOnePeriod + (channelOnePeriod / step) : channelOnePeriod - (channelOnePeriod / step);
}

void Apu::updateChannelOnePeriod()
{
    //channelOnePeriod = ((memory->readMemory(0xFF14) & 7) << 8) | (memory->readMemory(0xFF13);
}
