#include "MemoryBank.hxx"

MemoryBank::MemoryBank(int numberOfBanks, int size)
{
    numBanks = numberOfBanks;
    bankSize = size;

    data = new std::vector<unsigned_two_byte>(numBanks * size);

    for (int i = 0; i < numBanks * size; i++)
    {
        data->push_back(0);
    }
}

MemoryBank::~MemoryBank()
{
}

unsigned_two_byte MemoryBank::getData(int bank, int location)
{
    return (*data)[location + (bank * bankSize)];
}

void MemoryBank::setData(int bank, int location, unsigned_two_byte value)
{
    (*data)[location + (bank * bankSize)] = value;
}
