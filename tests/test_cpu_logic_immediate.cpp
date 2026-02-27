#include <catch2/catch_test_macros.hpp>

#include "Bus.h"
#include "Cpu.h"

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
// *           AND n   ::    OP CODE: 0xE6      *
// **********************************************
// *                                            *
// *        Bitwise AND between A and n         *
// *                                            *
// **********************************************
TEST_CASE_METHOD(CpuFixture, "AND n sets flags correctly", "[cpu][alu][logic]")
{
    cpu.SetA(0b10101010);

    // AND 0b11001100 => 0b10001000
    bus.Write(0x0000, 0xE6); // AND n
    bus.Write(0x0001, 0b11001100);

    cpu.Step();

    REQUIRE(cpu.GetA() == 0b10001000);

    // Flags: S Z H P/V N C
    REQUIRE(cpu.GetFlag(Cpu::FLAG_S) != 0);
    REQUIRE(cpu.GetFlag(Cpu::FLAG_S) != 0);
    REQUIRE(cpu.GetFlag(Cpu::FLAG_Z) == 0);
    REQUIRE(cpu.GetFlag(Cpu::FLAG_H) != 0);
    REQUIRE(cpu.GetFlag(Cpu::FLAG_PV) != 0);   // parity of 0b10001000 = even (2 bits set)
    REQUIRE(cpu.GetFlag(Cpu::FLAG_N) == 0);
    REQUIRE(cpu.GetFlag(Cpu::FLAG_C) == 0);
}

// **********************************************
// *           OR n   ::     OP CODE: 0xF6      *
// **********************************************
// *                                            *
// *        Bitwise OR between A and n        *
// *                                            *
// **********************************************
TEST_CASE_METHOD(CpuFixture, "OR n sets flags correctly", "[cpu][alu][logic]")
{
    cpu.SetA(0x00);

    bus.Write(0x0000, 0xF6); // OR n
    bus.Write(0x0001, 0x00);

    cpu.Step();

    REQUIRE(cpu.GetA() == 0x00);
    REQUIRE(cpu.GetFlag(Cpu::FLAG_S) == 0);
    REQUIRE(cpu.GetFlag(Cpu::FLAG_Z) != 0);
    REQUIRE(cpu.GetFlag(Cpu::FLAG_H) == 0);
    REQUIRE(cpu.GetFlag(Cpu::FLAG_PV) != 0); // parity of 0 is even
    REQUIRE(cpu.GetFlag(Cpu::FLAG_N) == 0);
    REQUIRE(cpu.GetFlag(Cpu::FLAG_C) == 0);
}

// **********************************************
// *           XOR n   ::    OP CODE: 0xEE      *
// **********************************************
// *                                            *
// *        Bitwise XOR between A and n         *
// *                                            *
// **********************************************
TEST_CASE_METHOD(CpuFixture, "XOR n sets flags correctly", "[cpu][alu][logic]")
{
    cpu.SetA(0xFF);

    bus.Write(0x0000, 0xEE); // XOR n
    bus.Write(0x0001, 0xFF);

    cpu.Step();

    REQUIRE(cpu.GetA() == 0x00);
    REQUIRE(cpu.GetFlag(Cpu::FLAG_S) == 0);
    REQUIRE(cpu.GetFlag(Cpu::FLAG_Z) != 0);
    REQUIRE(cpu.GetFlag(Cpu::FLAG_H) == 0);
    REQUIRE(cpu.GetFlag(Cpu::FLAG_PV) != 0);
    REQUIRE(cpu.GetFlag(Cpu::FLAG_N) == 0);
    REQUIRE(cpu.GetFlag(Cpu::FLAG_C) == 0);
}

// **********************************************
// *           CP n   ::     OP CODE: 0xFE      *
// **********************************************
// *                                            *
// *            Compares A with n               *
// *                                            *
// **********************************************
TEST_CASE_METHOD(CpuFixture, "CP n compares without changing A", "[cpu][alu][cp]")
{
    cpu.SetA(0x10);

    bus.Write(0x0000, 0xFE); // CP n
    bus.Write(0x0001, 0x10);

    cpu.Step();

    REQUIRE(cpu.GetA() == 0x10);      // unchanged
    REQUIRE(cpu.GetFlag(Cpu::FLAG_Z) != 0);  // equal
    REQUIRE(cpu.GetFlag(Cpu::FLAG_N) != 0);  // compare acts like subtraction
    REQUIRE(cpu.GetFlag(Cpu::FLAG_C) == 0); // no borrow
}