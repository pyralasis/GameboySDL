#include "RegisterCollection.hxx"
#include <iostream>

RegisterCollection::RegisterCollection()
{
    data[registerID::B] = 0x00;
    data[registerID::C] = 0x13;
    data[registerID::D] = 0x00;
    data[registerID::E] = 0xD8;
    data[registerID::H] = 0x01;
    data[registerID::L] = 0x4D;
    data[registerID::A] = 0x01;
    data[registerID::F] = 0xB0;
}

RegisterCollection::~RegisterCollection()
{
}

void RegisterCollection::setRegister(int registerIn, unsigned_two_byte value)
{
    data[registerIn] = value;
}

void RegisterCollection::setRegisterDouble(int registerHigh, int registerLow, unsigned_two_byte valueHigh, unsigned_two_byte valueLow)
{
    data[registerHigh] = valueHigh;
    data[registerLow] = valueLow;
}

unsigned_two_byte RegisterCollection::getRegister(int registerIn)
{
    return data[registerIn];
}

unsigned_four_byte RegisterCollection::getRegisterDouble(int registerHigh, int registerLow)
{
    unsigned_four_byte valueHigh = static_cast<unsigned_four_byte>(data[registerHigh]);
    valueHigh = valueHigh << 8;
    unsigned_four_byte valueLow = static_cast<unsigned_four_byte>(data[registerLow]);
    unsigned_four_byte output = valueHigh | valueLow;
    return output;
}

void RegisterCollection::setFlag(int flag)
{
    data[8] |= 1 << flag;
}

int RegisterCollection::getFlag(int flag)
{
    return (data[8] >> flag) & 1;
}

void RegisterCollection::clearFlag(int flag)
{
    data[8] &= ~(1 << flag);
}

void RegisterCollection::addA(unsigned_two_byte targetValue)
{
    unsigned_two_byte oldValue = data[registerID::A];
    data[registerID::A] = oldValue + targetValue;
    assignZero(data[registerID::A]);
    clearFlag(6);
    assignHalfcarryAdd(oldValue, targetValue);
    assignCarry(oldValue, targetValue);
}

void RegisterCollection::adcA(unsigned_two_byte targetValue)
{
    int carry = getFlag(4);
    unsigned_two_byte oldValue = data[registerID::A];
    data[registerID::A] = data[registerID::A] + targetValue;
    data[registerID::A] = data[registerID::A] + carry;
    assignZero(data[registerID::A]);
    clearFlag(6);
    assignHalfcarryAdc(oldValue, targetValue, carry);
    assignCarryAdc(oldValue, targetValue, carry);
}

void RegisterCollection::subA(unsigned_two_byte targetValue)
{
    unsigned_two_byte oldValue = data[registerID::A];
    data[registerID::A] = oldValue - targetValue;
    assignZero(data[registerID::A]);
    setFlag(6);
    assignHalfcarrySub(oldValue, targetValue);
    assignCarrySub(oldValue, targetValue);
}

void RegisterCollection::sbcA(unsigned_two_byte targetValue)
{
    unsigned_two_byte carry = getFlag(4);
    unsigned_two_byte oldValue = data[registerID::A];
    data[registerID::A] = data[registerID::A] - targetValue;
    data[registerID::A] = data[registerID::A] - getFlag(4);
    assignZero(data[registerID::A]);
    setFlag(6);
    assignHalfcarrySbc(oldValue, targetValue, carry);
    assignCarrySbc(oldValue, targetValue, carry);
}

void RegisterCollection::andA(unsigned_two_byte targetValue)
{
    unsigned_two_byte oldValue = data[registerID::A];
    data[registerID::A] = oldValue & targetValue;
    assignZero(data[registerID::A]);
    clearFlag(6);
    setFlag(5);
    clearFlag(4);
}

void RegisterCollection::xorA(unsigned_two_byte targetValue)
{
    unsigned_two_byte oldValue = data[registerID::A];
    data[registerID::A] = oldValue ^ targetValue;
    assignZero(data[registerID::A]);
    clearFlag(6);
    clearFlag(5);
    clearFlag(4);
}

void RegisterCollection::orA(unsigned_two_byte targetValue)
{
    unsigned_two_byte oldValue = data[registerID::A];
    data[registerID::A] = oldValue | targetValue;
    assignZero(data[registerID::A]);
    clearFlag(6);
    clearFlag(5);
    clearFlag(4);
}

void RegisterCollection::cpA(unsigned_two_byte targetValue)
{
    unsigned_two_byte comparison = data[registerID::A];
    comparison = comparison - targetValue;
    assignZero(comparison);
    setFlag(6);
    assignHalfcarrySub(data[registerID::A], targetValue);
    assignCarrySub(data[registerID::A], targetValue);
}

void RegisterCollection::addHL(unsigned_four_byte value)
{
    unsigned_four_byte oldValue = getRegisterDouble(registerID::H, registerID::L);
    unsigned_four_byte sum = oldValue + value;
    setRegisterDouble(registerID::H, registerID::L, sum >> 8, sum & 0xFF);
    clearFlag(6);
    assignHalfcarryAddDouble(oldValue, value);
    assignCarryDouble(oldValue, value);
}

void RegisterCollection::incRegister(int registerIn)
{
    unsigned_two_byte oldValue = data[registerIn];
    unsigned_two_byte newValue = oldValue + 1;
    data[registerIn] = newValue;
    assignZero(newValue);
    clearFlag(6);
    assignHalfcarryAdd(oldValue, 1);
}

void RegisterCollection::incRegisterDouble(int registerHigh, int registerLow)
{
    unsigned_two_byte high = data[registerHigh];
    unsigned_two_byte low = data[registerLow];
    unsigned_four_byte combined = (high << 8) | low;
    combined++;
    data[registerHigh] = (combined & 0xFF00) >> 8;
    data[registerLow] = combined & 0x00FF;
}

void RegisterCollection::decRegister(int registerIn)
{
    unsigned_two_byte oldValue = data[registerIn];
    unsigned_two_byte newValue = data[registerIn] - 1;
    data[registerIn] = newValue;
    assignZero(newValue);
    setFlag(6);
    assignHalfcarrySub(oldValue, 1);
}

void RegisterCollection::decRegisterDouble(int registerHigh, int registerLow)
{
    unsigned_two_byte high = data[registerHigh];
    unsigned_two_byte low = data[registerLow];
    unsigned_four_byte combined = (high << 8) | low;
    combined = combined - 1;
    data[registerHigh] = (combined & 0xFF00) >> 8;
    data[registerLow] = combined & 0x00FF;
}

void RegisterCollection::rotateRightA()
{
    int carry = getFlag(4);
    if (data[registerID::A] & 1)
        setFlag(4);
    else
        clearFlag(4);
    data[registerID::A] = ((data[registerID::A] >> 1 | (carry << 7)) & 0xFF);
    clearFlag(7);
    clearFlag(6);
    clearFlag(5);
}

void RegisterCollection::rotateLeftA()
{
    int carry = getFlag(4);
    if (data[registerID::A] & 0x80)
        setFlag(4);
    else
        clearFlag(4);
    data[registerID::A] = ((data[registerID::A] << 1 | (carry)) & 0xFF);
    clearFlag(7);
    clearFlag(6);
    clearFlag(5);
}

void RegisterCollection::rotateLeftCircularA()
{
    data[registerID::A] = (((data[registerID::A] << 1) | (data[registerID::A] >> 7)) & 0xFF);
    clearFlag(7);
    clearFlag(6);
    clearFlag(5);
    assignCarryShiftLeft(data[registerID::A]);
}

void RegisterCollection::rotateRightCircularA()
{
    data[registerID::A] = (((data[registerID::A] >> 1) | (data[registerID::A] << 7)) & 0xFF);
    clearFlag(7);
    clearFlag(6);
    clearFlag(5);
    assignCarryShiftRight(data[registerID::A]);
}

void RegisterCollection::rotateRight(int target)
{
    unsigned_two_byte registerValue = data[target];
    int carry = getFlag(4);
    if (registerValue & 1)
        setFlag(4);
    else
        clearFlag(4);
    registerValue = ((registerValue >> 1 | (carry << 7)) & 0xFF);
    data[target] = registerValue;
    assignZero(registerValue);
    clearFlag(6);
    clearFlag(5);
}

void RegisterCollection::rotateLeft(int target)
{
    unsigned_two_byte registerValue = data[target];
    int carry = getFlag(4);
    if (registerValue & 0x80)
        setFlag(4);
    else
        clearFlag(4);
    registerValue = ((registerValue << 1 | (carry)) & 0xFF);
    data[target] = registerValue;
    assignZero(registerValue);
    clearFlag(6);
    clearFlag(5);
}

void RegisterCollection::rotateRightCircular(int target)
{
    unsigned_two_byte registerValue = data[target];
    registerValue = (((registerValue >> 1) | (registerValue << 7)) & 0xFF);
    data[target] = registerValue;
    assignZero(registerValue);
    clearFlag(6);
    clearFlag(5);
    assignCarryShiftRight(registerValue);
}

void RegisterCollection::rotateLeftCircular(int target)
{
    unsigned_two_byte registerValue = data[target];
    registerValue = (((registerValue << 1) | (registerValue >> 7)) & 0xFF);
    data[target] = registerValue;
    assignZero(registerValue);
    clearFlag(6);
    clearFlag(5);
    assignCarryShiftLeft(registerValue);
}
void RegisterCollection::SLA(int target)
{
    unsigned_two_byte registerValue = getRegister(target);
    if (registerValue & 0x80)
    {
        setFlag(4);
    }
    else
    {
        clearFlag(4);
    }
    registerValue = (registerValue << 1) & 0xFF;
    data[target] = registerValue;
    assignZero(registerValue);
    clearFlag(6);
    clearFlag(5);
}

void RegisterCollection::SRA(int target)
{
    unsigned_two_byte registerValue = data[target];
    if (registerValue & 0x01)
    {
        setFlag(4);
    }
    else
    {
        clearFlag(4);
    }
    int bitSeven = (registerValue & 0x80);
    registerValue = registerValue >> 1;
    registerValue |= bitSeven;
    data[target] = registerValue;
    assignZero(registerValue);
    clearFlag(6);
    clearFlag(5);
}

void RegisterCollection::SRL(int target)
{
    unsigned_two_byte registerValue = data[target];
    if ((registerValue & 0x01) == 0x01)
    {
        setFlag(4);
    }
    else
    {
        clearFlag(4);
    }
    registerValue = registerValue >> 1;
    assignZero(registerValue);
    clearFlag(6);
    clearFlag(5);
    setRegister(target, registerValue);
}

void RegisterCollection::swap(int target)
{
    unsigned_two_byte registerValue = data[target];
    unsigned_two_byte high = registerValue >> 4;
    unsigned_two_byte low = registerValue & 0xF;
    registerValue = (low << 4) | high;
    assignZero(registerValue);
    clearFlag(6);
    clearFlag(5);
    clearFlag(4);
    setRegister(target, registerValue);
}

void RegisterCollection::assignZero(int value)
{
    if (value == 0)
        setFlag(7);
    else
        clearFlag(7);
}

void RegisterCollection::assignCarryShiftLeft(unsigned_two_byte value)
{
    if (value & 0x01)
        setFlag(4);
    else
        clearFlag(4);
}

void RegisterCollection::assignCarryShiftRight(unsigned_two_byte value)
{
    if (value & 0x80)
        setFlag(4);
    else
        clearFlag(4);
}

void RegisterCollection::assignCarry(unsigned_two_byte value1, unsigned_two_byte value2)
{
    if (((value1 + value2) & 0xFF) < value1)
        setFlag(4);
    else
        clearFlag(4);
}

void RegisterCollection::assignCarryDouble(unsigned_four_byte value1, unsigned_four_byte value2)
{
    if (((value1 + value2) & 0xFFFF) < value1)
        setFlag(4);
    else
        clearFlag(4);
}

void RegisterCollection::assignCarryAdc(unsigned_two_byte value1, unsigned_two_byte value2, unsigned_two_byte value3)
{
    int flag = 0;
    if (((value1 + value2) & 0xFF) < value1)
    {
        flag |= 1;
    }
    unsigned_two_byte temp = (value1 + value2);
    if (((temp + value3) & 0xFF) < temp)
    {
        flag |= 1;
    }
    if (flag)
        setFlag(4);
    else
        clearFlag(4);
}

void RegisterCollection::assignCarrySbc(unsigned_two_byte value1, unsigned_two_byte value2, unsigned_two_byte value3)
{
    int flag = 0;
    if (((value1 - value2) & 0xFF) > value1)
    {
        flag |= 1;
    }
    unsigned_two_byte temp = (value1 - value2);
    if (((temp - value3) & 0xFF) > temp)
    {
        flag |= 1;
    }
    if (flag)
        setFlag(4);
    else
        clearFlag(4);
}

void RegisterCollection::assignCarrySub(unsigned_two_byte value1, unsigned_two_byte value2)
{
    if (((value1 - value2) & 0xFF) > value1)
        setFlag(4);
    else
        clearFlag(4);
}

void RegisterCollection::assignHalfcarryAdd(unsigned_two_byte value1, unsigned_two_byte value2)
{
    if ((((value1 & 0xf) + (value2 & 0xf)) & 0x10) == 0x10)
        setFlag(5);
    else
        clearFlag(5);
}

void RegisterCollection::assignHalfcarryAddDouble(unsigned_four_byte value1, unsigned_four_byte value2)
{
    if ((((value1 & 0xfff) + (value2 & 0xfff)) & 0x1000) == 0x1000)
        setFlag(5);
    else
        clearFlag(5);
}

void RegisterCollection::assignHalfcarryAdc(unsigned_two_byte value1, unsigned_two_byte value2, unsigned_two_byte value3)
{
    int flag = 0;
    if ((((value1 & 0xf) + (value2 & 0xf)) & 0x10) == 0x10)
    {
        flag |= 1;
    }
    unsigned_two_byte temp = value1 + value2;
    if ((((temp & 0xf) + (value3 & 0xf)) & 0x10) == 0x10)
    {
        flag |= 1;
    }
    if (flag)
        setFlag(5);
    else
        clearFlag(5);
}

void RegisterCollection::assignHalfcarrySub(unsigned_two_byte value1, unsigned_two_byte value2)
{
    if ((((value1 & 0xf) - (value2 & 0xf)) & 0x10) == 0x10)
        setFlag(5);
    else
        clearFlag(5);
}
void RegisterCollection::assignHalfcarrySubDouble(unsigned_four_byte value1, unsigned_four_byte value2)
{
    if ((((value1 & 0xfff) - (value2 & 0xfff)) & 0x1000) == 0x1000)
        setFlag(5);
    else
        clearFlag(5);
}

void RegisterCollection::assignHalfcarrySbc(unsigned_two_byte value1, unsigned_two_byte value2, unsigned_two_byte value3)
{
    int flag = 0;
    if ((((value1 & 0xf) - (value2 & 0xf)) & 0x10) == 0x10)
    {
        flag |= 1;
    }
    unsigned_two_byte temp = value1 - value2;
    if ((((temp & 0xf) - (value3 & 0xf)) & 0x10) == 0x10)
    {
        flag |= 1;
    }
    if (flag)
        setFlag(5);
    else
        clearFlag(5);
}
