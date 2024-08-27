#pragma once

#include "RegisterCollection.hxx"
#include "Ppu.hxx"
#include "Memory.hxx"
#include "Types.hxx"

class Memory;

class Cpu
{
private:
    RegisterCollection registers;
    Memory *memory = nullptr;
    Ppu *ppu = nullptr;

    unsigned_four_byte programCounter = 0x0100;
    unsigned_four_byte stackPointer = 0xFFFE;
    unsigned_four_byte sysClock = 0xAB00;
    bool halt = false;
    bool ime = 0;

public:
    Cpu(/* args */);
    ~Cpu();

    unsigned_two_byte fetch();
    void execute();
    void interrupt();
    void interruptJump(unsigned_four_byte location, int flag);
    void tickClock(int cycles);
    
    void setMemory(Memory* value);
    void setPpu(Ppu* value);

    RegisterCollection* getRegisters();
    unsigned_four_byte getStackPointer();
    unsigned_four_byte getProgramCounter();

    void jumpConditional(bool condition);
    void jumpRelativeConditional(bool condition);

    void returnConditional(bool condition);
    void callConditional(bool condition);

    void pop(int register1, int register2);
    void push(int register1, int register2);

    void ldXY(int registerX, int registerY);

    void rst(unsigned_four_byte location);

    void bit(int bit, int target);
    void res(int bit, int target);
    void set(int bit, int target);

};
