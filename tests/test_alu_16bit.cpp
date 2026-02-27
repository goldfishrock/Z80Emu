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

// Test Helper Function: Set up a single opcode at PC=0x0000.
static void LoadOpcodeAtZero(Bus& bus, uint8_t opcode)
{
    bus.Write(0x0000, opcode);
}

TEST_CASE_METHOD(CpuFixture, "ADD HL,BC (0x09) basic addition", "[alu16][add][hl]")
{
    // Arrange
    LoadOpcodeAtZero(bus, 0x09);

    cpu.SetHl(0x1234);
    cpu.SetBc(0x0101);

    // Prove unaffected flags stay unchanged (S/Z/PV)
    cpu.SetFlag(Cpu::FLAG_S, 1);
    cpu.SetFlag(Cpu::FLAG_Z, 1);
    cpu.SetFlag(Cpu::FLAG_PV, 0);

    cpu.Step();

    // Assert
    REQUIRE(cpu.GetHl() == 0x1335);

    REQUIRE(cpu.GetFlag(Cpu::FLAG_N) == 0);
    REQUIRE(cpu.GetFlag(Cpu::FLAG_H) == 0);
    REQUIRE(cpu.GetFlag(Cpu::FLAG_C) == 0);

    REQUIRE(cpu.GetFlag(Cpu::FLAG_S) != 0);
    REQUIRE(cpu.GetFlag(Cpu::FLAG_Z) != 0);
    REQUIRE(cpu.GetFlag(Cpu::FLAG_PV) == 0);
}

TEST_CASE_METHOD(CpuFixture, "ADD HL,DE (0x19) half-carry only (bit 11 carry)", "[alu16][add][hl]")
{
    // Arrange
    LoadOpcodeAtZero(bus, 0x19);

    cpu.SetHl(0x0FFF);
    cpu.SetDe(0x0001);

    cpu.SetFlag(Cpu::FLAG_S, 0);
    cpu.SetFlag(Cpu::FLAG_Z, 0);
    cpu.SetFlag(Cpu::FLAG_PV, 1);

    // Act
    cpu.Step();

    // Assert
    REQUIRE(cpu.GetHl() == 0x1000);

    REQUIRE(cpu.GetFlag(Cpu::FLAG_N) == 0);
    REQUIRE(cpu.GetFlag(Cpu::FLAG_H) != 0);
    REQUIRE(cpu.GetFlag(Cpu::FLAG_C) == 0);

    REQUIRE(cpu.GetFlag(Cpu::FLAG_S) == 0);
    REQUIRE(cpu.GetFlag(Cpu::FLAG_Z) == 0);
    REQUIRE(cpu.GetFlag(Cpu::FLAG_PV) != 0);
}

TEST_CASE_METHOD(CpuFixture, "ADD HL,SP (0x39) carry out of bit 15 (wrap to 0)", "[alu16][add][hl]")
{
    // Arrange
    LoadOpcodeAtZero(bus, 0x39);

    cpu.SetHl(0xFFFF);
    cpu.SetSp(0x0001);

    cpu.SetFlag(Cpu::FLAG_S, 1);
    cpu.SetFlag(Cpu::FLAG_Z, 0);
    cpu.SetFlag(Cpu::FLAG_PV, 1);

    // Act
    cpu.Step();

    // Assert
    REQUIRE(cpu.GetHl() == 0x0000);

    REQUIRE(cpu.GetFlag(Cpu::FLAG_N) == 0);
    REQUIRE(cpu.GetFlag(Cpu::FLAG_H) == 1); // 0x0FFF + 0x0001 crosses 0x0FFF
    REQUIRE(cpu.GetFlag(Cpu::FLAG_C) != 0);

    REQUIRE(cpu.GetFlag(Cpu::FLAG_S) != 0);
    REQUIRE(cpu.GetFlag(Cpu::FLAG_Z) == 0);
    REQUIRE(cpu.GetFlag(Cpu::FLAG_PV) != 0);
}

TEST_CASE_METHOD(CpuFixture, "ADD HL,HL (0x29) doubles HL and sets carry when needed", "[alu16][add][hl]")
{
    // Arrange
    LoadOpcodeAtZero(bus, 0x29);

    cpu.SetHl(0x8000); // doubling should overflow bit 15

    cpu.SetFlag(Cpu::FLAG_S, 0);
    cpu.SetFlag(Cpu::FLAG_Z, 1);
    cpu.SetFlag(Cpu::FLAG_PV, 0);

    // Act
    cpu.Step();

    // Assert
    REQUIRE(cpu.GetHl() == 0x0000);

    REQUIRE(cpu.GetFlag(Cpu::FLAG_N) == 0);
    REQUIRE(cpu.GetFlag(Cpu::FLAG_H) == 0); // (0x000) + (0x000) doesn't half-carry
    REQUIRE(cpu.GetFlag(Cpu::FLAG_C) != 0);

    REQUIRE(cpu.GetFlag(Cpu::FLAG_S) == 0);
    REQUIRE(cpu.GetFlag(Cpu::FLAG_Z) != 0);
    REQUIRE(cpu.GetFlag(Cpu::FLAG_PV) == 0);
}