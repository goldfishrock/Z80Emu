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

TEST_CASE("TEST :: CPU can connect to a bus", "[cpu]") {
    Bus bus;
    Cpu cpu;
    REQUIRE_FALSE(cpu.is_connected());

    cpu.Connect(&bus);
    REQUIRE(cpu.is_connected());
}

TEST_CASE("TEST :: Reset sets PC to 0x0000")
{
    Cpu cpu;
    cpu.Reset();
    REQUIRE(cpu.GetPc() == 0x0000);
}

TEST_CASE("TEST :: Check CPU can fetch a Byte")
{
    Cpu cpu;
    Bus testBus;
	testBus.Write(0x0000, 0x55);
    cpu.Connect(&testBus);
    cpu.Reset();
    REQUIRE(cpu.FetchByte() == 0x55);
    REQUIRE(cpu.GetPc() == 0x0001);
}

TEST_CASE("TEST :: Reset sets SP to 0xFFFF")
{
    Cpu cpu;
    cpu.Reset();
    REQUIRE(cpu.GetSp() == 0xFFFF);
}

TEST_CASE("TEST :: Test pushByte() and popByte() to ensure they use stack correctly")
{
    Bus bus;
    Cpu cpu;

    cpu.Connect(&bus);
    cpu.Reset();                                        // SP should be 0xFFFF

    cpu.PushByte(0xAA);

    REQUIRE(cpu.GetSp() == 0xFFFE);
    REQUIRE(bus.Read(0xFFFE) == 0xAA);

    const auto value = cpu.PopByte();

    REQUIRE(value == 0xAA);
    REQUIRE(cpu.GetSp() == 0xFFFF);
}

TEST_CASE("TEST :: fetchWord() reads little-endian and increments PC by 2")
{
    Bus bus;
    Cpu cpu;

    cpu.Connect(&bus);
    cpu.Reset(); // PC = 0x0000

    // Memory: 0x0000 = 0x34 (low), 0x0001 = 0x12 (high) => word 0x1234
    bus.Write(0x0000, 0x34);
    bus.Write(0x0001, 0x12);

    REQUIRE(cpu.FetchWord() == 0x1234);
    REQUIRE(cpu.GetPc() == 0x0002);
}

// **********************************************
// *   NOP No Operation   ::   OP CODE: 0x00    *
// **********************************************
TEST_CASE_METHOD(CpuFixture, "TEST OP CODE 0x00 :: NOP - Do bugger all!")
{
    // Write 0 to the Memory
    bus.Write(0x0000, 0x00);
    cpu.Reset();                            // PC = 0x0000
    cpu.Step();
    REQUIRE(cpu.GetPc() == 0x0001);
}

// **********************************************
// *   SET HALT STATE   ::   OP CODE: 0x76      *
// **********************************************
TEST_CASE_METHOD(CpuFixture, "0x76 sets HALT state (not treated as LD (HL),(HL))", "[cpu][halt]")
{
    bus.Write(0x0000, 0x76); // HALT

    cpu.Step();

    REQUIRE(cpu.is_halted() == true);
}
