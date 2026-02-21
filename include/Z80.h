#pragma once

#include <cstdint>

class Bus;

class Z80
{
public:
    explicit Z80(Bus& bus);

    void reset();

    // For now: fetch 1 byte at PC and advance PC.
    // Returns the fetched byte (not executing yet).
    uint8_t fetch8();

    // Register access (keep it simple for now)
    uint16_t pc() const { return PC; }
    uint16_t sp() const { return SP; }

    void set_pc(uint16_t v) { PC = v; }
    void set_sp(uint16_t v) { SP = v; }

private:
    Bus& bus_;

    // Minimal registers to start
    uint16_t PC = 0x0000;
    uint16_t SP = 0x0000;

    // Later we’ll add AF, BC, DE, HL, etc.
};