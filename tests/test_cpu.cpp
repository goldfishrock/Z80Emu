#include <catch2/catch_test_macros.hpp>
#include "Cpu.h"
#include "Bus.h"

TEST_CASE("TEST :: CPU can connect to a bus", "[cpu]") {
    Bus bus;
    Cpu cpu;
    REQUIRE_FALSE(cpu.isConnected());

    cpu.connect(&bus);
    REQUIRE(cpu.isConnected());
}

TEST_CASE("TEST :: Reset sets PC to 0x0000")
{
    Cpu cpu;
    cpu.reset();
    REQUIRE(cpu.pc() == 0x0000);
}

TEST_CASE("TEST :: Check CPU can fetch a Byte")
{
    Cpu cpu;
    Bus testBus;
	testBus.write(0x0000, 0x55);
    cpu.connect(&testBus);
    cpu.reset();
    REQUIRE(cpu.fetchByte() == 0x55);
    REQUIRE(cpu.pc() == 0x0001);
}

TEST_CASE("TEST :: Reset sets SP to 0xFFFF")
{
    Cpu cpu;
    cpu.reset();
    REQUIRE(cpu.sp() == 0xFFFF);
}

TEST_CASE("TEST :: Test pushByte() and popByte() to ensure they use stack correctly")
{
    Bus bus;
    Cpu cpu;

    cpu.connect(&bus);
    cpu.reset();                                        // SP should be 0xFFFF

    cpu.pushByte(0xAA);

    REQUIRE(cpu.sp() == 0xFFFE);
    REQUIRE(bus.read(0xFFFE) == 0xAA);

    const auto value = cpu.popByte();

    REQUIRE(value == 0xAA);
    REQUIRE(cpu.sp() == 0xFFFF);
}

TEST_CASE("TEST :: fetchWord() reads little-endian and increments PC by 2")
{
    Bus bus;
    Cpu cpu;

    cpu.connect(&bus);
    cpu.reset(); // PC = 0x0000

    // Memory: 0x0000 = 0x34 (low), 0x0001 = 0x12 (high) => word 0x1234
    bus.write(0x0000, 0x34);
    bus.write(0x0001, 0x12);

    REQUIRE(cpu.fetchWord() == 0x1234);
    REQUIRE(cpu.pc() == 0x0002);
}

