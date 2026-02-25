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
// *            TEST LD OP CODES                *
// **********************************************

// **********************************************
// *      LD BC,nn   ::   OP CODE: 0x01         *
// **********************************************
TEST_CASE_METHOD(CpuFixture, "TEST OP CODE 0x01 :: LD BC,nn - Loads 16-bit immediate value (little-endian) into BC")
{
    // Opcode: 0x01, then nn little-endian: 0x34 0x12 => 0x1234
    bus.Write(0x0000, 0x01);
    bus.Write(0x0001, 0x34);
    bus.Write(0x0002, 0x12);

    cpu.Step();

    REQUIRE(cpu.GetBc() == 0x1234);
    REQUIRE(cpu.GetPc() == 0x0003);
}

// **********************************************
// *      LD DE,nn   ::   OP CODE: 0x11         *
// **********************************************
TEST_CASE_METHOD(CpuFixture, "TEST OP CODE 0x11 :: LD DE,nn - Loads 16-bit immediate value (little-endian) into DE")
{
    // Opcode: 0x11, then nn little-endian: 0x78 0x56 => 0x5678
    bus.Write(0x0000, 0x11);
    bus.Write(0x0001, 0x78);
    bus.Write(0x0002, 0x56);

    cpu.Step();

    REQUIRE(cpu.GetDe() == 0x5678);
    REQUIRE(cpu.GetPc() == 0x0003);
}

// **********************************************
// *      LD HL,nn   ::   OP CODE: 0x21         *
// **********************************************
TEST_CASE_METHOD(CpuFixture, "TEST OP CODE 0x21 :: LD HL,nn - Loads 16-bit immediate value (little-endian) into  HL")
{
    // Opcode: 0x21, then nn little-endian: 0xBC 0x9A => 0x9ABC
    bus.Write(0x0000, 0x21);
    bus.Write(0x0001, 0xBC);
    bus.Write(0x0002, 0x9A);

    cpu.Step();

    REQUIRE(cpu.GetHl() == 0x9ABC);
    REQUIRE(cpu.GetPc() == 0x0003);
}

// **********************************************
// *      LD SP,nn   ::   OP CODE: 0x31         *
// **********************************************
TEST_CASE_METHOD(CpuFixture, "TEST OP CODE 0x31 :: LD SP,nn - Loads 16-bit immediate value (little-endian) into  SP")
{
    // Opcode: 0x31, then nn little-endian: 0x00 0x80 => 0x8000
    bus.Write(0x0000, 0x31);
    bus.Write(0x0001, 0x00);
    bus.Write(0x0002, 0x80);

    cpu.Step();

    REQUIRE(cpu.GetSp() == 0x8000);
    REQUIRE(cpu.GetPc() == 0x0003);
}


// **********************************************
// *       LD A,nn   ::   OP CODE: 0x3E         *
// **********************************************
TEST_CASE_METHOD(CpuFixture, "LD A,n loads immediate into A and advances PC")
{
    const auto f_before = cpu.GetF();

    bus.Write(0x0000, 0x3E); // LD A,n
    bus.Write(0x0001, 0x56); // n

    cpu.Step();

    REQUIRE(cpu.GetA() == 0x56);
    REQUIRE(cpu.GetPc() == 0x0002);
    REQUIRE(cpu.GetF() == f_before);
}

// **********************************************
// *       LD B,nn   ::   OP CODE: 0x06         *
// **********************************************
TEST_CASE_METHOD(CpuFixture, "LD B,n loads immediate into B and advances PC")
{

    const auto f_before = cpu.GetF();

    bus.Write(0x0000, 0x06); // LD B,n
    bus.Write(0x0001, 0x33); // n

    cpu.Step();

    REQUIRE(cpu.GetB() == 0x33);
    REQUIRE(cpu.GetPc() == 0x0002);
    REQUIRE(cpu.GetF() == f_before);
}

// **********************************************
// *       LD C,nn   ::   OP CODE: 0x0E         *
// **********************************************
TEST_CASE_METHOD(CpuFixture, "LD C,n loads immediate into C and advances PC")
{
    const auto f_before = cpu.GetF();

    bus.Write(0x0000, 0x0E); // LD C,n
    bus.Write(0x0001, 0x88); // n

    cpu.Step();

    REQUIRE(cpu.GetC() == 0x88);
    REQUIRE(cpu.GetPc() == 0x0002);
    REQUIRE(cpu.GetF() == f_before);
}

// **********************************************
// *       LD D,nn   ::   OP CODE: 0x16         *
// **********************************************
TEST_CASE_METHOD(CpuFixture, "LD D,n loads immediate into D and advances PC")
{
    const auto f_before = cpu.GetF();

    bus.Write(0x0000, 0x16); // LD D,n
    bus.Write(0x0001, 0x77); // n

    cpu.Step();

    REQUIRE(cpu.GetD() == 0x77);
    REQUIRE(cpu.GetPc() == 0x0002);
    REQUIRE(cpu.GetF() == f_before);
}

// **********************************************
// *       LD E,nn   ::   OP CODE: 0x1E         *
// **********************************************
TEST_CASE_METHOD(CpuFixture, "LD E,n loads immediate into E and advances PC")
{
    const auto f_before = cpu.GetF();

    bus.Write(0x0000, 0x1E); // LD E,n
    bus.Write(0x0001, 0x33); // n

    cpu.Step();

    REQUIRE(cpu.GetE() == 0x33);
    REQUIRE(cpu.GetPc() == 0x0002);
    REQUIRE(cpu.GetF() == f_before);
}

// **********************************************
// *       LD H,nn   ::   OP CODE: 0x26         *
// **********************************************
TEST_CASE_METHOD(CpuFixture, "LD H,n loads immediate into H and advances PC")
{
    const auto f_before = cpu.GetF();

    bus.Write(0x0000, 0x26); // LD E,n
    bus.Write(0x0001, 0x22); // n

    cpu.Step();

    REQUIRE(cpu.GetH() == 0x22);
    REQUIRE(cpu.GetPc() == 0x0002);
    REQUIRE(cpu.GetF() == f_before);
}

// **********************************************
// *       LD L,nn   ::   OP CODE: 0x2E         *
// **********************************************
TEST_CASE_METHOD(CpuFixture, "LD L,n loads immediate into L and advances PC")
{
    const auto f_before = cpu.GetF();

    bus.Write(0x0000, 0x2E); // LD L,n
    bus.Write(0x0001, 0x22); // n

    cpu.Step();

    auto xxx = cpu.GetL();
    auto yyy = cpu.GetPc();
    auto zzz = cpu.GetF();

    REQUIRE(cpu.GetL() == 0x22);
    REQUIRE(cpu.GetPc() == 0x0002);
    REQUIRE(cpu.GetF() == f_before);
}

// **********************************************
// *       LD A,B   ::   OP CODE: 0x78          *
// **********************************************
TEST_CASE_METHOD(CpuFixture, "LD A,B loads contents of B into A and advances PC")
{
    cpu.SetA(0x00);
    cpu.SetB(0x42);
    cpu.SetF(0xA5);
    const auto f_before = cpu.GetF();

    bus.Write(0x0000, 0x78); // LD A,B
    cpu.Step();

    REQUIRE(cpu.GetA() == 0x42);
    REQUIRE(cpu.GetPc() == 0x0001);
    REQUIRE(cpu.GetF() == 0xA5);
}

// **********************************************
// *       LD r, (HL)   ::   OP CODE: 0x46      *
// **********************************************
TEST_CASE_METHOD(CpuFixture, "LD r,(HL) reads from memory at HL", "[cpu][ld]")
{
    cpu.SetHl(0x4000);
    bus.Write(0x4000, 0xAB);

    // Put opcode at PC=0
    bus.Write(0x0000, 0x46); // LD B,(HL)

    cpu.Step();

    REQUIRE(cpu.GetB() == 0xAB);
}

// **********************************************
// *       LD (HL), r   ::   OP CODE: 0x71      *
// **********************************************
TEST_CASE_METHOD(CpuFixture, "LD (HL),r writes to memory at HL", "[cpu][ld]")
{
    cpu.SetHl(0x4000);
    cpu.SetC(0x5E);

    bus.Write(0x0000, 0x71); // LD (HL),C

    cpu.Step();

    REQUIRE(bus.Read(0x4000) == 0x5E);
}
