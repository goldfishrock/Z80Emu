#include <catch2/catch_test_macros.hpp>
#include "Cpu.h"
#include "Bus.h"

// **********************************************
// *              TEST OP CODES                 *
// **********************************************

// ************* NOP - No Operation *************
TEST_CASE("TEST OP CODE 0x00 :: NOP - Do bugger all!")
{
    Bus bus;
    Cpu cpu;

    cpu.connect(&bus);
    
    // Write 0 to the Memory
    bus.write(0x0000, 0x00);
    cpu.reset();                            // PC = 0x0000
    cpu.step();
    REQUIRE(cpu.pc() == 0x0001);
}

// *************      LD BC,nn     **************
TEST_CASE("TEST OP CODE 0x01 :: LD BC,nn - Loads 16-bit immediate into BC")
{
    Bus bus;
    Cpu cpu;

    cpu.connect(&bus);
    cpu.reset(); // PC = 0x0000

    // Opcode: 0x01, then nn little-endian: 0x34 0x12 => 0x1234
    bus.write(0x0000, 0x01);
    bus.write(0x0001, 0x34);
    bus.write(0x0002, 0x12);

    cpu.step();

    REQUIRE(cpu.bc() == 0x1234);
    REQUIRE(cpu.pc() == 0x0003);
}

// *************      LD DE,nn     **************
TEST_CASE("TEST OP CODE 0x11 :: LD DE,nn - Loads 16-bit immediate into DE")
{
    Bus bus;
    Cpu cpu;

    cpu.connect(&bus);
    cpu.reset(); // PC = 0x0000

    // Opcode: 0x11, then nn little-endian: 0x78 0x56 => 0x5678
    bus.write(0x0000, 0x11);
    bus.write(0x0001, 0x78);
    bus.write(0x0002, 0x56);

    cpu.step();

    REQUIRE(cpu.de() == 0x5678);
    REQUIRE(cpu.pc() == 0x0003);
}

// *************      LD HL,nn     **************
TEST_CASE("TEST OP CODE 0x21 :: LD HL,nn - Loads 16-bit immediate into HL")
{
    Bus bus;
    Cpu cpu;

    cpu.connect(&bus);
    cpu.reset(); // PC = 0x0000

    // Opcode: 0x21, then nn little-endian: 0xBC 0x9A => 0x9ABC
    bus.write(0x0000, 0x21);
    bus.write(0x0001, 0xBC);
    bus.write(0x0002, 0x9A);

    cpu.step();

    REQUIRE(cpu.hl() == 0x9ABC);
    REQUIRE(cpu.pc() == 0x0003);
}

// *************      LD SP,nn     **************
TEST_CASE("TEST OP CODE 0x31 :: LD SP,nn - Loads 16-bit immediate into SP")
{
    Bus bus;
    Cpu cpu;

    cpu.connect(&bus);
    cpu.reset(); // PC = 0x0000

    // Opcode: 0x31, then nn little-endian: 0x00 0x80 => 0x8000
    bus.write(0x0000, 0x31);
    bus.write(0x0001, 0x00);
    bus.write(0x0002, 0x80);

    cpu.step();

    REQUIRE(cpu.sp() == 0x8000);
    REQUIRE(cpu.pc() == 0x0003);
}

// *************      INC BC      ***************
TEST_CASE("TEST OP CODE 0x03 :: INC BC - Increments the BC register")
{
    Bus bus;
    Cpu cpu;

    cpu.connect(&bus);
    cpu.reset(); // PC = 0x0000

	// Opcode: 0x03, BC=ABCD initially, then after INC BC, BC should be ABCD + 1 = ABCE
    cpu.setBC(0xABCD);
	bus.write(0x0000, 0x03); // 0x03 is the opcode for INC BC
    cpu.step();

    REQUIRE(cpu.bc() == 0xABCE);
    REQUIRE(cpu.pc() == 0x0001);
}

// *************      INC DE      ***************
TEST_CASE("TEST OP CODE 0x13 INC DE :: Increments the DE register")
{
    Bus bus;
    Cpu cpu;

    cpu.connect(&bus);
    cpu.reset(); // PC = 0x0000

    // Opcode: 0x13, DE=5656 initially, then after INC DE, DE should be 5656 + 1 = 5657
    cpu.setDE(0x5656);
    bus.write(0x0000, 0x13); // 0x03 is the opcode for INC DE
    cpu.step();

    REQUIRE(cpu.de() == 0x5657);
    REQUIRE(cpu.pc() == 0x0001);
}

// *************      INC HL      ***************
TEST_CASE("TEST OP CODE 0x23 :: INC HL - Increments the HL register")
{
    Bus bus;
    Cpu cpu;

    cpu.connect(&bus);
    cpu.reset(); // PC = 0x0000

    // Opcode: 0x23, HL=4321 initially, then after INC HL, HL should be 4321 + 1 = 4322
    cpu.setHL(0x4321);
    bus.write(0x0000, 0x23); // 0x23 is the opcode for INC DE
    cpu.step();

    REQUIRE(cpu.hl() == 0x4322);
    REQUIRE(cpu.pc() == 0x0001);
}

// *************      DEC BC      ***************
TEST_CASE("TEST OP CODE 0x0B :: DEC BC - Decrements the BC register")
{
    Bus bus;
    Cpu cpu;

    cpu.connect(&bus);
    cpu.reset(); // PC = 0x0000

    // Opcode: 0x0B, BC=0x2222 initially, then after DEC BC, BC should be 2222 - 1 = 2221
    cpu.setBC(0x2222);
    bus.write(0x0000, 0x0B); // 0x0B is the opcode for DEC BC
    cpu.step();

    REQUIRE(cpu.bc() == 0x2221);
    REQUIRE(cpu.pc() == 0x0001);
}

// *************      DEC DE      ***************
TEST_CASE("TEST OP CODE 0x1B :: DEC DE - Decrements the DE register")
{
    Bus bus;
    Cpu cpu;

    cpu.connect(&bus);
    cpu.reset(); // PC = 0x0000

    // Opcode: 0x1B, DE=8765 initially, then after DEC DE, DE should be 8765 - 1 = 8764
    cpu.setDE(0x8765);
    bus.write(0x0000, 0x1B); // 0x1B is the opcode for DEC DE
    cpu.step();

    REQUIRE(cpu.de() == 0x8764);
    REQUIRE(cpu.pc() == 0x0001);
}

// *************      DEC HL      ***************
TEST_CASE("TEST OP CODE 0x2B :: DEC HL - Decrements the HL register")
{
    Bus bus;
    Cpu cpu;

    cpu.connect(&bus);
    cpu.reset(); // PC = 0x0000

    // Opcode: 0x2B, DE=EEEF initially, then after DEC DE, DE should be EEEF - 1 = EEEE
    cpu.setHL(0xEEEF);
    bus.write(0x0000, 0x2B); // 0x2B is the opcode for DEC HL
    cpu.step();

    REQUIRE(cpu.hl() == 0xEEEE);
    REQUIRE(cpu.pc() == 0x0001);
}
