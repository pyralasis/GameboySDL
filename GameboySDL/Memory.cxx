#include "Memory.hxx"
#include <cmath>
#include <iostream>

Memory::Memory()
{
}

Memory::~Memory()
{
}

void Memory::initialize(unsigned_two_byte *romInput, int romLength)
{
    mbcType = romInput[0x147];
    int romSize = romInput[0x148];
    numRomBanks = std::pow(2, (romSize + 1));
    int ramSize = romInput[0x149];
    switch (ramSize)
    {
    case 0:
        numRamBanks = 0;
        break;
    case 2:
        numRamBanks = 1;
        break;
    case 3:
        numRamBanks = 4;
        break;
    case 4:
        numRamBanks = 16;
        break;
    case 5:
        numRamBanks = 8;
        break;
    }
    rom = new MemoryBank(numRomBanks + 1, 16384);
    ram = new MemoryBank(numRamBanks + 1, 8192);

    int romArrayIndex = 0;
    int currentBankIndex = 0;
    int currentBank = 0;
    // while (romArrayIndex < 16384)
    // {
    //     rom->setData(0, romArrayIndex, romInput[romArrayIndex]);
    //     romArrayIndex++;
    // }
    while (romArrayIndex < romLength - 1)
    {
        if (currentBankIndex == 16384)
        {
            currentBankIndex = 0;
            currentBank++;
        }
        rom->setData(currentBank, currentBankIndex, romInput[romArrayIndex]);
        currentBankIndex++;

        romArrayIndex++;
    }

    writeMemory(0xFF00, 0xCF);
    writeMemory(0xFF01, 0x00);
    writeMemory(0xFF02, 0x7E);
    io->setData(0, 4, 0xAB);
    writeMemory(0xFF05, 0x00);
    writeMemory(0xFF06, 0x00);
    writeMemory(0xFF07, 0xF8);
    writeMemory(0xFF0F, 0xE1);
    writeMemory(0xFF10, 0x80);
    writeMemory(0xFF11, 0xBF);
    writeMemory(0xFF12, 0xF3);
    writeMemory(0xFF13, 0xFF);
    writeMemory(0xFF14, 0xBF);
    writeMemory(0xFF16, 0x3F);
    writeMemory(0xFF17, 0x00);
    writeMemory(0xFF18, 0xFF);
    writeMemory(0xFF19, 0xBF);
    writeMemory(0xFF1A, 0x7F);
    writeMemory(0xFF1B, 0xFF);
    writeMemory(0xFF1C, 0x9F);
    writeMemory(0xFF1D, 0xFF);
    writeMemory(0xFF1E, 0xBF);
    writeMemory(0xFF20, 0xFF);
    writeMemory(0xFF21, 0x00);
    writeMemory(0xFF22, 0x00);
    writeMemory(0xFF23, 0xBF);
    writeMemory(0xFF24, 0x77);
    writeMemory(0xFF25, 0xF3);
    writeMemory(0xFF26, 0xF1);
    writeMemory(0xFF40, 0x91);
    writeMemory(0xFF41, 0x85);
    writeMemory(0xFF42, 0x00);
    writeMemory(0xFF43, 0x00);
    writeMemory(0xFF44, 0x00);
    writeMemory(0xFF45, 0x00);
    // writeMemory(0xFF46, 0xFF);
    io->setData(0, 0x46, 0xFF);
    writeMemory(0xFF47, 0xFC);
    writeMemory(0xFF48, 0x00);
    writeMemory(0xFF49, 0x00);
    writeMemory(0xFF4A, 0x00);
    writeMemory(0xFF4B, 0x00);
    writeMemory(0xFF4D, 0xFF);
    writeMemory(0xFF4F, 0xFF);
    writeMemory(0xFF51, 0xFF);
    writeMemory(0xFF52, 0xFF);
    writeMemory(0xFF53, 0xFF);
    writeMemory(0xFF54, 0xFF);
    writeMemory(0xFF55, 0xFF);
    writeMemory(0xFF56, 0xFF);
    writeMemory(0xFF68, 0xFF);
    writeMemory(0xFF69, 0xFF);
    writeMemory(0xFF6A, 0xFF);
    writeMemory(0xFF6B, 0xFF);
    writeMemory(0xFF70, 0xFF);
    writeMemory(0xFFFF, 0x00);
}

unsigned_two_byte Memory::readMemory(unsigned_four_byte address)
{
    if (address < 0x4000)
        return rom->getData(0, address);
    else if (address < 0x8000)
        return rom->getData(currentRomBank , address - 0x4000);
    else if (address < 0xA000)
        return vram->getData(0, address - 0x8000);
    else if (address < 0xC000)
        return ram->getData(0, address - 0xA000);
    else if (address < 0xE000)
        return wram->getData(0, address - 0xC000);
    else if (address < 0xFE00)
        return echoram->getData(0, address - 0xE000);
    else if (address < 0xFEA0)
        return oam->getData(0, address - 0xFE00);
    else if (address < 0xFF00)
        return prohibited->getData(0, address - 0xFEA0);
    else if (address < 0xFF80)
        return io->getData(0, address - 0xFF00);
    else if (address < 0xFFFF)
        return hram->getData(0, address - 0xFF80);
    else if (address == 0xFFFF)
        return ie->getData(0, 0);
    else
        return 0;
}

void Memory::writeMemory(unsigned_four_byte address, unsigned_two_byte value)
{
    switch (mbcType)
    {
    case 0: // No MBC
        writeMemoryMbcZero(address, value);
        break;
    case 1:
    case 3: // MBC1
        writeMemoryMbcOne(address, value);
        break;
        // case 0x13://MBC3+RAM+BATTERY
        //     writeMemoryMbcThree(address, value);
        //     break;
        // default:
        //     throw new Error("Invalid MBC Type: This MBC Chip is not Implemented");
    }
}

void Memory::writeMemoryMbcZero(unsigned_four_byte address, unsigned_two_byte value)
{
    if (address < 0x8000)
    {
    }
    else if (address < 0xA000)
    {
        vram->setData(0, address - 0x8000, value);
    }
    else if (address < 0xC000)
    {
        ram->setData(currentRamBank, address - 0xA000, value);
    }
    else if (address < 0xE000)
    {
        wram->setData(0, address - 0xC000, value);
    }
    else if (address < 0xFE00)
        return echoram->setData(0, address - 0xE000, value);
    else if (address < 0xFEA0)
    {
        oam->setData(0, address - 0xFE00, value);
    }
    else if (address < 0xFF00)
        return prohibited->setData(0, address - 0xFEA0, value);
    else if (address < 0xFF80)
    {
        if (address == 0xFF04)
        { // RESET TIMA
            io->setData(0, address - 0xFF00, 0);
        }
        else if (address == 0xFF46)
        { // OAM DMA
            io->setData(0, address - 0xFF00, value);
            for (int i = 0; i < 160; i++)
            {
                oam->setData(0, i, readMemory((value << 8) | i));
                cpu->tickClock(1);
            }
        }
        else
        {
            io->setData(0, address - 0xFF00, value);
        }
    }
    else if (address < 0xFFFF)
    {
        hram->setData(0, address - 0xFF80, value);
    }
    else if (address == 0xFFFF)
    {
        ie->setData(0, 0, value);
    }
}

void Memory::writeMemoryMbcOne(unsigned_four_byte address, unsigned_two_byte value)
{ // TODO

    if (address < 0x2000)
    { // Any write to here will enable RAM if the value has a lower nibble of 0xA                let low = value | 0xF;
        unsigned_two_byte low = value | 0xF;
        if (low == 0xA)
        {
            ramEnabled = true;
        }
        else
        {
            ramEnabled = false;
        }
    }
    else if (address < 0x4000)
    { // Writing to here will change the active ROM Bank
        // TODO
        unsigned_two_byte maskedValue = value & 0x1F;
        switch (maskedValue)
        {
        case 0:
            maskedValue = 1;
            break;
        }
        currentRomBank = (value);
    }
    else if (address < 0x6000)
    {
        // TODO
        // need to figure out ram if need bank or not
        if (ramEnabled)
        {
        }
    }
    else if (address < 0x8000)
    {
        if (value == 0)
        {
            bankMode = false;
        }
        else if (value == 1)
        {
            bankMode = true;
        }
    }
    else if (address < 0xA000)
    {
        vram->setData(0, address - 0x8000, value);
    }
    else if (address < 0xC000)
    {
        if (ramEnabled)
        {
            ram->setData(0, address - 0xA000, value);
        }
    }
    else if (address < 0xE000)
    {
        wram->setData(0, address - 0xC000, value);
    }
    else if (address < 0xFE00)
        return echoram->setData(0, address - 0xE000, value);
    else if (address < 0xFEA0)
    {
        oam->setData(0, address - 0xFE00, value);
    }
    else if (address < 0xFF00)
        return prohibited->setData(0, address - 0xFEA0, value);
    else if (address < 0xFF80)
    {
        if (address == 0xFF04)
        {
            io->setData(0, address - 0xFF00, 0);
        }
        else if (address == 0xFF46)
        { // OAM DMA
            io->setData(0, address - 0xFF00, value);
            for (int i = 0; i < 160; i++)
            {
                oam->setData(0, i, readMemory((value << 8) | i));
                cpu->tickClock(1);
            }
        }
        else
        {
            io->setData(0, address - 0xFF00, value);
        }
    }
    else if (address < 0xFFFF)
    {
        hram->setData(0, address - 0xFF80, value);
    }
    else if (address == 0xFFFF)
    {
        ie->setData(0, 0, value);
    }
}

void Memory::setCpu(Cpu *input)
{
    cpu = input;
}

void Memory::setDiv(unsigned_two_byte div)
{
    io->setData(0, 0x4, div);
}
