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
// *      CALL nn     ::    OP CODE: 0xCD       *
// **********************************************
// *                                            *
// *   Push return address, then jump to nn     *
// *                                            *
// **********************************************
TEST_CASE_METHOD(CpuFixture, "CALL nn (0xCD) pushes return address and jumps", "[flow][call]")
{
    // Arrange:
    // 0000: CD 34 12   CALL 0x1234
    bus.Write(0x0000, 0xCD);
    bus.Write(0x0001, 0x34);
    bus.Write(0x0002, 0x12);

    cpu.SetPc(0x0000);
    cpu.SetSp(0xFFFE);  // standard high stack start

    cpu.Step();

    // After CALL:
    // PC = 0x1234
    REQUIRE(cpu.GetPc() == 0x1234);

    // Return address should be 0x0003
    // Z80 pushes high byte first
    REQUIRE(cpu.GetSp() == 0xFFFC);
    REQUIRE(bus.Read(0xFFFD) == 0x00); // high byte
    REQUIRE(bus.Read(0xFFFC) == 0x03); // low byte
}

// **********************************************
// *      CALL nn     ::    OP CODE: 0xCD       *
// **********************************************
// *                                            *
// *   Verifies little-endian push order        *
// *                                            *
// **********************************************
TEST_CASE_METHOD(CpuFixture, "CALL nn (0xCD) pushes correct return address", "[flow][call]")
{
    bus.Write(0x0100, 0xCD);
    bus.Write(0x0101, 0x78);
    bus.Write(0x0102, 0x56);

    cpu.SetPc(0x0100);
    cpu.SetSp(0x8000);

    cpu.Step();

    // Return address should be 0x0103
    REQUIRE(bus.Read(0x7FFF) == 0x01); // high
    REQUIRE(bus.Read(0x7FFE) == 0x03); // low
}

// **********************************************
// *        RET        ::    OP CODE: 0xC9      *
// **********************************************
// *                                            *
// *        Pop address from stack to PC        *
// *                                            *
// **********************************************
TEST_CASE_METHOD(CpuFixture, "RET (0xC9) pops address and sets PC", "[flow][ret]")
{
    // Arrange
    bus.Write(0x0000, 0xC9);  // RET

    cpu.SetPc(0x0000);
    cpu.SetSp(0xFFFC);

    // Simulate return address 0x1234 on stack
    bus.Write(0xFFFC, 0x34); // low
    bus.Write(0xFFFD, 0x12); // high

    cpu.Step();

    REQUIRE(cpu.GetPc() == 0x1234);
    REQUIRE(cpu.GetSp() == 0xFFFE);
}