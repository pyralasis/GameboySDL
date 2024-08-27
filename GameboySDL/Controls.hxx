#pragma once
#include "Memory.hxx"
#include "Types.hxx"

class Controls
{
private:
	unsigned_two_byte up = 1;
	unsigned_two_byte down = 1;
	unsigned_two_byte left = 1;
	unsigned_two_byte right = 1;
	unsigned_two_byte a = 1;
	unsigned_two_byte b = 1;
	unsigned_two_byte start = 1;
	unsigned_two_byte select = 1;

	Memory* memory = nullptr;

public:
	void setMemory(Memory* memory);
	void pressButton(int buttonCode);
	void releaseButton(int buttonCode);
	void updateInputState();
};