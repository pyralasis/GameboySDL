#pragma once

#include <vector>
#include "Types.hxx"

class MemoryBank
{
private:
    int bankSize;
    int numBanks;
    std::vector<unsigned_two_byte>* data;
public:
    MemoryBank(int numberOfBanks, int size);
    ~MemoryBank();
    unsigned_two_byte getData(int bank, int location);
    void setData(int bank, int location, unsigned_two_byte value);
};