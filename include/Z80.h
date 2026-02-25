#pragma once

#include <cstdint>

class Bus;

class Z80
{
public:
    explicit Z80(Bus& bus);

    void Reset();

    // For now: fetch 1 byte at PC and advance PC.
    // Returns the fetched byte (not executing yet).
    std::uint8_t Fetch8();

    // Register access (keep it simple for now)
    std::uint16_t Pc() const { return PC; }
    std::uint16_t Sp() const { return SP; }

    void SetPc(uint16_t v) { PC = v; }
    void SetSp(uint16_t v) { SP = v; }

private:
    Bus& bus_;

    // Minimal registers to start
    std::uint16_t PC = 0x0000;
    std::uint16_t SP = 0x0000;

    // Later we’ll add AF, BC, DE, HL, etc.
};