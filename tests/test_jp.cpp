#include <catch2/catch_test_macros.hpp>
#include "Cpu.h"
#include "Bus.h"

struct CpuFixture
{
    Bus bus;
    Cpu cpu;

    CpuFixture()
    {
        cpu.Connect(&bus);
        cpu.Reset();
    }
};

// **********************************************
// *        JP nn      ::    OP CODE: 0xC3      *
// **********************************************
// *                                            *
// *       Sets PC to immediate address         *
// *                                            *
// **********************************************
TEST_CASE_METHOD(CpuFixture, "JP nn (0xC3) sets PC to immediate address", "[flow][jp]")
{
    // Arrange: JP 0x1234
    bus.Write(0x0000, 0xC3); // JP nn
    bus.Write(0x0001, 0x34); // low
    bus.Write(0x0002, 0x12); // high

    cpu.SetPc(0x0000);       // or however you set PC in your tests

    // Act
    cpu.Step();              // or ExecuteInstruction()/Tick()/RunOne(), whatever your API is

    // Assert
    REQUIRE(cpu.GetPc() == 0x1234);
}

// **********************************************
// *        JP nn      ::    OP CODE: 0xC3      *
// **********************************************
// *                                            *
// *       Uses Little-Endian Immediate         *
// *                                            *
// **********************************************
TEST_CASE_METHOD(CpuFixture, "JP nn (0xC3) uses little-endian immediate", "[flow][jp]")
{
    // JP 0xABCD
    bus.Write(0x0000, 0xC3);
    bus.Write(0x0001, 0xCD);
    bus.Write(0x0002, 0xAB);

    cpu.SetPc(0x0000);

    cpu.Step();

    REQUIRE(cpu.GetPc() == 0xABCD);
}

// **********************************************
// *        JP (HL)    ::    OP CODE: 0xE9      *
// **********************************************
// *                                            *
// *              PC <- HL                      *
// *                                            *
// **********************************************
TEST_CASE_METHOD(CpuFixture, "JP (HL) (0xE9) sets PC to HL", "[flow][jp][hl]")
{
    // Arrange
    bus.Write(0x0000, 0xE9);   // JP (HL)
    cpu.SetPc(0x0000);
    cpu.SetHl(0x3456);

    // Act
    cpu.Step();

    // Assert
    REQUIRE(cpu.GetPc() == 0x3456);
}

// **********************************************
// *        JP (HL)    ::    OP CODE: 0xE9      *
// **********************************************
// *                                            *
// *              PC <- HL                      *
// *                                            *
// **********************************************
TEST_CASE_METHOD(CpuFixture, "JP (HL) (0xE9) does not fetch extra bytes", "[flow][jp][hl]")
{
    // Arrange:
    // If JP (HL) incorrectly fetches operands, it might advance PC too far before jump logic.
    // This test checks we jump directly to HL regardless of what follows in memory.
    bus.Write(0x0000, 0xE9);   // JP (HL)
    bus.Write(0x0001, 0xFF);   // junk (should not be consumed)
    bus.Write(0x0002, 0xFF);   // junk (should not be consumed)

    cpu.SetPc(0x0000);
    cpu.SetHl(0x2000);

    // Act
    cpu.Step();

    // Assert
    REQUIRE(cpu.GetPc() == 0x2000);
}