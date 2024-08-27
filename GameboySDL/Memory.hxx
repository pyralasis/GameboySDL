#pragma once

#include "Types.hxx"
#include "MemoryBank.hxx"
#include "Cpu.hxx"

class Cpu;

class Memory
{
private:
    Cpu *cpu = nullptr;

    int mbcType;
    int numRomBanks;
    int numRamBanks;
    int currentRomBank = 1;
    int currentRamBank = 0;
    bool ramEnabled = false;
    bool bankMode = false;

    MemoryBank *rom = nullptr;

    /** 0x8000-0x9FFF */
    MemoryBank *vram = new MemoryBank(1, 8192);

    /** 0xA000-0xBFFF */
    MemoryBank *ram = nullptr;

    /** 0xC000-0xDFFF */
    MemoryBank *wram = new MemoryBank(1, 8192);

    /** 0xE000-0xFDFF
     * ECHO RAM SO IGNORE IT
     */
    MemoryBank *echoram = new MemoryBank(1, 7680);

    /** 0xFE00-0xFE9F */
    MemoryBank *oam = new MemoryBank(1, 160);

    /** 0xFEA0-0xFEFF
     * NOT USABLE SO IGNORE IT
     */
    MemoryBank *prohibited = new MemoryBank(1, 96);

    /** 0xFF00-0xFF7F */
    MemoryBank *io = new MemoryBank(1, 128);

    /** 0xFF80-0xFFFE */
    MemoryBank *hram = new MemoryBank(1, 127);

    /** 0xFFFF */
    MemoryBank *ie = new MemoryBank(1, 1);

public:
    Memory(/* args */);
    ~Memory();

    void initialize(unsigned_two_byte *romInput, int romLength);

    unsigned_two_byte readMemory(unsigned_four_byte address);
    void writeMemory(unsigned_four_byte address, unsigned_two_byte value);

    void writeMemoryMbcZero(unsigned_four_byte address, unsigned_two_byte value);
    void writeMemoryMbcOne(unsigned_four_byte address, unsigned_two_byte value);

    void setCpu(Cpu* input);

    void setDiv(unsigned_two_byte div);
};