#include "Z80.h"
#include "Bus.h"

Z80::Z80(Bus& bus) : bus_(bus)
{
}

void Z80::reset()
{
    PC = 0x0000;
    SP = 0x0000;
}

uint8_t Z80::fetch8()
{
    const uint8_t value = bus_.read(PC);
    PC = static_cast<uint16_t>(PC + 1);
    return value;
}