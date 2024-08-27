#include "Controls.hxx"

void Controls::setMemory(Memory* memory)
{
    this->memory = memory;
}

void Controls::pressButton(int buttonCode)
{
    switch (buttonCode) {
    case 0:
        up = 0;
        break;
    case 1:
        down = 0;
        break;
    case 2:
        left = 0;
        break;
    case 3:
        right = 0;
        break;
    case 4:
        a = 0;
        break;
    case 5:
        b = 0;
        break;
    case 6:
        start = 0;
        break;
    case 7:
        select = 0;
        break;
    default:
        break;
    }
}

void Controls::releaseButton(int buttonCode)
{
    switch (buttonCode) {
    case 0:
        up = 1;
        break;
    case 1:
        down = 1;
        break;
    case 2:
        left = 1;
        break;
    case 3:
        right = 1;
        break;
    case 4:
        a = 1;
        break;
    case 5:
        b = 1;
        break;
    case 6:
        start = 1;
        break;
    case 7:
        select = 1;
        break;
    default:
        break;
    }
}

void Controls::updateInputState()
{
    unsigned_two_byte selectAction = (memory->readMemory(0xFF00) >> 5) & 1;
    unsigned_two_byte selectDirection = (memory->readMemory(0xFF00) >> 4) & 1;

    if (!selectAction) {
        unsigned_two_byte low = (start << 3) | (select << 2) | (b << 1) | a;
        unsigned_two_byte high = memory->readMemory(0xFF00) >> 4;
        memory->writeMemory(0xFF00, (high << 4) | low);
    }
    else if (!selectDirection) {
        unsigned_two_byte low = (down << 3) | (up << 2) | (left << 1) | right;
        unsigned_two_byte high = memory->readMemory(0xFF00) >> 4;
        memory->writeMemory(0xFF00, (high << 4) | low);
    }
}

