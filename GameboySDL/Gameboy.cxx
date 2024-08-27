#include "Gameboy.hxx"
#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include "Types.hxx"
#include <sstream>

#define formatter2 std::uppercase << std::setfill('0') << std::setw(2) << std::hex
#define formatter4 std::uppercase << std::setfill('0') << std::setw(4) << std::hex

Gameboy::Gameboy()
{
}

Gameboy::~Gameboy()
{
}

void Gameboy::readRom(unsigned char* arrayBuffer, int size)
{
    
    memory->initialize(arrayBuffer, size);
    romLoaded = true;
}

void Gameboy::initialize()
{
    cpu->setMemory(memory);
    cpu->setPpu(ppu);

    ppu->setMemory(memory);

    memory->setCpu(cpu);
    controls->setMemory(memory);
}

void Gameboy::mainLoop()
{
    while (!ppu->getIsFrameReady())
    {
        cpu->interrupt();

        //getNextInstructionDebugLine();
        cpu->execute();
        controls->updateInputState();
    }
    ppu->setIsFrameReady(false);
}

std::vector<int> Gameboy::getBackground()
{
    return ppu->getBackgroundTileNumbers();
}

std::vector<int> Gameboy::getTileData()
{
    return ppu->getTiles();
}

void Gameboy::setBackgroundSettings(unsigned_four_byte backgroundAddress, unsigned_four_byte tilemapAddress)
{

    ppu->setDebugAddresses(backgroundAddress, tilemapAddress);
}

void Gameboy::getNextInstructionDebugLine()
{
    std::stringstream debugString;
    debugString << "A: " << formatter2 << static_cast<int>(cpu->getRegisters()->getRegister(registerID::A)) << " "
                << "F: " << formatter2 << static_cast<int>(cpu->getRegisters()->getRegister(registerID::F)) << " "
                << "B: " << formatter2 << static_cast<int>(cpu->getRegisters()->getRegister(registerID::B)) << " "
                << "C: " << formatter2 << static_cast<int>(cpu->getRegisters()->getRegister(registerID::C)) << " "
                << "D: " << formatter2 << static_cast<int>(cpu->getRegisters()->getRegister(registerID::D)) << " "
                << "E: " << formatter2 << static_cast<int>(cpu->getRegisters()->getRegister(registerID::E)) << " "
                << "H: " << formatter2 << static_cast<int>(cpu->getRegisters()->getRegister(registerID::H)) << " "
                << "L: " << formatter2 << static_cast<int>(cpu->getRegisters()->getRegister(registerID::L)) << " "
                << "SP: " << formatter4 << static_cast<int>(cpu->getStackPointer()) << " "
                << "PC: 00:" << formatter4 << static_cast<int>(cpu->getProgramCounter()) << " "
                << "(" << formatter2 << static_cast<int>(memory->readMemory(cpu->getProgramCounter())) << " "
                << formatter2 << static_cast<int>(memory->readMemory(cpu->getProgramCounter() + 1)) << " "
                << formatter2 << static_cast<int>(memory->readMemory(cpu->getProgramCounter() + 2)) << " "
                << formatter2 << static_cast<int>(memory->readMemory(cpu->getProgramCounter() + 3)) << ")"
                << std::endl;
    myfile.open("C:\\Users\\quazl\\source\\repos\\GameboySDL\\LOG.txt", std::ios::out | std::ios::app);
    myfile << debugString.str();
    myfile.close();
}

bool Gameboy::isRomLoaded()
{
    return romLoaded;
}

Ppu* Gameboy::getPpu()
{
    return ppu;
}

Controls* Gameboy::getControls()
{
    return controls;
}
