#include "Bus.h"

Bus::Bus()
{
    ram_.fill(0);
}

uint8_t Bus::Read(uint16_t address) const
{
    return ram_[address];
}

void Bus::Write(uint16_t address, uint8_t value)
{
    ram_[address] = value;
}