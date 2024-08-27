#pragma once
#include <iomanip>
#include <iostream>

typedef short signed_four_byte;
typedef unsigned short unsigned_four_byte;
typedef char signed_two_byte;
typedef unsigned char unsigned_two_byte;

enum registerID {
    B,
    C,
    D,
    E,
    H,
    L,
    HL,
    A,
    F
};

#define formatter2 std::uppercase << std::setfill('0') << std::setw(2) << std::hex
#define formatter4 std::uppercase << std::setfill('0') << std::setw(4) << std::hex