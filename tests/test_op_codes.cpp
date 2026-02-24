#include <catch2/catch_test_macros.hpp>
#include "Cpu.h"
#include "Bus.h"

struct CpuFixture
{
    Bus bus;
    Cpu cpu;

    CpuFixture()
    {
        cpu.connect(&bus);
        cpu.reset();
    }
};

// **********************************************
// *              TEST OP CODES                 *
// **********************************************

// ************* NOP - No Operation *************
TEST_CASE_METHOD(CpuFixture, "TEST OP CODE 0x00 :: NOP - Do bugger all!")
{
    // Write 0 to the Memory
    bus.write(0x0000, 0x00);
    cpu.reset();                            // PC = 0x0000
    cpu.step();
    REQUIRE(cpu.pc() == 0x0001);
}

// **********************************************
// *      LD BC,nn   ::   OP CODE: 0x01         *
// **********************************************
TEST_CASE_METHOD(CpuFixture, "TEST OP CODE 0x01 :: LD BC,nn - Loads 16-bit immediate value (little-endian) into BC")
{
    // Opcode: 0x01, then nn little-endian: 0x34 0x12 => 0x1234
    bus.write(0x0000, 0x01);
    bus.write(0x0001, 0x34);
    bus.write(0x0002, 0x12);

    cpu.step();

    REQUIRE(cpu.bc() == 0x1234);
    REQUIRE(cpu.pc() == 0x0003);
}

// **********************************************
// *      LD DE,nn   ::   OP CODE: 0x11         *
// **********************************************
TEST_CASE_METHOD(CpuFixture,"TEST OP CODE 0x11 :: LD DE,nn - Loads 16-bit immediate value (little-endian) into DE")
{
    // Opcode: 0x11, then nn little-endian: 0x78 0x56 => 0x5678
    bus.write(0x0000, 0x11);
    bus.write(0x0001, 0x78);
    bus.write(0x0002, 0x56);

    cpu.step();

    REQUIRE(cpu.de() == 0x5678);
    REQUIRE(cpu.pc() == 0x0003);
}

// **********************************************
// *      LD HL,nn   ::   OP CODE: 0x21         *
// **********************************************
TEST_CASE_METHOD(CpuFixture,"TEST OP CODE 0x21 :: LD HL,nn - Loads 16-bit immediate value (little-endian) into  HL")
{
    // Opcode: 0x21, then nn little-endian: 0xBC 0x9A => 0x9ABC
    bus.write(0x0000, 0x21);
    bus.write(0x0001, 0xBC);
    bus.write(0x0002, 0x9A);

    cpu.step();

    REQUIRE(cpu.hl() == 0x9ABC);
    REQUIRE(cpu.pc() == 0x0003);
}

// **********************************************
// *      LD SP,nn   ::   OP CODE: 0x31         *
// **********************************************
TEST_CASE_METHOD(CpuFixture,"TEST OP CODE 0x31 :: LD SP,nn - Loads 16-bit immediate value (little-endian) into  SP")
{
    // Opcode: 0x31, then nn little-endian: 0x00 0x80 => 0x8000
    bus.write(0x0000, 0x31);
    bus.write(0x0001, 0x00);
    bus.write(0x0002, 0x80);

    cpu.step();

    REQUIRE(cpu.sp() == 0x8000);
    REQUIRE(cpu.pc() == 0x0003);
}

// **********************************************
// *       INC BC   ::   OP CODE: 0x03          *
// **********************************************
TEST_CASE_METHOD(CpuFixture,"TEST OP CODE 0x03 :: INC BC - Increments the BC register")
{
	// Opcode: 0x03, BC=ABCD initially, then after INC BC, BC should be ABCD + 1 = ABCE
    cpu.setBC(0xABCD);
	bus.write(0x0000, 0x03); // 0x03 is the opcode for INC BC
    cpu.step();

    REQUIRE(cpu.bc() == 0xABCE);
    REQUIRE(cpu.pc() == 0x0001);
}

// **********************************************
// *       INC DE   ::   OP CODE: 0x13          *
// **********************************************
TEST_CASE_METHOD(CpuFixture,"TEST OP CODE 0x13 INC DE :: Increments the DE register")
{
    // Opcode: 0x13, DE=5656 initially, then after INC DE, DE should be 5656 + 1 = 5657
    cpu.setDE(0x5656);
    bus.write(0x0000, 0x13); // 0x03 is the opcode for INC DE
    cpu.step();

    REQUIRE(cpu.de() == 0x5657);
    REQUIRE(cpu.pc() == 0x0001);
}

// **********************************************
// *       INC HL   ::   OP CODE: 0x23          *
// **********************************************
TEST_CASE_METHOD(CpuFixture,"TEST OP CODE 0x23 :: INC HL - Increments the HL register")
{
    // Opcode: 0x23, HL=4321 initially, then after INC HL, HL should be 4321 + 1 = 4322
    cpu.setHL(0x4321);
    bus.write(0x0000, 0x23); // 0x23 is the opcode for INC DE
    cpu.step();

    REQUIRE(cpu.hl() == 0x4322);
    REQUIRE(cpu.pc() == 0x0001);
}

// **********************************************
// *       DEC BC   ::   OP CODE: 0x0B          *
// **********************************************
TEST_CASE_METHOD(CpuFixture,"TEST OP CODE 0x0B :: DEC BC - Decrements the BC register")
{
    // Opcode: 0x0B, BC=0x2222 initially, then after DEC BC, BC should be 2222 - 1 = 2221
    cpu.setBC(0x2222);
    bus.write(0x0000, 0x0B); // 0x0B is the opcode for DEC BC
    cpu.step();

    REQUIRE(cpu.bc() == 0x2221);
    REQUIRE(cpu.pc() == 0x0001);
}

// **********************************************
// *       DEC DE   ::   OP CODE: 0x1B          *
// **********************************************
TEST_CASE_METHOD(CpuFixture,"TEST OP CODE 0x1B :: DEC DE - Decrements the DE register")
{
    // Opcode: 0x1B, DE=8765 initially, then after DEC DE, DE should be 8765 - 1 = 8764
    cpu.setDE(0x8765);
    bus.write(0x0000, 0x1B); // 0x1B is the opcode for DEC DE
    cpu.step();

    REQUIRE(cpu.de() == 0x8764);
    REQUIRE(cpu.pc() == 0x0001);
}

// **********************************************
// *       DEC HL   ::   OP CODE: 0x2B          *
// **********************************************
TEST_CASE_METHOD(CpuFixture,"TEST OP CODE 0x2B :: DEC HL - Decrements the HL register")
{
    // Opcode: 0x2B, DE=EEEF initially, then after DEC DE, DE should be EEEF - 1 = EEEE
    cpu.setHL(0xEEEF);
    bus.write(0x0000, 0x2B); // 0x2B is the opcode for DEC HL
    cpu.step();

    REQUIRE(cpu.hl() == 0xEEEE);
    REQUIRE(cpu.pc() == 0x0001);
}

// **********************************************
// *       LD A,nn   ::   OP CODE: 0x3E         *
// **********************************************
TEST_CASE_METHOD(CpuFixture,"LD A,n loads immediate into A and advances PC")
{
    const auto f_before = cpu.f();

    bus.write(0x0000, 0x3E); // LD A,n
    bus.write(0x0001, 0x56); // n

    cpu.step();

    REQUIRE(cpu.a() == 0x56);
    REQUIRE(cpu.pc() == 0x0002);
    REQUIRE(cpu.f() == f_before);
}

// **********************************************
// *       LD B,nn   ::   OP CODE: 0x06         *
// **********************************************
TEST_CASE_METHOD(CpuFixture,"LD B,n loads immediate into B and advances PC")
{

    const auto f_before = cpu.f();

    bus.write(0x0000, 0x06); // LD B,n
    bus.write(0x0001, 0x33); // n

    cpu.step();

    REQUIRE(cpu.b() == 0x33);
    REQUIRE(cpu.pc() == 0x0002);
    REQUIRE(cpu.f() == f_before);
}

// **********************************************
// *       LD C,nn   ::   OP CODE: 0x0E         *
// **********************************************
TEST_CASE_METHOD(CpuFixture,"LD C,n loads immediate into C and advances PC")
{
    const auto f_before = cpu.f();

    bus.write(0x0000, 0x0E); // LD C,n
    bus.write(0x0001, 0x88); // n

    cpu.step();

    REQUIRE(cpu.c() == 0x88);
    REQUIRE(cpu.pc() == 0x0002);
    REQUIRE(cpu.f() == f_before);
}

// **********************************************
// *       LD D,nn   ::   OP CODE: 0x16         *
// **********************************************
TEST_CASE_METHOD(CpuFixture,"LD D,n loads immediate into D and advances PC")
{
    const auto f_before = cpu.f();

    bus.write(0x0000, 0x16); // LD D,n
    bus.write(0x0001, 0x77); // n

    cpu.step();

    REQUIRE(cpu.d() == 0x77);
    REQUIRE(cpu.pc() == 0x0002);
    REQUIRE(cpu.f() == f_before);
}

// **********************************************
// *       LD E,nn   ::   OP CODE: 0x1E         *
// **********************************************
TEST_CASE_METHOD(CpuFixture,"LD E,n loads immediate into E and advances PC")
{
    const auto f_before = cpu.f();

    bus.write(0x0000, 0x1E); // LD E,n
    bus.write(0x0001, 0x33); // n

    cpu.step();

    REQUIRE(cpu.e() == 0x33);
    REQUIRE(cpu.pc() == 0x0002);
    REQUIRE(cpu.f() == f_before);
}

// **********************************************
// *       LD H,nn   ::   OP CODE: 0x26         *
// **********************************************
TEST_CASE_METHOD(CpuFixture,"LD H,n loads immediate into H and advances PC")
{
    const auto f_before = cpu.f();

    bus.write(0x0000, 0x26); // LD E,n
    bus.write(0x0001, 0x22); // n

    cpu.step();

    REQUIRE(cpu.h() == 0x22);
    REQUIRE(cpu.pc() == 0x0002);
    REQUIRE(cpu.f() == f_before);
}

// **********************************************
// *       LD L,nn   ::   OP CODE: 0x2E         *
// **********************************************
TEST_CASE_METHOD(CpuFixture,"LD L,n loads immediate into L and advances PC")
{
    const auto f_before = cpu.f();

    bus.write(0x0000, 0x2E); // LD L,n
    bus.write(0x0001, 0x22); // n

    cpu.step();

    REQUIRE(cpu.l() == 0x22);
    REQUIRE(cpu.pc() == 0x0002);
    REQUIRE(cpu.f() == f_before);
}

// **********************************************
// *       LD A,B   ::   OP CODE: 0x78          *
// **********************************************
TEST_CASE_METHOD(CpuFixture,"LD A,B loads contents of B into A and advances PC")
{
    cpu.setA(0x00);
    cpu.setB(0x42);
    cpu.setF(0xA5);
    const auto f_before = cpu.f();

    bus.write(0x0000, 0x78); // LD A,B
    cpu.step();

    REQUIRE(cpu.a() == 0x42);
    REQUIRE(cpu.pc() == 0x0001);
    REQUIRE(cpu.f() == 0xA5);
}

// **********************************************
// *       LD r, (HL)   ::   OP CODE: 0x46      *
// **********************************************
TEST_CASE_METHOD(CpuFixture,"LD r,(HL) reads from memory at HL", "[cpu][ld]")
{
    cpu.setHL(0x4000);
    bus.write(0x4000, 0xAB);

    // Put opcode at PC=0
    bus.write(0x0000, 0x46); // LD B,(HL)

    cpu.step();

    REQUIRE(cpu.b() == 0xAB);
}

// **********************************************
// *       LD (HL), r   ::   OP CODE: 0x71      *
// **********************************************
TEST_CASE_METHOD(CpuFixture,"LD (HL),r writes to memory at HL", "[cpu][ld]")
{
    cpu.setHL(0x4000);
    cpu.setC(0x5E);

    bus.write(0x0000, 0x71); // LD (HL),C

    cpu.step();

    REQUIRE(bus.read(0x4000) == 0x5E);
}

// **********************************************
// *   SET HALT STATE   ::   OP CODE: 0x76      *
// **********************************************
TEST_CASE_METHOD(CpuFixture,"0x76 sets HALT state (not treated as LD (HL),(HL))", "[cpu][halt]")
{
    bus.write(0x0000, 0x76); // HALT

    cpu.step();

    REQUIRE(cpu.isHalted() == true);
}

// **********************************************
// *        INC B   ::   OP CODE: 0x04          *
// **********************************************
TEST_CASE_METHOD(CpuFixture,"INC B sets flags correctly on overflow 0x7F -> 0x80", "[cpu][inc]")
{
    cpu.setB(0x7F);
    bus.write(0x0000, 0x04); // INC B
    cpu.step();

    REQUIRE(cpu.b() == 0x80);

    const uint8_t f = cpu.f();
    REQUIRE((f & 0x80) != 0); // S
    REQUIRE((f & 0x40) == 0); // Z
    REQUIRE((f & 0x10) != 0); // H
    REQUIRE((f & 0x04) != 0); // PV
    REQUIRE((f & 0x02) == 0); // N
}

// **********************************************
// *        DEC B   ::   OP CODE: 0x05          *
// **********************************************
TEST_CASE_METHOD(CpuFixture,"DEC B sets flags correctly on overflow 0x80 -> 0x7F", "[cpu][dec]")
{
    cpu.setB(0x80);
    bus.write(0x0000, 0x05); // DEC B
    cpu.step();

    REQUIRE(cpu.b() == 0x7F);

    const uint8_t f = cpu.f();
    REQUIRE((f & 0x80) == 0); // S
    REQUIRE((f & 0x40) == 0); // Z
    REQUIRE((f & 0x10) != 0); // H
    REQUIRE((f & 0x04) != 0); // PV
    REQUIRE((f & 0x02) != 0); // N
}

// **********************************************
// *      INC (HL)   ::   OP CODE: 0x34         *
// **********************************************
TEST_CASE_METHOD(CpuFixture, "0x34 :: INC (HL)", "[cpu][inc][mem]")
{
    cpu.setHL(0x4000);
    bus.write(0x4000, 0x0F);

    bus.write(0x0000, 0x34); // INC (HL)
    cpu.step();

    REQUIRE(bus.read(0x4000) == 0x10);
    REQUIRE((cpu.f() & 0x10) != 0); // H set
}

// **********************************************
// *      DEC (HL)   ::   OP CODE: 0x35         *
// **********************************************
TEST_CASE_METHOD(CpuFixture, "0x35 :: DEC (HL)", "[cpu][dec][mem]")
{
    cpu.setHL(0x4000);
    bus.write(0x4000, 0x00);

    bus.write(0x0000, 0x35); // DEC (HL)
    cpu.step();

    REQUIRE(bus.read(0x4000) == 0xFF);
    REQUIRE((cpu.f() & 0x10) != 0); // H set on borrow from bit 4 when low nibble was 0
    REQUIRE((cpu.f() & 0x02) != 0); // N set
}

// **********************************************
// *         INC does not change Carry          *
// **********************************************
TEST_CASE_METHOD(CpuFixture, "INC does not change Carry", "[cpu][inc][flags]")
{
    // Set carry using SCF
    bus.write(0x0000, 0x37); // SCF
    bus.write(0x0001, 0x04); // INC B

    cpu.setB(0x00);

    cpu.step(); // SCF
    REQUIRE((cpu.f() & 0x01) != 0); // C set

    cpu.step(); // INC B
    REQUIRE(cpu.b() == 0x01);
    REQUIRE((cpu.f() & 0x01) != 0); // C still set
}

// **********************************************
// *         DEC does not change Carry          *
// **********************************************

TEST_CASE_METHOD(CpuFixture, "DEC (HL) does not change Carry", "[cpu][dec][mem][flags]")
{
    cpu.setHL(0x4000);
    bus.write(0x4000, 0x01);

    bus.write(0x0000, 0x37); // SCF
    bus.write(0x0001, 0x35); // DEC (HL)

    cpu.step(); // SCF
    REQUIRE((cpu.f() & 0x01) != 0); // C set

    cpu.step(); // DEC (HL)
    REQUIRE(bus.read(0x4000) == 0x00);
    REQUIRE((cpu.f() & 0x01) != 0); // C still set
}