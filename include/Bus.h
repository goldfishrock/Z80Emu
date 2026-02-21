#pragma once
#include <array>
#include <cstdint>
#include <cstddef>

class Bus
{
public:
    static constexpr std::size_t RAM_SIZE = 65536;

    Bus();

    uint8_t read(uint16_t address) const;
    void write(uint16_t address, uint8_t value);

private:
    std::array<uint8_t, RAM_SIZE> ram_;
};