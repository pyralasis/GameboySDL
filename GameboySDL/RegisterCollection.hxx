#pragma once

#include "Types.hxx"

class RegisterCollection
{
private:
    unsigned_two_byte data[9];

public:
    RegisterCollection(/* args */);
    ~RegisterCollection();

    void setRegister(int registerIn, unsigned_two_byte value);
    void setRegisterDouble(int registerHigh, int registerLow, unsigned_two_byte valueHigh, unsigned_two_byte valueLow);

    unsigned_two_byte getRegister(int registerIn);
    unsigned_four_byte getRegisterDouble(int registerHigh, int registerLow);

    void setFlag(int flag);
    int getFlag(int flag);
    void clearFlag(int flag);

    void addA(unsigned_two_byte targetValue);
    void adcA(unsigned_two_byte targetValue);

    void subA(unsigned_two_byte targetValue);
    void sbcA(unsigned_two_byte targetValue);

    void andA(unsigned_two_byte targetValue);
    void xorA(unsigned_two_byte targetValue);
    void orA(unsigned_two_byte targetValue);
    void cpA(unsigned_two_byte targetValue);

    void addHL(unsigned_four_byte value);

    void incRegister(int registerIn);
    void incRegisterDouble(int registerHigh, int registerLow);

    void decRegister(int registerIn);
    void decRegisterDouble(int registerHigh, int registerLow);

    void rotateRightA();
    void rotateLeftA();
    void rotateLeftCircularA();
    void rotateRightCircularA();

    void rotateRight(int target);
    void rotateLeft(int target);
    void rotateRightCircular(int target);
    void rotateLeftCircular(int target);

    void SRA(int target);
    void SLA(int target);

    void SRL(int target);

    void swap(int target);

    void assignZero(int value);

    void assignCarryShiftLeft(unsigned_two_byte value);
    void assignCarryShiftRight(unsigned_two_byte value);

    void assignCarry(unsigned_two_byte value1, unsigned_two_byte value2);
    void assignCarryDouble(unsigned_four_byte value1, unsigned_four_byte value2);
    void assignCarryAdc(unsigned_two_byte value1, unsigned_two_byte value2, unsigned_two_byte value3);
    void assignCarrySbc(unsigned_two_byte value1, unsigned_two_byte value2, unsigned_two_byte value3);
    void assignCarrySub(unsigned_two_byte value1, unsigned_two_byte value2);

    void assignHalfcarryAdd(unsigned_two_byte value1, unsigned_two_byte value2);
    void assignHalfcarryAddDouble(unsigned_four_byte value1, unsigned_four_byte value2);
    void assignHalfcarryAdc(unsigned_two_byte value1, unsigned_two_byte value2, unsigned_two_byte value3);

    void assignHalfcarrySub(unsigned_two_byte value1, unsigned_two_byte value2);
    void assignHalfcarrySubDouble(unsigned_four_byte value1, unsigned_four_byte value2);
    void assignHalfcarrySbc(unsigned_two_byte value1, unsigned_two_byte value2, unsigned_two_byte value3);
};
