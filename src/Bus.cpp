#include "Bus.h"

Bus::Bus()
{
    ram_.fill(0);
}

uint8_t Bus::read(uint16_t address) const
{
    return ram_[address];
}

void Bus::write(uint16_t address, uint8_t value)
{
    ram_[address] = value;
}