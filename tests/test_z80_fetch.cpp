#include <iostream>
#include "Bus.h"
#include "Z80.h"

static int g_failures = 0;

// filepath: test_z80_fetch.cpp
// ...existing code...
#define ASSERT_EQ(actual, expected) \
    do { \
        const auto _actual = (actual); \
        const auto _expected = (expected); \
        if (_actual != _expected) { \
            std::cerr << "ASSERT_EQ failed at " << __FILE__ << ":" << __LINE__ \
                      << " actual=" << +_actual << " expected=" << +_expected << "\n"; \
            ++g_failures; \
        } \
    } while (0)
// ...existing code...

int main()
{
    Bus bus;
    Z80 cpu(bus);

    cpu.reset();
    cpu.set_pc(0x8000);

    bus.write(0x8000, 0xAA);
    bus.write(0x8001, 0xBB);

    const uint8_t a = cpu.fetch8();
    ASSERT_EQ(a, 0xAA);
    ASSERT_EQ(cpu.pc(), 0x8001);

    const uint8_t b = cpu.fetch8();
    ASSERT_EQ(b, 0xBB);
    ASSERT_EQ(cpu.pc(), 0x8002);

    if (g_failures == 0) {
        std::cout << "test_z80_fetch: PASS\n";
        return 0;
    } else {
        std::cout << "test_z80_fetch: FAIL (" << g_failures << ")\n";
        return 1;
    }
}