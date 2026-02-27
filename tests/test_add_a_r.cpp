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
// *        TEST 16-Bit ALU OP CODES            *
// **********************************************



// **********************************************
// *        ADD A, C   ::    OP CODE: 0x81      *
// **********************************************
// *                                            *
// *                ADD A & C                   *
// *                                            *
// **********************************************
TEST_CASE_METHOD(CpuFixture, "ADD A, C (0x81) basic addition", "[alu8][add][a]")
{
    // Opcode: 0x81,
    bus.Write(0x0000, 0x81);

    cpu.SetA(0x01);
    cpu.SetC(0x02);

    cpu.Step();

    REQUIRE(cpu.GetA() == 0x03);
    REQUIRE(cpu.GetFlag(Cpu::FLAG_N) == 0);
}