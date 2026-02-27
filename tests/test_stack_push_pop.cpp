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
// *      TEST STACK PUSH/POP OP CODES          *
// **********************************************



// **********************************************
// *        PUSH BC    ::    OP CODE: 0xC5      *
// **********************************************
// *                                            *
// *         Puts BC on the stack               *
// *                                            *
// **********************************************
TEST_CASE_METHOD(CpuFixture, "PUSH BC (0xC5) stores BC on stack", "[stack][push]")
{
    bus.Write(0x0000, 0xC5);

    cpu.SetSp(0x1000);
    cpu.SetBc(0x1234);

    cpu.Step();

    REQUIRE(cpu.GetSp() == 0x0FFE);
    REQUIRE(bus.Read(0x0FFF) == 0x12);
    REQUIRE(bus.Read(0x0FFE) == 0x34);
}

// **********************************************
// *        POP BC    ::     OP CODE: 0xC1      *
// **********************************************
// *                                            *
// *         Get BC from the stack              *
// *                                            *
// **********************************************
TEST_CASE_METHOD(CpuFixture, "POP BC (0xC1) restores BC from stack", "[stack][pop]")
{
    bus.Write(0x0000, 0xC1);

    cpu.SetSp(0x0FFE);

    bus.Write(0x0FFE, 0x34); // low
    bus.Write(0x0FFF, 0x12); // high

    cpu.Step();

    REQUIRE(cpu.GetBc() == 0x1234);
    REQUIRE(cpu.GetSp() == 0x1000);
}

// **********************************************
// *        PUSH DE   ::     OP CODE: 0xD5      *
// **********************************************
// *                                            *
// *         Place DE on the stack              *
// *                                            *
// **********************************************
TEST_CASE_METHOD(CpuFixture, "PUSH DE (0xD5) stores BC on the stack", "[stack][push]")
{
    bus.Write(0x0000, 0xD5);

    cpu.SetSp(0x1000);
    cpu.SetDe(0x1234);

    cpu.Step();

    REQUIRE(cpu.GetSp() == 0x0FFE);
    REQUIRE(bus.Read(0x0FFF) == 0x12);
    REQUIRE(bus.Read(0x0FFE) == 0x34);
}

// **********************************************
// *        POP DE   ::     OP CODE: 0xD1       *
// **********************************************
// *                                            *
// *         POP DE from the stack              *
// *                                            *
// **********************************************
TEST_CASE_METHOD(CpuFixture, "POP DE (0xD1) restores DE from stack", "[stack][pop]")
{
    bus.Write(0x0000, 0xD1);

    cpu.SetSp(0x0FFE);

    bus.Write(0x0FFE, 0x34); // low
    bus.Write(0x0FFF, 0x12); // high

    cpu.Step();

    REQUIRE(cpu.GetDe() == 0x1234);
    REQUIRE(cpu.GetSp() == 0x1000);
}

// **********************************************
// *        PUSH HL   ::     OP CODE: 0xE5      *
// **********************************************
// *                                            *
// *         Place HL on the stack              *
// *                                            *
// **********************************************
TEST_CASE_METHOD(CpuFixture, "PUSH HL (0xE5) stores HL on the stack", "[stack][push]")
{
    bus.Write(0x0000, 0xE5);

    cpu.SetSp(0x1000);
    cpu.SetHl(0x6677);

    cpu.Step();

    REQUIRE(cpu.GetSp() == 0x0FFE);
    REQUIRE(bus.Read(0x0FFF) == 0x66);
    REQUIRE(bus.Read(0x0FFE) == 0x77);
}

// **********************************************
// *        POP HL   ::     OP CODE: 0xE1       *
// **********************************************
// *                                            *
// *         POP HL from the stack              *
// *                                            *
// **********************************************
TEST_CASE_METHOD(CpuFixture, "POP HL (0xE1) restores HL from stack", "[stack][pop]")
{
    bus.Write(0x0000, 0xE1);

    cpu.SetSp(0x0FFE);

    bus.Write(0x0FFE, 0x77); // low
    bus.Write(0x0FFF, 0x66); // high

    cpu.Step();

    REQUIRE(cpu.GetHl() == 0x6677);
    REQUIRE(cpu.GetSp() == 0x1000);
}