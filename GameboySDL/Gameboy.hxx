#pragma once

#include "Cpu.hxx"
#include "Ppu.hxx"
#include "Memory.hxx"
#include "Controls.hxx"
#include <fstream>

class Gameboy
{
private:
    Cpu *cpu = new Cpu();
    Ppu *ppu = new Ppu();
    Memory *memory = new Memory();
    Controls* controls = new Controls;

    char *debugStringFull = new char[0];
    int debugStringLength = 0;
    int *debugBackgroundMap = new int[65536];
    int *debugTileMap = new int[24576];

    bool romLoaded = false;

    std::ofstream myfile;

public:
    Gameboy();
    ~Gameboy();

    void readRom(unsigned char* arrayBuffer, int size);
    void initialize();
    void mainLoop();


    std::vector<int> getBackground();
    std::vector<int> getTileData();
    void setBackgroundSettings(unsigned_four_byte backgroundAddress, unsigned_four_byte tilemapAddress);

    void getNextInstructionDebugLine();

    bool isRomLoaded();

    Ppu* getPpu();
    Controls* getControls();
};