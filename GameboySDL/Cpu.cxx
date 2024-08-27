#include "Cpu.hxx"
#include <iostream>

void Cpu::setMemory(Memory *value)
{
    memory = value;
}

void Cpu::setPpu(Ppu *value)
{
    ppu = value;
}

RegisterCollection *Cpu::getRegisters()
{
    return &registers;
}

unsigned_four_byte Cpu::getStackPointer()
{
    return stackPointer;
}

unsigned_four_byte Cpu::getProgramCounter()
{
    return programCounter;
}

void Cpu::jumpConditional(bool condition)
{
    unsigned_two_byte currentOpcodeLow = fetch();
    unsigned_two_byte high = fetch();
    if (condition)
    {
        programCounter = (high << 8) | currentOpcodeLow;
        tickClock(16);
    }
    else
    {
        tickClock(12);
    }
}

void Cpu::jumpRelativeConditional(bool condition)
{
    unsigned_two_byte location = fetch();
    if (condition)
    {
        programCounter += static_cast<signed_two_byte>(location);
        tickClock(12);
    }
    else
    {
        tickClock(8);
    }
}

void Cpu::returnConditional(bool condition)
{
    if (condition)
    {
        unsigned_two_byte currentOpcodeLow = memory->readMemory(stackPointer);
        stackPointer++;
        unsigned_two_byte high = memory->readMemory(stackPointer);
        stackPointer++;
        programCounter = (high << 8) | currentOpcodeLow;
        tickClock(20);
    }
    else
    {
        tickClock(8);
    }
}

void Cpu::callConditional(bool condition)
{
    unsigned_two_byte currentOpcodeLow = fetch();
    unsigned_two_byte high = fetch();
    if (condition)
    {
        stackPointer--;
        memory->writeMemory(stackPointer, (programCounter >> 8));
        stackPointer--;
        memory->writeMemory(stackPointer, (programCounter & 0xff));
        programCounter = (high << 8) | currentOpcodeLow;
        tickClock(24);
    }
    else
    {
        tickClock(12);
    }
}

void Cpu::pop(int register1, int register2)
{
    unsigned_two_byte currentOpcodeLow = memory->readMemory(stackPointer);
    stackPointer++;
    unsigned_two_byte high = memory->readMemory(stackPointer);
    if (register2 == registerID::F)
    {
        currentOpcodeLow = currentOpcodeLow & 0xF0;
    }
    stackPointer++;
    registers.setRegisterDouble(register1, register2, high, currentOpcodeLow);
    tickClock(12);
}

void Cpu::push(int register1, int register2)
{
    unsigned_two_byte high = registers.getRegister(register1);
    stackPointer--;
    memory->writeMemory(stackPointer, high);
    unsigned_two_byte currentOpcodeLow = registers.getRegister(register2);
    stackPointer--;
    memory->writeMemory(stackPointer, currentOpcodeLow);
    tickClock(16);
}

void Cpu::ldXY(int registerX, int registerY)
{
    registers.setRegister(registerX, registers.getRegister(registerY));
    if (registerY == 0x6)
    {
        tickClock(8);
    }
    else
    {
        tickClock(4);
    }
}

void Cpu::rst(unsigned_four_byte location)
{
    stackPointer--;
    memory->writeMemory(stackPointer, (programCounter >> 8));
    stackPointer--;
    memory->writeMemory(stackPointer, (programCounter & 0xff));
    programCounter = location;
    tickClock(16);
}

void Cpu::bit(int bit, int target)
{
    unsigned_two_byte registerValue = registers.getRegister(target);
    int bitValue = registerValue & (1 << bit);
    registers.assignZero(bitValue);
    registers.clearFlag(6);
    registers.setFlag(5);
    if (target == 0x6)
    {
        tickClock(12);
    }
    else
    {
        tickClock(8);
    }
}

void Cpu::res(int bit, int target)
{
    unsigned_two_byte registerValue = registers.getRegister(target);
    registerValue &= ~(1 << bit);
    registers.setRegister(target, registerValue);
    if (target == 0x6)
    {
        tickClock(16);
    }
    else
    {
        tickClock(8);
    }
}

void Cpu::set(int bit, int target)
{
    unsigned_two_byte registerValue = registers.getRegister(target);
    registerValue |= (1 << bit);
    registers.setRegister(target, registerValue);
    if (target == 0x6)
    {
        tickClock(16);
    }
    else
    {
        tickClock(8);
    }
}

Cpu::Cpu()
{
}

Cpu::~Cpu()
{
}

unsigned_two_byte Cpu::fetch()
{
    unsigned_two_byte currentOpcode = memory->readMemory(programCounter);
    programCounter++;
    return currentOpcode;
}

void Cpu::execute()
{
    unsigned_two_byte currentOpcode = fetch();
    unsigned_two_byte currentOpcodeHigh = currentOpcode >> 4;
    unsigned_two_byte currentOpcodeLow = currentOpcode & 0xF;
    if (currentOpcode != 0xCB)
    {
        switch (currentOpcodeHigh)
        {
        case 0x0:
            switch (currentOpcodeLow)
            {
            case 0x0:
            {
                tickClock(4);
                break;
            }
            case 0x1:
            {
                unsigned_two_byte currentOpcodeLow = fetch();
                unsigned_two_byte high = fetch();
                registers.setRegisterDouble(registerID::B, registerID::C, high, currentOpcodeLow);
                tickClock(12);
                break;
            }
            case 0x2:
            {
                int location = registers.getRegisterDouble(registerID::B, registerID::C);
                unsigned_two_byte value = registers.getRegister(registerID::A);
                memory->writeMemory(location, value);
                tickClock(8);
                break;
            }
            case 0x3:
            {
                registers.incRegisterDouble(registerID::B, registerID::C);
                tickClock(8);
                break;
            }
            case 0x4:
            {
                registers.incRegister(registerID::B);
                tickClock(4);
                break;
            }
            case 0x5:
            {
                registers.decRegister(registerID::B);
                tickClock(4);
                break;
            }
            case 0x6:
            {
                registers.setRegister(registerID::B, fetch());
                tickClock(8);
                break;
            }
            case 0x7:
            {
                registers.rotateLeftCircularA();
                tickClock(4);
                break;
            }
            case 0x8:
            {
                unsigned_two_byte currentOpcodeLow = fetch();
                unsigned_two_byte high = fetch();
                unsigned_four_byte location = (high << 8) | currentOpcodeLow;
                unsigned_two_byte lowStack = stackPointer & 0xFF;
                unsigned_two_byte highStack = stackPointer >> 8;
                memory->writeMemory(location, lowStack);
                memory->writeMemory(location + 1, highStack);
                tickClock(20);
                break;
            }
            case 0x9:
            {
                registers.addHL(registers.getRegisterDouble(registerID::B, registerID::C));
                tickClock(8);
                break;
            }
            case 0xA:
            {
                unsigned_two_byte value = memory->readMemory(registers.getRegisterDouble(registerID::B, registerID::C));
                registers.setRegister(registerID::A, value);
                tickClock(8);
                break;
            }
            case 0xB:
            {
                registers.decRegisterDouble(registerID::B, registerID::C);
                tickClock(8);
                break;
            }
            case 0xC:
            {
                registers.incRegister(registerID::C);
                tickClock(4);
                break;
            }
            case 0xD:
            {
                registers.decRegister(registerID::C);
                tickClock(4);
                break;
            }
            case 0xE:
            {
                registers.setRegister(registerID::C, fetch());
                tickClock(8);
                break;
            }
            case 0xF:
            {
                registers.rotateRightCircularA();
                tickClock(4);
                break;
            }
            }
            break;
        case 0x1:
            switch (currentOpcodeLow)
            {
            case 0x0:
            { // TODO: STOP
                tickClock(4);
                break;
            }
            case 0x1:
            {
                unsigned_two_byte currentOpcodeLow = fetch();
                unsigned_two_byte high = fetch();
                registers.setRegisterDouble(registerID::D, registerID::E, high, currentOpcodeLow);
                tickClock(12);
                break;
            }
            case 0x2:
            {
                unsigned_four_byte location = registers.getRegisterDouble(registerID::D, registerID::E);
                unsigned_two_byte value = registers.getRegister(registerID::A);
                memory->writeMemory(location, value);
                tickClock(8);
                break;
            }
            case 0x3:
            {
                registers.incRegisterDouble(registerID::D, registerID::E);
                tickClock(8);
                break;
            }
            case 0x4:
            {
                registers.incRegister(registerID::D);
                tickClock(4);
                break;
            }
            case 0x5:
            {
                registers.decRegister(registerID::D);
                tickClock(4);
                break;
            }
            case 0x6:
            {
                registers.setRegister(registerID::D, fetch());
                tickClock(8);
                break;
            }
            case 0x7:
            {
                registers.rotateLeftA();
                tickClock(4);
                break;
            }
            case 0x8:
            {
                unsigned_two_byte jumpOffset = fetch();
                programCounter += static_cast<signed_two_byte>(jumpOffset);
                tickClock(12);
                break;
            }
            case 0x9:
            {
                registers.addHL(registers.getRegisterDouble(registerID::D, registerID::E));
                tickClock(8);
                break;
            }
            case 0xA:
            {
                unsigned_two_byte value = memory->readMemory(registers.getRegisterDouble(registerID::D, registerID::E));
                registers.setRegister(registerID::A, value);
                tickClock(8);
                break;
            }
            case 0xB:
            {
                registers.decRegisterDouble(registerID::D, registerID::E);
                tickClock(8);
                break;
            }
            case 0xC:
            {
                registers.incRegister(registerID::E);
                tickClock(4);
                break;
            }
            case 0xD:
            {
                registers.decRegister(registerID::E);
                tickClock(4);
                break;
            }
            case 0xE:
            {
                registers.setRegister(registerID::E, fetch());
                tickClock(8);
                break;
            }
            case 0xF:
            {
                registers.rotateRightA();
                tickClock(4);
                break;
            }
            }
            break;
        case 0x2:
            switch (currentOpcodeLow)
            {
            case 0x0:
            {
                jumpRelativeConditional(!registers.getFlag(7));
                break;
            }
            case 0x1:
            {
                unsigned_two_byte currentOpcodeLow = fetch();
                unsigned_two_byte high = fetch();
                registers.setRegisterDouble(registerID::H, registerID::L, high, currentOpcodeLow);
                tickClock(12);
                break;
            }
            case 0x2:
            {
                unsigned_four_byte tempHLLocation = registers.getRegisterDouble(registerID::H, registerID::L);
                registers.setRegister(registerID::HL, memory->readMemory(tempHLLocation));
                unsigned_two_byte value = registers.getRegister(registerID::A);
                registers.setRegister(registerID::HL, value);
                memory->writeMemory(tempHLLocation, registers.getRegister(registerID::HL));
                registers.incRegisterDouble(registerID::H, registerID::L);
                tickClock(8);
                break;
            }
            case 0x3:
            {
                registers.incRegisterDouble(registerID::H, registerID::L);
                tickClock(8);
                break;
            }
            case 0x4:
            {
                registers.incRegister(registerID::H);
                tickClock(4);
                break;
            }
            case 0x5:
            {
                registers.decRegister(registerID::H);
                tickClock(4);
                break;
            }
            case 0x6:
            {
                registers.setRegister(registerID::H, fetch());
                tickClock(8);
                break;
            }
            case 0x7:
            {
                bool carry = registers.getFlag(4);
                bool halfcarry = registers.getFlag(5);
                bool negative = registers.getFlag(6);

                unsigned int currentValue = registers.getRegister(registerID::A);
                registers.clearFlag(4);
                if (!negative)
                {
                    unsigned_two_byte low = currentValue & 0xF;

                    if ((low > 0x9) || halfcarry)
                    {
                        currentValue += 0x06;
                    }

                    unsigned_two_byte high = currentValue >> 4;

                    if ((high > 0x9) || carry)
                    {
                        
                        currentValue += 0x60;
                        unsigned_two_byte temp = static_cast<unsigned_two_byte>(currentValue);
                        currentValue = static_cast<unsigned int>(temp);
                        registers.setFlag(4);
                    }
                }
                else
                {
                    if (carry)
                    {
                        currentValue -= 0x60;
                        unsigned_two_byte temp = static_cast<unsigned_two_byte>(currentValue);
                        currentValue = static_cast<unsigned int>(temp);
                        registers.setFlag(4);
                    }

                    if (halfcarry)
                    {
                        currentValue -= 0x06;
                        unsigned_two_byte temp = static_cast<unsigned_two_byte>(currentValue);
                        currentValue = static_cast<unsigned int>(temp);
                    }
                }
                registers.setRegister(registerID::A, static_cast<unsigned_two_byte>(currentValue));

                registers.clearFlag(5);
                registers.assignZero(registers.getRegister(registerID::A));
                break;
            }
            case 0x8:
            {
                jumpRelativeConditional(registers.getFlag(7));
                break;
            }
            case 0x9:
            {
                registers.addHL(registers.getRegisterDouble(registerID::H, registerID::L));
                tickClock(8);
                break;
            }
            case 0xA:
            {
                unsigned_four_byte tempHLLocation = registers.getRegisterDouble(registerID::H, registerID::L);
                registers.setRegister(registerID::HL, memory->readMemory(tempHLLocation));
                unsigned_two_byte value = registers.getRegister(registerID::HL);
                registers.incRegisterDouble(registerID::H, registerID::L);
                registers.setRegister(registerID::A, value);

                tickClock(8);
                break;
            }
            case 0xB:
            {
                registers.decRegisterDouble(registerID::H, registerID::L);
                tickClock(8);
                break;
            }
            case 0xC:
            {
                registers.incRegister(registerID::L);
                tickClock(4);
                break;
            }
            case 0xD:
            {
                registers.decRegister(registerID::L);
                tickClock(4);
                break;
            }
            case 0xE:
            {
                registers.setRegister(registerID::L, fetch());
                tickClock(8);
                break;
            }
            case 0xF:
            {
                registers.setRegister(registerID::A, (registers.getRegister(registerID::A) ^ 0xFF));
                registers.setFlag(6);
                registers.setFlag(5);
                break;
            }
            }
            break;
        case 0x3:
            switch (currentOpcodeLow)
            {
            case 0x0:
                jumpRelativeConditional(!registers.getFlag(4));
                break;
            case 0x1:
            {
                unsigned_two_byte currentOpcodeLow = fetch();
                unsigned_two_byte high = fetch();
                stackPointer = ((high << 8) | currentOpcodeLow);
                tickClock(12);
                break;
            }
            case 0x2:
            {
                unsigned_four_byte tempHLLocation = registers.getRegisterDouble(registerID::H, registerID::L);
                registers.setRegister(registerID::HL, memory->readMemory(tempHLLocation));
                unsigned_two_byte value = registers.getRegister(registerID::A);
                registers.setRegister(registerID::HL, value);
                memory->writeMemory(tempHLLocation, registers.getRegister(registerID::HL));
                registers.decRegisterDouble(registerID::H, registerID::L);
                tickClock(8);
                break;
            }
            case 0x3:
                stackPointer++;
                tickClock(8);
                break;
            case 0x4:
            {
                unsigned_four_byte tempHLLocation = registers.getRegisterDouble(registerID::H, registerID::L);
                registers.setRegister(registerID::HL, memory->readMemory(tempHLLocation));
                registers.incRegister(registerID::HL);
                memory->writeMemory(tempHLLocation, registers.getRegister(registerID::HL));
                tickClock(12);
                break;
            }
            case 0x5:
            {
                unsigned_four_byte tempHLLocation = registers.getRegisterDouble(registerID::H, registerID::L);
                registers.setRegister(registerID::HL, memory->readMemory(tempHLLocation));
                registers.decRegister(registerID::HL);
                memory->writeMemory(tempHLLocation, registers.getRegister(registerID::HL));
                tickClock(12);
                break;
            }
            case 0x6:
            {
                unsigned_four_byte tempHLLocation = registers.getRegisterDouble(registerID::H, registerID::L);
                registers.setRegister(registerID::HL, fetch());
                memory->writeMemory(tempHLLocation, registers.getRegister(registerID::HL));
                tickClock(12);
                break;
            }
            case 0x7:
                registers.clearFlag(6);
                registers.clearFlag(5);
                registers.setFlag(4);
                tickClock(4);
                break;
            case 0x8:
                jumpRelativeConditional(registers.getFlag(4));
                break;
            case 0x9:
            {
                registers.addHL(stackPointer);
                tickClock(8);
                break;
            }
            case 0xA:
            {
                unsigned_four_byte tempHLLocation = registers.getRegisterDouble(registerID::H, registerID::L);
                registers.setRegister(registerID::HL, memory->readMemory(tempHLLocation));
                unsigned_two_byte value = registers.getRegister(registerID::HL);
                registers.decRegisterDouble(registerID::H, registerID::L);
                registers.setRegister(registerID::A, value);
                tickClock(8);
                break;
            }
            case 0xB:
                stackPointer--;
                tickClock(8);
                break;
            case 0xC:
            {
                registers.incRegister(registerID::A);
                tickClock(4);
                break;
            }
            case 0xD:
            {
                registers.decRegister(registerID::A);
                tickClock(4);
                break;
            }
            case 0xE:
            {
                registers.setRegister(registerID::A, fetch());
                tickClock(8);
                break;
            }
            case 0xF:
                int carry = registers.getFlag(4);
                if (carry ^ 1)
                {
                    registers.setFlag(4);
                }
                else
                {
                    registers.clearFlag(4);
                }
                registers.clearFlag(6);
                registers.clearFlag(5);
                tickClock(4);
                break;
            }
            break;
        case 0x4:
            if (currentOpcodeLow == 0x6)
            {
                unsigned_four_byte tempHLLocation = registers.getRegisterDouble(registerID::H, registerID::L);
                registers.setRegister(registerID::HL, memory->readMemory(tempHLLocation));
                ldXY(registerID::B, currentOpcodeLow);
            }
            else if (currentOpcodeLow == 0xE)
            {
                unsigned_four_byte tempHLLocation = registers.getRegisterDouble(registerID::H, registerID::L);
                registers.setRegister(registerID::HL, memory->readMemory(tempHLLocation));
                ldXY(registerID::C, currentOpcodeLow - 8);
            }
            else if (currentOpcodeLow < 8)
            {
                ldXY(registerID::B, currentOpcodeLow);
            }
            else
            {
                ldXY(registerID::C, currentOpcodeLow - 8);
            }
            break;
        case 0x5:
            if (currentOpcodeLow == 0x6)
            {
                unsigned_four_byte tempHLLocation = registers.getRegisterDouble(registerID::H, registerID::L);
                registers.setRegister(registerID::HL, memory->readMemory(tempHLLocation));
                ldXY(registerID::D, currentOpcodeLow);
            }
            else if (currentOpcodeLow == 0xE)
            {
                unsigned_four_byte tempHLLocation = registers.getRegisterDouble(registerID::H, registerID::L);
                registers.setRegister(registerID::HL, memory->readMemory(tempHLLocation));
                ldXY(registerID::E, currentOpcodeLow - 8);
            }
            else if (currentOpcodeLow < 8)
            {
                ldXY(registerID::D, currentOpcodeLow);
            }
            else
            {
                ldXY(registerID::E, currentOpcodeLow - 8);
            }
            break;
        case 0x6:
            if (currentOpcodeLow == 0x6)
            {
                unsigned_four_byte tempHLLocation = registers.getRegisterDouble(registerID::H, registerID::L);
                registers.setRegister(registerID::HL, memory->readMemory(tempHLLocation));
                ldXY(registerID::H, currentOpcodeLow);
            }
            else if (currentOpcodeLow == 0xE)
            {
                unsigned_four_byte tempHLLocation = registers.getRegisterDouble(registerID::H, registerID::L);
                registers.setRegister(registerID::HL, memory->readMemory(tempHLLocation));
                ldXY(registerID::L, currentOpcodeLow - 8);
            }
            else if (currentOpcodeLow < 8)
            {
                ldXY(registerID::H, currentOpcodeLow);
            }
            else
            {
                ldXY(registerID::L, currentOpcodeLow - 8);
            }
            break;
        case 0x7:
            if (currentOpcodeLow == 0x6)
            {
                // This is the Halt Bug. Causes the next instruction to repeat.
                if (ime == 0 && (memory->readMemory(0xFFFF) & memory->readMemory(0xFF0F)) != 0)
                {
                    tickClock(4);
                    unsigned_four_byte nextPC = programCounter;
                    execute();
                    programCounter = nextPC;
                }
                else
                {
                    halt = true;
                }

                // This is true Halt Behavior.
                while (halt)
                {
                    if ((memory->readMemory(0xFFFF) & memory->readMemory(0xFF0F)) != 0)
                    {
                        halt = false;
                    }
                    tickClock(4);
                }
            }
            else if (currentOpcodeLow == 0xE)
            {
                unsigned_four_byte tempHLLocation = registers.getRegisterDouble(registerID::H, registerID::L);
                registers.setRegister(registerID::HL, memory->readMemory(tempHLLocation));
                ldXY(registerID::A, currentOpcodeLow - 8);
            }
            else if (currentOpcodeLow < 8)
            {
                unsigned_four_byte tempHLLocation = registers.getRegisterDouble(registerID::H, registerID::L);
                registers.setRegister(registerID::HL, memory->readMemory(tempHLLocation));
                registers.setRegister(registerID::HL, registers.getRegister(currentOpcodeLow));
                memory->writeMemory(tempHLLocation, registers.getRegister(registerID::HL));
                tickClock(8);
            }
            else
            {
                ldXY(registerID::A, currentOpcodeLow - 8);
            }
            break;
        case 0x8:
            if (currentOpcodeLow == 0x6)
            {
                unsigned_four_byte tempHLLocation = registers.getRegisterDouble(registerID::H, registerID::L);
                registers.setRegister(registerID::HL, memory->readMemory(tempHLLocation));
                registers.addA(registers.getRegister(currentOpcodeLow));
                tickClock(8);
            }
            else if (currentOpcodeLow == 0xE)
            {
                unsigned_four_byte tempHLLocation = registers.getRegisterDouble(registerID::H, registerID::L);
                registers.setRegister(registerID::HL, memory->readMemory(tempHLLocation));
                registers.adcA(registers.getRegister(currentOpcodeLow - 8));
                tickClock(8);
            }
            else if (currentOpcodeLow < 8)
            {
                registers.addA(registers.getRegister(currentOpcodeLow));
                tickClock(4);
            }
            else
            {
                registers.adcA(registers.getRegister(currentOpcodeLow - 8));
                tickClock(4);
            }
            break;
        case 0x9:
            if (currentOpcodeLow == 0x6)
            {
                unsigned_four_byte tempHLLocation = registers.getRegisterDouble(registerID::H, registerID::L);
                registers.setRegister(registerID::HL, memory->readMemory(tempHLLocation));
                registers.subA(registers.getRegister(currentOpcodeLow));
                tickClock(8);
            }
            else if (currentOpcodeLow == 0xE)
            {
                unsigned_four_byte tempHLLocation = registers.getRegisterDouble(registerID::H, registerID::L);
                registers.setRegister(registerID::HL, memory->readMemory(tempHLLocation));
                registers.sbcA(registers.getRegister(currentOpcodeLow - 8));
                tickClock(8);
            }
            else if (currentOpcodeLow < 8)
            {
                registers.subA(registers.getRegister(currentOpcodeLow));
                tickClock(4);
            }
            else
            {
                registers.sbcA(registers.getRegister(currentOpcodeLow - 8));
                tickClock(4);
            }
            break;
        case 0xA:
            if (currentOpcodeLow == 0x6)
            {
                unsigned_four_byte tempHLLocation = registers.getRegisterDouble(registerID::H, registerID::L);
                registers.setRegister(registerID::HL, memory->readMemory(tempHLLocation));
                registers.andA(registers.getRegister(currentOpcodeLow));
                tickClock(8);
            }
            else if (currentOpcodeLow == 0xE)
            {
                unsigned_four_byte tempHLLocation = registers.getRegisterDouble(registerID::H, registerID::L);
                registers.setRegister(registerID::HL, memory->readMemory(tempHLLocation));
                registers.xorA(registers.getRegister(currentOpcodeLow - 8));
                tickClock(8);
            }
            else if (currentOpcodeLow < 8)
            {
                registers.andA(registers.getRegister(currentOpcodeLow));
                tickClock(4);
            }
            else
            {
                registers.xorA(registers.getRegister(currentOpcodeLow - 8));
                tickClock(4);
            }
            break;
        case 0xB:
            if (currentOpcodeLow == 0x6)
            {
                unsigned_four_byte tempHLLocation = registers.getRegisterDouble(registerID::H, registerID::L);
                registers.setRegister(registerID::HL, memory->readMemory(tempHLLocation));
                registers.orA(registers.getRegister(currentOpcodeLow));
                tickClock(8);
            }
            else if (currentOpcodeLow == 0xE)
            {
                unsigned_four_byte tempHLLocation = registers.getRegisterDouble(registerID::H, registerID::L);
                registers.setRegister(registerID::HL, memory->readMemory(tempHLLocation));
                registers.cpA(registers.getRegister(currentOpcodeLow - 8));
                tickClock(8);
            }
            else if (currentOpcodeLow < 8)
            {
                registers.orA(registers.getRegister(currentOpcodeLow));
                tickClock(4);
            }
            else
            {
                registers.cpA(registers.getRegister(currentOpcodeLow - 8));
                tickClock(4);
            }
            break;
        case 0xC:
            switch (currentOpcodeLow)
            {
            case 0x0:
                returnConditional(!registers.getFlag(7));
                break;
            case 0x1:
                pop(registerID::B, registerID::C);
                break;
            case 0x2:
                jumpConditional(!registers.getFlag(7));
                break;
            case 0x3:
                jumpConditional(true);
                break;
            case 0x4:
                callConditional(!registers.getFlag(7));
                break;
            case 0x5:
                push(registerID::B, registerID::C);
                break;
            case 0x6:
                registers.addA(fetch());
                tickClock(8);
                break;
            case 0x7:
                rst(0x00);
                break;
            case 0x8:
                returnConditional(registers.getFlag(7));
                break;
            case 0x9:
            {
                unsigned_two_byte currentOpcodeLow = memory->readMemory(stackPointer);
                stackPointer++;
                unsigned_two_byte high = memory->readMemory(stackPointer);
                stackPointer++;
                programCounter = (high << 8) | currentOpcodeLow;
                tickClock(16);
                break;
            }
            case 0xA:
                jumpConditional(registers.getFlag(7));
                break;
            case 0xC:
                callConditional(registers.getFlag(7));
                break;
            case 0xD:
               callConditional(true);
                break;
            case 0xE:
                registers.adcA(fetch());
                tickClock(8);
                break;
            case 0xF:
                rst(0x08);
                break;
            }
            break;
        case 0xD:
            switch (currentOpcodeLow)
            {
            case 0x0:
                returnConditional(!registers.getFlag(4));
                break;
            case 0x1:
                pop(registerID::D, registerID::E);
                break;
            case 0x2:
                jumpConditional(!registers.getFlag(4));
                break;
            case 0x4:
                callConditional(!registers.getFlag(4));
                break;
            case 0x5:
                push(registerID::D, registerID::E);
                break;
            case 0x6:
                registers.subA(fetch());
                tickClock(8);
                break;
            case 0x7:
                rst(0x10);
                break;
            case 0x8:
                returnConditional(registers.getFlag(4));
                break;
            case 0x9:
            {
                unsigned_two_byte currentOpcodeLow = memory->readMemory(stackPointer);
                stackPointer++;
                unsigned_two_byte high = memory->readMemory(stackPointer);
                stackPointer++;
                programCounter = (high << 8) | currentOpcodeLow;
                memory->writeMemory(0xFFFF, 1);
                tickClock(16);
                break;
            }
            case 0xA:
                jumpConditional(registers.getFlag(4));
                break;
            case 0xC:
                callConditional(registers.getFlag(4));
                break;
            case 0xE:
                registers.sbcA(fetch());
                tickClock(8);
                break;
            case 0xF:
                rst(0x18);
                break;
            }
            break;
        case 0xE:
            switch (currentOpcodeLow)
            {
            case 0x0:
                memory->writeMemory((0xFF00 + fetch()), registers.getRegister(registerID::A));
                tickClock(12);
                break;
            case 0x1:
                pop(registerID::H, registerID::L);
                break;
            case 0x2:
                memory->writeMemory((0xFF00 + registers.getRegister(registerID::C)), registers.getRegister(registerID::A));
                tickClock(12);
                break;
            case 0x5:
                push(registerID::H, registerID::L);
                break;
            case 0x6:
                registers.andA(fetch());
                tickClock(8);
                break;
            case 0x7:
                rst(0x20);
                break;
            case 0x8:
            {
                unsigned_two_byte value = fetch();
                signed_two_byte signedValue = static_cast<signed_two_byte>(value);
                unsigned_four_byte oldStack = stackPointer;
                stackPointer = oldStack + signedValue;
                registers.assignHalfcarryAdd(oldStack & 0xFF, value);
                registers.assignCarry(oldStack & 0xFF, value);
                registers.clearFlag(7);
                registers.clearFlag(6);
                tickClock(16);
                break;
            }
            case 0x9:
                programCounter = registers.getRegisterDouble(registerID::H, registerID::L);
                tickClock(4);
                break;
            case 0xA:
            {
                unsigned_two_byte currentOpcodeLow = fetch();
                unsigned_two_byte high = fetch();
                memory->writeMemory(((high << 8) | currentOpcodeLow), registers.getRegister(registerID::A));
                tickClock(16);
                break;
            }
            case 0xE:
                registers.xorA(fetch());
                tickClock(8);
                break;
            case 0xF:
                rst(0x28);
                break;
            }
            break;
        case 0xF:
            switch (currentOpcodeLow)
            {
            case 0x0:
                registers.setRegister(registerID::A, memory->readMemory(0xFF00 + fetch()));
                tickClock(12);
                break;
            case 0x1:
                pop(registerID::A, registerID::F);
                break;
            case 0x2:
                registers.setRegister(registerID::A, memory->readMemory(0xFF00 + registers.getRegister(registerID::C)));
                tickClock(8);
                break;
            case 0x3:
                ime = 0;
                break;
            case 0x5:
                push(registerID::A, registerID::F);
                tickClock(4);
                break;
            case 0x6:
                registers.orA(fetch());
                tickClock(8);
                break;
            case 0x7:
                rst(0x30);
                break;
            case 0x8:
            {
                unsigned_two_byte value = fetch();
                signed_two_byte signedValue = static_cast<signed_two_byte>(value);
                unsigned_four_byte newValue = stackPointer + signedValue;
                registers.setRegisterDouble(registerID::H, registerID::L, newValue >> 8, newValue & 0xFF);
                registers.assignHalfcarryAdd(stackPointer & 0xFF, value);
                registers.assignCarry(stackPointer & 0xFF, value);
                registers.clearFlag(7);
                registers.clearFlag(6);
                tickClock(16);
                break;
            }

            case 0x9:
                stackPointer = registers.getRegisterDouble(registerID::H, registerID::L);
                tickClock(8);
                break;
            case 0xA:
            {
                unsigned_two_byte currentOpcodeLow = fetch();
                unsigned_two_byte high = fetch();
                unsigned_two_byte value = memory->readMemory((high << 8) | currentOpcodeLow);
                registers.setRegister(registerID::A, value);
                tickClock(16);
                break;
            }
            case 0xB:
                ime = 1;
                break;
            case 0xE:
                registers.cpA(fetch());
                tickClock(8);
                break;
            case 0xF:
                rst(0x38);
                break;
            }
            break;
        }
    }
    else
    {
        currentOpcode = fetch();
        currentOpcodeHigh = currentOpcode >> 4;
        currentOpcodeLow = currentOpcode & 0xF;
        switch (currentOpcodeHigh)
        {
        case 0x0:
            if (currentOpcodeLow == 0x6)
            {
                unsigned_four_byte tempHLLocation = registers.getRegisterDouble(registerID::H, registerID::L);
                registers.setRegister(registerID::HL, memory->readMemory(tempHLLocation));
                registers.rotateLeftCircular(currentOpcodeLow);
                memory->writeMemory(tempHLLocation, registers.getRegister(registerID::HL));
                tickClock(16);
            }
            else if (currentOpcodeLow == 0xE)
            {
                unsigned_four_byte tempHLLocation = registers.getRegisterDouble(registerID::H, registerID::L);
                registers.setRegister(registerID::HL, memory->readMemory(tempHLLocation));
                registers.rotateRightCircular(currentOpcodeLow - 8);
                memory->writeMemory(tempHLLocation, registers.getRegister(registerID::HL));
                tickClock(16);
            }
            else if (currentOpcodeLow < 8)
            {
                registers.rotateLeftCircular(currentOpcodeLow);
                tickClock(8);
            }
            else
            {
                registers.rotateRightCircular(currentOpcodeLow - 8);
                tickClock(8);
            }
            break;
        case 0x1:
            if (currentOpcodeLow == 0x6)
            {
                unsigned_four_byte tempHLLocation = registers.getRegisterDouble(registerID::H, registerID::L);
                registers.setRegister(registerID::HL, memory->readMemory(tempHLLocation));
                registers.rotateLeft(currentOpcodeLow);
                memory->writeMemory(tempHLLocation, registers.getRegister(registerID::HL));
                tickClock(16);
            }
            else if (currentOpcodeLow == 0xE)
            {
                unsigned_four_byte tempHLLocation = registers.getRegisterDouble(registerID::H, registerID::L);
                registers.setRegister(registerID::HL, memory->readMemory(tempHLLocation));
                registers.rotateRight(currentOpcodeLow - 8);
                memory->writeMemory(tempHLLocation, registers.getRegister(registerID::HL));
                tickClock(16);
            }
            else if (currentOpcodeLow < 8)
            {
                registers.rotateLeft(currentOpcodeLow);
                tickClock(8);
            }
            else
            {
                registers.rotateRight(currentOpcodeLow - 8);
                tickClock(8);
            }
            break;
        case 0x2:
            if (currentOpcodeLow == 0x6)
            {
                unsigned_four_byte tempHLLocation = registers.getRegisterDouble(registerID::H, registerID::L);
                registers.setRegister(registerID::HL, memory->readMemory(tempHLLocation));
                registers.SLA(currentOpcodeLow);
                memory->writeMemory(tempHLLocation, registers.getRegister(registerID::HL));
                tickClock(16);
            }
            else if (currentOpcodeLow == 0xE)
            {
                unsigned_four_byte tempHLLocation = registers.getRegisterDouble(registerID::H, registerID::L);
                registers.setRegister(registerID::HL, memory->readMemory(tempHLLocation));
                registers.SRA(currentOpcodeLow - 8);
                memory->writeMemory(tempHLLocation, registers.getRegister(registerID::HL));
                tickClock(16);
            }
            else if (currentOpcodeLow < 8)
            {
                registers.SLA(currentOpcodeLow);
                tickClock(8);
            }
            else
            {
                registers.SRA(currentOpcodeLow - 8);
                tickClock(8);
            }
            break;
        case 0x3:
            if (currentOpcodeLow == 0x6)
            {
                unsigned_four_byte tempHLLocation = registers.getRegisterDouble(registerID::H, registerID::L);
                registers.setRegister(registerID::HL, memory->readMemory(tempHLLocation));
                registers.swap(currentOpcodeLow);
                memory->writeMemory(tempHLLocation, registers.getRegister(registerID::HL));
                tickClock(16);
            }
            else if (currentOpcodeLow == 0xE)
            {
                unsigned_four_byte tempHLLocation = registers.getRegisterDouble(registerID::H, registerID::L);
                registers.setRegister(registerID::HL, memory->readMemory(tempHLLocation));
                registers.SRL(currentOpcodeLow - 8);
                memory->writeMemory(tempHLLocation, registers.getRegister(registerID::HL));
                tickClock(16);
            }
            else if (currentOpcodeLow < 8)
            {
                registers.swap(currentOpcodeLow);
                tickClock(8);
            }
            else
            {
                registers.SRL(currentOpcodeLow - 8);
                tickClock(8);
            }
            break;
        case 0x4:
            if (currentOpcodeLow == 0x6)
            {
                unsigned_four_byte tempHLLocation = registers.getRegisterDouble(registerID::H, registerID::L);
                registers.setRegister(registerID::HL, memory->readMemory(tempHLLocation));
                bit(0, currentOpcodeLow);
            }
            else if (currentOpcodeLow == 0xE)
            {
                unsigned_four_byte tempHLLocation = registers.getRegisterDouble(registerID::H, registerID::L);
                registers.setRegister(registerID::HL, memory->readMemory(tempHLLocation));
                bit(1, currentOpcodeLow - 8);
            }
            else if (currentOpcodeLow < 8)
            {
                bit(0, currentOpcodeLow);
            }
            else
            {
                bit(1, currentOpcodeLow - 8);
            }
            break;
        case 0x5:
            if (currentOpcodeLow == 0x6)
            {
                unsigned_four_byte tempHLLocation = registers.getRegisterDouble(registerID::H, registerID::L);
                registers.setRegister(registerID::HL, memory->readMemory(tempHLLocation));
                bit(2, currentOpcodeLow);
            }
            else if (currentOpcodeLow == 0xE)
            {
                unsigned_four_byte tempHLLocation = registers.getRegisterDouble(registerID::H, registerID::L);
                registers.setRegister(registerID::HL, memory->readMemory(tempHLLocation));
                bit(3, currentOpcodeLow - 8);
            }
            else if (currentOpcodeLow < 8)
            {
                bit(2, currentOpcodeLow);
            }
            else
            {
                bit(3, currentOpcodeLow - 8);
            }
            break;
        case 0x6:
            if (currentOpcodeLow == 0x6)
            {
                unsigned_four_byte tempHLLocation = registers.getRegisterDouble(registerID::H, registerID::L);
                registers.setRegister(registerID::HL, memory->readMemory(tempHLLocation));
                bit(4, currentOpcodeLow);
            }
            else if (currentOpcodeLow == 0xE)
            {
                unsigned_four_byte tempHLLocation = registers.getRegisterDouble(registerID::H, registerID::L);
                registers.setRegister(registerID::HL, memory->readMemory(tempHLLocation));
                bit(5, currentOpcodeLow - 8);
            }
            else if (currentOpcodeLow < 8)
            {
                bit(4, currentOpcodeLow);
            }
            else
            {
                bit(5, currentOpcodeLow - 8);
            }
            break;
        case 0x7:
            if (currentOpcodeLow == 0x6)
            {
                unsigned_four_byte tempHLLocation = registers.getRegisterDouble(registerID::H, registerID::L);
                registers.setRegister(registerID::HL, memory->readMemory(tempHLLocation));
                bit(6, currentOpcodeLow);
            }
            else if (currentOpcodeLow == 0xE)
            {
                unsigned_four_byte tempHLLocation = registers.getRegisterDouble(registerID::H, registerID::L);
                registers.setRegister(registerID::HL, memory->readMemory(tempHLLocation));
                bit(7, currentOpcodeLow - 8);
            }
            else if (currentOpcodeLow < 8)
            {
                bit(6, currentOpcodeLow);
            }
            else
            {
                bit(7, currentOpcodeLow - 8);
            }
            break;
        case 0x8:
            if (currentOpcodeLow == 0x6)
            {
                unsigned_four_byte tempHLLocation = registers.getRegisterDouble(registerID::H, registerID::L);
                registers.setRegister(registerID::HL, memory->readMemory(tempHLLocation));
                res(0, currentOpcodeLow);
                memory->writeMemory(tempHLLocation, registers.getRegister(registerID::HL));
            }
            else if (currentOpcodeLow == 0xE)
            {
                unsigned_four_byte tempHLLocation = registers.getRegisterDouble(registerID::H, registerID::L);
                registers.setRegister(registerID::HL, memory->readMemory(tempHLLocation));
                res(1, currentOpcodeLow - 8);
                memory->writeMemory(tempHLLocation, registers.getRegister(registerID::HL));
            }
            else if (currentOpcodeLow < 8)
            {
                res(0, currentOpcodeLow);
            }
            else
            {
                res(1, currentOpcodeLow - 8);
            }
            break;
        case 0x9:
            if (currentOpcodeLow == 0x6)
            {
                unsigned_four_byte tempHLLocation = registers.getRegisterDouble(registerID::H, registerID::L);
                registers.setRegister(registerID::HL, memory->readMemory(tempHLLocation));
                res(2, currentOpcodeLow);
                memory->writeMemory(tempHLLocation, registers.getRegister(registerID::HL));
            }
            else if (currentOpcodeLow == 0xE)
            {
                unsigned_four_byte tempHLLocation = registers.getRegisterDouble(registerID::H, registerID::L);
                registers.setRegister(registerID::HL, memory->readMemory(tempHLLocation));
                res(3, currentOpcodeLow - 8);
                memory->writeMemory(tempHLLocation, registers.getRegister(registerID::HL));
            }
            else if (currentOpcodeLow < 8)
            {
                res(2, currentOpcodeLow);
            }
            else
            {
                res(3, currentOpcodeLow - 8);
            }
            break;
        case 0xA:
            if (currentOpcodeLow == 0x6)
            {
                unsigned_four_byte tempHLLocation = registers.getRegisterDouble(registerID::H, registerID::L);
                registers.setRegister(registerID::HL, memory->readMemory(tempHLLocation));
                res(4, currentOpcodeLow);
                memory->writeMemory(tempHLLocation, registers.getRegister(registerID::HL));
            }
            else if (currentOpcodeLow == 0xE)
            {
                unsigned_four_byte tempHLLocation = registers.getRegisterDouble(registerID::H, registerID::L);
                registers.setRegister(registerID::HL, memory->readMemory(tempHLLocation));
                res(5, currentOpcodeLow - 8);
                memory->writeMemory(tempHLLocation, registers.getRegister(registerID::HL));
            }
            else if (currentOpcodeLow < 8)
            {
                res(4, currentOpcodeLow);
            }
            else
            {
                res(5, currentOpcodeLow - 8);
            }
            break;
        case 0xB:
            if (currentOpcodeLow == 0x6)
            {
                unsigned_four_byte tempHLLocation = registers.getRegisterDouble(registerID::H, registerID::L);
                registers.setRegister(registerID::HL, memory->readMemory(tempHLLocation));
                res(6, currentOpcodeLow);
                memory->writeMemory(tempHLLocation, registers.getRegister(registerID::HL));
            }
            else if (currentOpcodeLow == 0xE)
            {
                unsigned_four_byte tempHLLocation = registers.getRegisterDouble(registerID::H, registerID::L);
                registers.setRegister(registerID::HL, memory->readMemory(tempHLLocation));
                res(7, currentOpcodeLow - 8);
                memory->writeMemory(tempHLLocation, registers.getRegister(registerID::HL));
            }
            else if (currentOpcodeLow < 8)
            {
                res(6, currentOpcodeLow);
            }
            else
            {
                res(7, currentOpcodeLow - 8);
            }
            break;
        case 0xC:
            if (currentOpcodeLow == 0x6)
            {
                unsigned_four_byte tempHLLocation = registers.getRegisterDouble(registerID::H, registerID::L);
                registers.setRegister(registerID::HL, memory->readMemory(tempHLLocation));
                set(0, currentOpcodeLow);
                memory->writeMemory(tempHLLocation, registers.getRegister(registerID::HL));
            }
            else if (currentOpcodeLow == 0xE)
            {
                unsigned_four_byte tempHLLocation = registers.getRegisterDouble(registerID::H, registerID::L);
                registers.setRegister(registerID::HL, memory->readMemory(tempHLLocation));
                set(1, currentOpcodeLow - 8);
                memory->writeMemory(tempHLLocation, registers.getRegister(registerID::HL));
            }
            else if (currentOpcodeLow < 8)
            {
                set(0, currentOpcodeLow);
            }
            else
            {
                set(1, currentOpcodeLow - 8);
            }
            break;
        case 0xD:
            if (currentOpcodeLow == 0x6)
            {
                unsigned_four_byte tempHLLocation = registers.getRegisterDouble(registerID::H, registerID::L);
                registers.setRegister(registerID::HL, memory->readMemory(tempHLLocation));
                set(2, currentOpcodeLow);
                memory->writeMemory(tempHLLocation, registers.getRegister(registerID::HL));
            }
            else if (currentOpcodeLow == 0xE)
            {
                unsigned_four_byte tempHLLocation = registers.getRegisterDouble(registerID::H, registerID::L);
                registers.setRegister(registerID::HL, memory->readMemory(tempHLLocation));
                set(3, currentOpcodeLow - 8);
                memory->writeMemory(tempHLLocation, registers.getRegister(registerID::HL));
            }
            else if (currentOpcodeLow < 8)
            {
                set(2, currentOpcodeLow);
            }
            else
            {
                set(3, currentOpcodeLow - 8);
            }
            break;
        case 0xE:
            if (currentOpcodeLow == 0x6)
            {
                unsigned_four_byte tempHLLocation = registers.getRegisterDouble(registerID::H, registerID::L);
                registers.setRegister(registerID::HL, memory->readMemory(tempHLLocation));
                set(4, currentOpcodeLow);
                memory->writeMemory(tempHLLocation, registers.getRegister(registerID::HL));
            }
            else if (currentOpcodeLow == 0xE)
            {
                unsigned_four_byte tempHLLocation = registers.getRegisterDouble(registerID::H, registerID::L);
                registers.setRegister(registerID::HL, memory->readMemory(tempHLLocation));
                set(5, currentOpcodeLow - 8);
                memory->writeMemory(tempHLLocation, registers.getRegister(registerID::HL));
            }
            else if (currentOpcodeLow < 8)
            {
                set(4, currentOpcodeLow);
            }
            else
            {
                set(5, currentOpcodeLow - 8);
            }
            break;
        case 0xF:
            if (currentOpcodeLow == 0x6)
            {
                unsigned_four_byte tempHLLocation = registers.getRegisterDouble(registerID::H, registerID::L);
                registers.setRegister(registerID::HL, memory->readMemory(tempHLLocation));
                set(6, currentOpcodeLow);
                memory->writeMemory(tempHLLocation, registers.getRegister(registerID::HL));
            }
            else if (currentOpcodeLow == 0xE)
            {
                unsigned_four_byte tempHLLocation = registers.getRegisterDouble(registerID::H, registerID::L);
                registers.setRegister(registerID::HL, memory->readMemory(tempHLLocation));
                set(7, currentOpcodeLow - 8);
                memory->writeMemory(tempHLLocation, registers.getRegister(registerID::HL));
            }
            else if (currentOpcodeLow < 8)
            {
                set(6, currentOpcodeLow);
            }
            else
            {
                set(7, currentOpcodeLow - 8);
            }
            break;
        }
    }
}

void Cpu::interrupt()
{
    if (ime && memory->readMemory(0xFFFF))
    {
        if (memory->readMemory(0xFFFF) & 1 && memory->readMemory(0xFF0F) & 1)
        {
            interruptJump(0x40, 0);
        }
        if (memory->readMemory(0xFFFF) & 2 && memory->readMemory(0xFF0F) & 2)
        {
            interruptJump(0x48, 1);
        }
        if (memory->readMemory(0xFFFF) & 4 && memory->readMemory(0xFF0F) & 4)
        {
            interruptJump(0x50, 2);
        }
        if (memory->readMemory(0xFFFF) & 8 && memory->readMemory(0xFF0F) & 8)
        {
            interruptJump(0x58, 3);
        }
        if (memory->readMemory(0xFFFF) & 16 && memory->readMemory(0xFF0F) & 16)
        {
            interruptJump(0x60, 4);
        }
    }
}

void Cpu::tickClock(int cycles)
{
    const unsigned_four_byte ioMemoryStart = 0xFF00;
    for (int i = 0; i < cycles; i++)
    {
        if (i % 2 == 1)
        {
            ppu->cycle();
        }

        // DIV Timer
        sysClock++;
        unsigned_four_byte upperSysClock = sysClock >> 8;
        if (memory->readMemory(ioMemoryStart + 0x4) != upperSysClock)
        {
            memory->setDiv(upperSysClock);

            // TIMA Timer
            int bit = 0;
            switch (memory->readMemory(ioMemoryStart + 0x7) & 0x3)
            { // determines the bit to check against in DIV
            case 0:
                bit = 9;
                break;
            case 1:
                bit = 3;
                break;
            case 2:
                bit = 5;
                break;
            case 3:
                bit = 7;
                break;
            }
            unsigned_two_byte timerEnable = (memory->readMemory(0xFF07) & 0x4) >> 2;
            unsigned_four_byte andResultPrevious = (((sysClock - 1) >> bit) & 1) & timerEnable;
            unsigned_four_byte andResult = ((sysClock >> bit) & 1) & timerEnable;
            unsigned_two_byte TIMA = 0;
            if (andResultPrevious == 1 && andResult == 0)
            {
                TIMA = memory->readMemory(ioMemoryStart + 0x5);
                TIMA++;
                if (TIMA > 0xFF)
                { // if TIMA overflows past 0xFF, request TIMA Interrupt and reset value to TIMA Modulo (0xFF07)
                    memory->writeMemory(0xFF05, memory->readMemory(ioMemoryStart + 0x6));
                    memory->writeMemory(0xFF0F, memory->readMemory(ioMemoryStart + 0xF) | (1 << 2)); // Request Interrupt
                }
                else
                {
                    memory->writeMemory(ioMemoryStart + 0x5, TIMA);
                }
            }
        }
    }
}

void Cpu::interruptJump(unsigned_four_byte location, int flag)
{
    stackPointer--;
    memory->writeMemory(stackPointer, (programCounter >> 8));
    stackPointer--;

    memory->writeMemory(stackPointer, (programCounter & 0xff));
    programCounter = location;
    memory->writeMemory(0xFF0F, memory->readMemory(0xFF0F) & ~(1 << flag));
    tickClock(20);
}
