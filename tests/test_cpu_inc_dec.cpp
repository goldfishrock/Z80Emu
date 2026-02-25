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
// *       INC BC   ::   OP CODE: 0x03          *
// **********************************************
TEST_CASE_METHOD(CpuFixture, "TEST OP CODE 0x03 :: INC BC - Increments the BC register")
{
    // Opcode: 0x03, BC=ABCD initially, then after INC BC, BC should be ABCD + 1 = ABCE
    cpu.SetBc(0xABCD);
    bus.Write(0x0000, 0x03); // 0x03 is the opcode for INC BC
    cpu.Step();

    REQUIRE(cpu.GetBc() == 0xABCE);
    REQUIRE(cpu.GetPc() == 0x0001);
}

// **********************************************
// *       INC DE   ::   OP CODE: 0x13          *
// **********************************************
TEST_CASE_METHOD(CpuFixture, "TEST OP CODE 0x13 INC DE :: Increments the DE register")
{
    // Opcode: 0x13, DE=5656 initially, then after INC DE, DE should be 5656 + 1 = 5657
    cpu.SetDe(0x5656);
    bus.Write(0x0000, 0x13); // 0x03 is the opcode for INC DE
    cpu.Step();

    REQUIRE(cpu.GetDe() == 0x5657);
    REQUIRE(cpu.GetPc() == 0x0001);
}

// **********************************************
// *       INC HL   ::   OP CODE: 0x23          *
// **********************************************
TEST_CASE_METHOD(CpuFixture, "TEST OP CODE 0x23 :: INC HL - Increments the HL register")
{
    // Opcode: 0x23, HL=4321 initially, then after INC HL, HL should be 4321 + 1 = 4322
    cpu.SetHl(0x4321);
    bus.Write(0x0000, 0x23); // 0x23 is the opcode for INC DE
    cpu.Step();

    REQUIRE(cpu.GetHl() == 0x4322);
    REQUIRE(cpu.GetPc() == 0x0001);
}

// **********************************************
// *       DEC BC   ::   OP CODE: 0x0B          *
// **********************************************
TEST_CASE_METHOD(CpuFixture, "TEST OP CODE 0x0B :: DEC BC - Decrements the BC register")
{
    // Opcode: 0x0B, BC=0x2222 initially, then after DEC BC, BC should be 2222 - 1 = 2221
    cpu.SetBc(0x2222);
    bus.Write(0x0000, 0x0B); // 0x0B is the opcode for DEC BC
    cpu.Step();

    REQUIRE(cpu.GetBc() == 0x2221);
    REQUIRE(cpu.GetPc() == 0x0001);
}

// **********************************************
// *       DEC DE   ::   OP CODE: 0x1B          *
// **********************************************
TEST_CASE_METHOD(CpuFixture, "TEST OP CODE 0x1B :: DEC DE - Decrements the DE register")
{
    // Opcode: 0x1B, DE=8765 initially, then after DEC DE, DE should be 8765 - 1 = 8764
    cpu.SetDe(0x8765);
    bus.Write(0x0000, 0x1B); // 0x1B is the opcode for DEC DE
    cpu.Step();

    REQUIRE(cpu.GetDe() == 0x8764);
    REQUIRE(cpu.GetPc() == 0x0001);
}

// **********************************************
// *       DEC HL   ::   OP CODE: 0x2B          *
// **********************************************
TEST_CASE_METHOD(CpuFixture, "TEST OP CODE 0x2B :: DEC HL - Decrements the HL register")
{
    // Opcode: 0x2B, DE=EEEF initially, then after DEC DE, DE should be EEEF - 1 = EEEE
    cpu.SetHl(0xEEEF);
    bus.Write(0x0000, 0x2B); // 0x2B is the opcode for DEC HL
    cpu.Step();

    REQUIRE(cpu.GetHl() == 0xEEEE);
    REQUIRE(cpu.GetPc() == 0x0001);
}

// **********************************************
// *        INC B   ::   OP CODE: 0x04          *
// **********************************************
TEST_CASE_METHOD(CpuFixture, "INC B sets flags correctly on overflow 0x7F -> 0x80", "[cpu][inc]")
{
    cpu.SetB(0x7F);
    bus.Write(0x0000, 0x04); // INC B
    cpu.Step();

    REQUIRE(cpu.GetB() == 0x80);

    const uint8_t f = cpu.GetF();
    REQUIRE((f & 0x80) != 0); // S
    REQUIRE((f & 0x40) == 0); // Z
    REQUIRE((f & 0x10) != 0); // H
    REQUIRE((f & 0x04) != 0); // PV
    REQUIRE((f & 0x02) == 0); // N
}

// **********************************************
// *        DEC B   ::   OP CODE: 0x05          *
// **********************************************
TEST_CASE_METHOD(CpuFixture, "DEC B sets flags correctly on overflow 0x80 -> 0x7F", "[cpu][dec]")
{
    cpu.SetB(0x80);
    bus.Write(0x0000, 0x05); // DEC B
    cpu.Step();

    REQUIRE(cpu.GetB() == 0x7F);

    const uint8_t f = cpu.GetF();
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
    cpu.SetHl(0x4000);
    bus.Write(0x4000, 0x0F);

    bus.Write(0x0000, 0x34); // INC (HL)
    cpu.Step();

    REQUIRE(bus.Read(0x4000) == 0x10);
    REQUIRE((cpu.GetF() & 0x10) != 0); // H set
}

// **********************************************
// *      DEC (HL)   ::   OP CODE: 0x35         *
// **********************************************
TEST_CASE_METHOD(CpuFixture, "0x35 :: DEC (HL)", "[cpu][dec][mem]")
{
    cpu.SetHl(0x4000);
    bus.Write(0x4000, 0x00);

    bus.Write(0x0000, 0x35); // DEC (HL)
    cpu.Step();

    REQUIRE(bus.Read(0x4000) == 0xFF);
    REQUIRE((cpu.GetF() & 0x10) != 0); // H set on borrow from bit 4 when low nibble was 0
    REQUIRE((cpu.GetF() & 0x02) != 0); // N set
}

// **********************************************
// *         INC does not change Carry          *
// **********************************************
TEST_CASE_METHOD(CpuFixture, "INC does not change Carry", "[cpu][inc][flags]")
{
    // Set carry using SCF
    bus.Write(0x0000, 0x37); // SCF
    bus.Write(0x0001, 0x04); // INC B

    cpu.SetB(0x00);

    cpu.Step(); // SCF
    REQUIRE((cpu.GetF() & 0x01) != 0); // C set

    cpu.Step(); // INC B
    REQUIRE(cpu.GetB() == 0x01);
    REQUIRE((cpu.GetF() & 0x01) != 0); // C still set
}

// **********************************************
// *         DEC does not change Carry          *
// **********************************************
TEST_CASE_METHOD(CpuFixture, "DEC (HL) does not change Carry", "[cpu][dec][mem][flags]")
{
    cpu.SetHl(0x4000);
    bus.Write(0x4000, 0x01);

    bus.Write(0x0000, 0x37); // SCF
    bus.Write(0x0001, 0x35); // DEC (HL)

    cpu.Step(); // SCF
    REQUIRE((cpu.GetF() & 0x01) != 0); // C set

    cpu.Step(); // DEC (HL)
    REQUIRE(bus.Read(0x4000) == 0x00);
    REQUIRE((cpu.GetF() & 0x01) != 0); // C still set
}