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
// *        TEST ARITHMETIC OP CODES            *
// **********************************************

// **********************************************
// *        ADD A,B   ::   OP CODE: 0x80        *
// **********************************************
TEST_CASE("ADD A,B sets A and flags correctly (0x7F + 0x01)")
{
    Bus bus;
    Cpu cpu;
    cpu.Connect(&bus);

    bus.Write(0x0000, 0x80); // ADD A,B

    cpu.Reset();

    cpu.SetA(0x7F);
    cpu.SetB(0x01);

    cpu.Step();

    REQUIRE(cpu.GetA() == 0x80);
    REQUIRE(cpu.GetF() == (Cpu::FLAG_S | Cpu::FLAG_H | Cpu::FLAG_PV));
}

// **********************************************
// *        ADD A,n   ::   OP CODE: 0xC6        *
// **********************************************
TEST_CASE("ADD A,n (0xC6): 0xFF + 0x01 -> 0x00 sets Z|H|C")
{
    Bus bus;
    Cpu cpu;
    cpu.Connect(&bus);

    bus.Write(0x0000, 0xC6); // ADD A,n
    bus.Write(0x0001, 0x01); // n = 1

    cpu.Reset();
    cpu.SetA(0xFF);

    cpu.Step();

    REQUIRE(cpu.GetA() == 0x00);
    REQUIRE(cpu.GetF() == (Cpu::FLAG_Z | Cpu::FLAG_H | Cpu::FLAG_C));
}

// **********************************************
// *        ADC A,B   ::   OP CODE: 0x88        *
// **********************************************
// *                                            *
// * Carry-in behaviour                         *
// *                                            *
// **********************************************
TEST_CASE("ADC A,B (0x88): carry-in increments result")
{
    // Testing carry-in behaviour
    Bus bus;
    Cpu cpu;
    cpu.Connect(&bus);

    bus.Write(0x0000, 0x88); // ADC A,B
    cpu.Reset();

    cpu.SetA(0x00);
    cpu.SetB(0x00);
    cpu.SetF(Cpu::FLAG_C);        // carry in = 1

    cpu.Step();

    REQUIRE(cpu.GetA() == 0x01);
    REQUIRE(cpu.GetF() == 0x00);
}

// **********************************************
// *        ADC A,B   ::   OP CODE: 0x88        *
// **********************************************
// *                                            *
// * Carry-in causes half-carry + carry + zero  *
// *                                            *
// **********************************************
TEST_CASE("ADC A,B (0x88): 0xFF + 0x00 + C -> 0x00 sets Z|H|C")
{
    // Testing carry-in causes half-carry + carry + zero
    Bus bus;
    Cpu cpu;
    cpu.Connect(&bus);

    bus.Write(0x0000, 0x88); // ADC A,B
    cpu.Reset();

    cpu.SetA(0xFF);
    cpu.SetB(0x00);
    cpu.SetF(Cpu::FLAG_C);        // carry in = 1

    cpu.Step();

    REQUIRE(cpu.GetA() == 0x00);
    REQUIRE(cpu.GetF() == (Cpu::FLAG_Z | Cpu::FLAG_H | Cpu::FLAG_C));
}

// **********************************************
// *        SUB B    ::    OP CODE: 0x90        *
// **********************************************
// *                                            *
// *            Simple 5-3=2 Test               *
// *                                            *
// **********************************************

TEST_CASE("SUB B (0x90): 0x05 - 0x03 = 0x02 sets N")
{
    Bus bus;
    Cpu cpu;
    cpu.Connect(&bus);

    bus.Write(0x0000, 0x90); // SUB B
    cpu.Reset();

    cpu.SetA(0x05);
    cpu.SetB(0x03);

    cpu.Step();

    REQUIRE(cpu.GetA() == 0x02);
    REQUIRE(cpu.GetF() == Cpu::FLAG_N);
}

// **********************************************
// *        SUB B    ::    OP CODE: 0x90        *
// **********************************************
// *                                            *
// *      Borrow: 0 − 1 = 0xFF (S|H|N|C)        *
// *                                            *
// **********************************************
TEST_CASE("SUB B (0x90): 0x00 - 0x01 = 0xFF sets S|H|N|C")
{
    Bus bus;
    Cpu cpu;
    cpu.Connect(&bus);

    bus.Write(0x0000, 0x90); // SUB B
    cpu.Reset();

    cpu.SetA(0x00);
    cpu.SetB(0x01);

    cpu.Step();

    REQUIRE(cpu.GetA() == 0xFF);
    REQUIRE(cpu.GetF() == (Cpu::FLAG_S | Cpu::FLAG_H | Cpu::FLAG_N | Cpu::FLAG_C));
}

// **********************************************
// *        SUB B    ::    OP CODE: 0x90        *
// **********************************************
// *                                            *
// * Signed overflow: 0x80 − 1 = 0x7F (H|PV|N)  *
// *                                            *
// **********************************************
TEST_CASE("SUB B (0x90): 0x80 - 0x01 = 0x7F sets H|PV|N")
{
    Bus bus;
    Cpu cpu;
    cpu.Connect(&bus);

    bus.Write(0x0000, 0x90); // SUB B
    cpu.Reset();

    cpu.SetA(0x80);
    cpu.SetB(0x01);

    cpu.Step();

    REQUIRE(cpu.GetA() == 0x7F);
    REQUIRE(cpu.GetF() == (Cpu::FLAG_H | Cpu::FLAG_PV | Cpu::FLAG_N));
}

// **********************************************
// *        SBC A,B    ::    OP CODE: 0x98      *
// **********************************************
// *                                            *
// *         Carry-In Changes Result            *
// *                                            *
// **********************************************
TEST_CASE("SBC A,B (0x98): 0x01 - 0x00 - C -> 0x00 sets Z|N")
{
    Bus bus;
    Cpu cpu;
    cpu.Connect(&bus);

    bus.Write(0x0000, 0x98); // SBC A,B
    cpu.Reset();

    cpu.SetA(0x01);
    cpu.SetB(0x00);
    cpu.SetF(Cpu::FLAG_C);    // carry_in = 1

    cpu.Step();

    REQUIRE(cpu.GetA() == 0x00);
    REQUIRE(cpu.GetF() == (Cpu::FLAG_Z | Cpu::FLAG_N));
}

// **********************************************
// *        SBC A,B    ::    OP CODE: 0x98      *
// **********************************************
// *                                            *
// *        Borrow occurs with Carry-In         *
// *                                            *
// **********************************************
TEST_CASE("SBC A,B (0x98): 0x00 - 0x00 - C -> 0xFF sets S|H|N|C")
{
    Bus bus;
    Cpu cpu;
    cpu.Connect(&bus);

    bus.Write(0x0000, 0x98); // SBC A,B
    cpu.Reset();

    cpu.SetA(0x00);
    cpu.SetB(0x00);
    cpu.SetF(Cpu::FLAG_C);    // carry_in = 1

    cpu.Step();

    REQUIRE(cpu.GetA() == 0xFF);
    REQUIRE(cpu.GetF() == (Cpu::FLAG_S | Cpu::FLAG_H | Cpu::FLAG_N | Cpu::FLAG_C));
}