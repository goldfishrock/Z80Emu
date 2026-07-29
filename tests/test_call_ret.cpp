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
// *      CALL nn     ::    OP CODE: 0xCD       *
// **********************************************
// *                                            *
// *   Push return address, then jump to nn     *
// *                                            *
// **********************************************
TEST_CASE_METHOD(CpuFixture, "CALL nn (0xCD) pushes return address and jumps", "[flow][call]")
{
    // Arrange:
    // 0000: CD 34 12   CALL 0x1234
    bus.Write(0x0000, 0xCD);
    bus.Write(0x0001, 0x34);
    bus.Write(0x0002, 0x12);

    cpu.SetPc(0x0000);
    cpu.SetSp(0xFFFE);  // standard high stack start

    cpu.Step();

    // After CALL:
    // PC = 0x1234
    REQUIRE(cpu.GetPc() == 0x1234);

    // Return address should be 0x0003
    // Z80 pushes high byte first
    REQUIRE(cpu.GetSp() == 0xFFFC);
    REQUIRE(bus.Read(0xFFFD) == 0x00); // high byte
    REQUIRE(bus.Read(0xFFFC) == 0x03); // low byte
}

// **********************************************
// *      CALL nn     ::    OP CODE: 0xCD       *
// **********************************************
// *                                            *
// *   Verifies little-endian push order        *
// *                                            *
// **********************************************
TEST_CASE_METHOD(CpuFixture, "CALL nn (0xCD) pushes correct return address", "[flow][call]")
{
    bus.Write(0x0100, 0xCD);
    bus.Write(0x0101, 0x78);
    bus.Write(0x0102, 0x56);

    cpu.SetPc(0x0100);
    cpu.SetSp(0x8000);

    cpu.Step();

    // Return address should be 0x0103
    REQUIRE(bus.Read(0x7FFF) == 0x01); // high
    REQUIRE(bus.Read(0x7FFE) == 0x03); // low
}

// **********************************************
// *        RET        ::    OP CODE: 0xC9      *
// **********************************************
// *                                            *
// *        Pop address from stack to PC        *
// *                                            *
// **********************************************
TEST_CASE_METHOD(CpuFixture, "RET (0xC9) pops address and sets PC", "[flow][ret]")
{
    // Arrange
    bus.Write(0x0000, 0xC9);  // RET

    cpu.SetPc(0x0000);
    cpu.SetSp(0xFFFC);

    // Simulate return address 0x1234 on stack
    bus.Write(0xFFFC, 0x34); // low
    bus.Write(0xFFFD, 0x12); // high

    cpu.Step();

    REQUIRE(cpu.GetPc() == 0x1234);
    REQUIRE(cpu.GetSp() == 0xFFFE);
}

// **********************************************
// *      RET NZ      ::    OP CODE: 0xC0       *
// **********************************************
// *                                            *
// *   Return when the Zero flag is clear       *
// *                                            *
// **********************************************
TEST_CASE_METHOD(CpuFixture, "RET NZ (0xC0) returns when Z flag is clear", "[flow][ret][conditional]")
{
    // Arrange
    bus.Write(0x0000, 0xC0);  // RET NZ

    cpu.SetPc(0x0000);
    cpu.SetSp(0xFFFC);
    cpu.SetFlag(Cpu::FLAG_Z, false);

    // Simulate return address 0x1234 on stack
    bus.Write(0xFFFC, 0x34); // low
    bus.Write(0xFFFD, 0x12); // high

    // Act
    cpu.Step();

    // Assert
    REQUIRE(cpu.GetPc() == 0x1234);
    REQUIRE(cpu.GetSp() == 0xFFFE);
}

// **********************************************
// *      RET NZ      ::    OP CODE: 0xC0       *
// **********************************************
// *                                            *
// *   Does not return when Z flag is set       *
// *                                            *
// **********************************************
TEST_CASE_METHOD(CpuFixture, "RET NZ (0xC0) does not return when Z flag is set", "[flow][ret][conditional]")
{
    bus.Write(0x0000, 0xC0);

    cpu.SetPc(0x0000);
    cpu.SetSp(0xFFFC);
    cpu.SetFlag(Cpu::FLAG_Z, true);

    // Dummy return address (should never be used)
    bus.Write(0xFFFC, 0x34);
    bus.Write(0xFFFD, 0x12);

    cpu.Step();

    REQUIRE(cpu.GetPc() == 0x0001);
    REQUIRE(cpu.GetSp() == 0xFFFC);
}


// **********************************************
// *      RET Z       ::    OP CODE: 0xC8       *
// **********************************************
// *                                            *
// *     Return when Z flag is set              *
// *                                            *
// **********************************************
TEST_CASE_METHOD(CpuFixture, "RET Z (0xC8) returns when Z flag is set", "[flow][ret][conditional]")
{
    bus.Write(0x0000, 0xC8);

    cpu.SetPc(0x0000);
    cpu.SetSp(0xFFFC);
    cpu.SetFlag(Cpu::FLAG_Z, true);

    bus.Write(0xFFFC, 0x34);
    bus.Write(0xFFFD, 0x12);

    cpu.Step();

    REQUIRE(cpu.GetPc() == 0x1234);
    REQUIRE(cpu.GetSp() == 0xFFFE);
}


// **********************************************
// *      RET Z       ::    OP CODE: 0xC8       *
// **********************************************
// *                                            *
// *   Does not return when Z flag is clear     *
// *                                            *
// **********************************************
TEST_CASE_METHOD(CpuFixture, "RET Z (0xC8) does not return when Z flag is clear", "[flow][ret][conditional]")
{
    bus.Write(0x0000, 0xC8);

    cpu.SetPc(0x0000);
    cpu.SetSp(0xFFFC);
    cpu.SetFlag(Cpu::FLAG_Z, false);

    bus.Write(0xFFFC, 0x34);
    bus.Write(0xFFFD, 0x12);

    cpu.Step();

    REQUIRE(cpu.GetPc() == 0x0001);
    REQUIRE(cpu.GetSp() == 0xFFFC);
}

// **********************************************
// *      RET NC      ::    OP CODE: 0xD0       *
// **********************************************
// *                                            *
// *     Returns when C flag is clear           *
// *                                            *
// **********************************************
TEST_CASE_METHOD(CpuFixture, "RET NC (0xD0) returns when C flag is clear", "[flow][ret][conditional]")
{
    bus.Write(0x0000, 0xD0);

    cpu.SetPc(0x0000);
    cpu.SetSp(0xFFFC);
    cpu.SetFlag(Cpu::FLAG_C, false);

    bus.Write(0xFFFC, 0x34);
    bus.Write(0xFFFD, 0x12);

    cpu.Step();

    REQUIRE(cpu.GetPc() == 0x1234);
    REQUIRE(cpu.GetSp() == 0xFFFE);
}

// **********************************************
// *      RET NC      ::    OP CODE: 0xD0       *
// **********************************************
// *                                            *
// *    Does not return when C flag is set      *
// *                                            *
// **********************************************
TEST_CASE_METHOD(CpuFixture, "RET NC (0xD0) does not return when C flag is set", "[flow][ret][conditional]")
{
    bus.Write(0x0000, 0xD0);

    cpu.SetPc(0x0000);
    cpu.SetSp(0xFFFC);
    cpu.SetFlag(Cpu::FLAG_C, true);

    bus.Write(0xFFFC, 0x34);
    bus.Write(0xFFFD, 0x12);

    cpu.Step();

    REQUIRE(cpu.GetPc() == 0x0001);
    REQUIRE(cpu.GetSp() == 0xFFFC);
}

// **********************************************
// *      RET C       ::    OP CODE: 0xD8       *
// **********************************************
// *                                            *
// *      Returns when C flag is set            *
// *                                            *
// **********************************************
TEST_CASE_METHOD(CpuFixture, "RET C (0xD8) returns when C flag is set", "[flow][ret][conditional]")
{
    bus.Write(0x0000, 0xD8);

    cpu.SetPc(0x0000);
    cpu.SetSp(0xFFFC);
    cpu.SetFlag(Cpu::FLAG_C, true);

    bus.Write(0xFFFC, 0x34);
    bus.Write(0xFFFD, 0x12);

    cpu.Step();

    REQUIRE(cpu.GetPc() == 0x1234);
    REQUIRE(cpu.GetSp() == 0xFFFE);
}

// **********************************************
// *      RET C       ::    OP CODE: 0xD8       *
// **********************************************
// *                                            *
// *   Does not return when C flag is clear     *
// *                                            *
// **********************************************
TEST_CASE_METHOD(CpuFixture, "RET C (0xD8) does not return when C flag is clear", "[flow][ret][conditional]")
{
    bus.Write(0x0000, 0xD8);

    cpu.SetPc(0x0000);
    cpu.SetSp(0xFFFC);
    cpu.SetFlag(Cpu::FLAG_C, false);

    bus.Write(0xFFFC, 0x34);
    bus.Write(0xFFFD, 0x12);

    cpu.Step();

    REQUIRE(cpu.GetPc() == 0x0001);
    REQUIRE(cpu.GetSp() == 0xFFFC);
}

// **********************************************
// *      CALL NZ,nn   ::   OP CODE: 0xC4       *
// **********************************************
// *                                            *
// *   Calls when the Zero flag is clear        *
// *                                            *
// **********************************************
TEST_CASE_METHOD(CpuFixture, "CALL NZ,nn (0xC4) calls when Z flag is clear", "[flow][call][conditional]")
{
    // 0000: C4 34 12   CALL NZ,0x1234
    bus.Write(0x0000, 0xC4);
    bus.Write(0x0001, 0x34);
    bus.Write(0x0002, 0x12);

    cpu.SetPc(0x0000);
    cpu.SetSp(0xFFFE);
    cpu.SetFlag(Cpu::FLAG_Z, false);

    cpu.Step();

    REQUIRE(cpu.GetPc() == 0x1234);
    REQUIRE(cpu.GetSp() == 0xFFFC);
    REQUIRE(bus.Read(0xFFFD) == 0x00); // return address high byte
    REQUIRE(bus.Read(0xFFFC) == 0x03); // return address low byte
}

// **********************************************
// *        CALL nn    ::   OP CODE: 0xCD       *
// **********************************************
// *                                            *
// *   Unconditional subroutine call            *
// *                                            *
// **********************************************
TEST_CASE_METHOD(CpuFixture, "CALL nn (0xCD) calls unconditionally", "[flow][call]")
{
    // 0000: CD 34 12   CALL 0x1234
    bus.Write(0x0000, 0xCD);
    bus.Write(0x0001, 0x34);
    bus.Write(0x0002, 0x12);

    cpu.SetPc(0x0000);
    cpu.SetSp(0xFFFE);

    cpu.Step();

    REQUIRE(cpu.GetPc() == 0x1234);
    REQUIRE(cpu.GetSp() == 0xFFFC);
    REQUIRE(bus.Read(0xFFFD) == 0x00); // return address high byte
    REQUIRE(bus.Read(0xFFFC) == 0x03); // return address low byte
}

// **********************************************
// *      CALL NC,nn   ::   OP CODE: 0xD4       *
// **********************************************
// *                                            *
// *   Calls when the Carry flag is clear       *
// *                                            *
// **********************************************
TEST_CASE_METHOD(CpuFixture, "CALL NC,nn (0xD4) calls when C flag is clear", "[flow][call][conditional]")
{
    // 0000: D4 34 12   CALL NC,0x1234
    bus.Write(0x0000, 0xD4);
    bus.Write(0x0001, 0x34);
    bus.Write(0x0002, 0x12);

    cpu.SetPc(0x0000);
    cpu.SetSp(0xFFFE);
    cpu.SetFlag(Cpu::FLAG_C, false);

    cpu.Step();

    REQUIRE(cpu.GetPc() == 0x1234);
    REQUIRE(cpu.GetSp() == 0xFFFC);
    REQUIRE(bus.Read(0xFFFD) == 0x00);
    REQUIRE(bus.Read(0xFFFC) == 0x03);
}

// **********************************************
// *       CALL Z,nn   ::   OP CODE: 0xCC       *
// **********************************************
// *                                            *
// *   Calls when the Zero flag is set          *
// *                                            *
// **********************************************
TEST_CASE_METHOD(CpuFixture, "CALL Z,nn (0xCC) calls when Z flag is set", "[flow][call][conditional]")
{
    // 0000: CC 34 12   CALL Z,0x1234
    bus.Write(0x0000, 0xCC);
    bus.Write(0x0001, 0x34);
    bus.Write(0x0002, 0x12);

    cpu.SetPc(0x0000);
    cpu.SetSp(0xFFFE);
    cpu.SetFlag(Cpu::FLAG_Z, true);

    cpu.Step();

    REQUIRE(cpu.GetPc() == 0x1234);
    REQUIRE(cpu.GetSp() == 0xFFFC);
    REQUIRE(bus.Read(0xFFFD) == 0x00); // return address high byte
    REQUIRE(bus.Read(0xFFFC) == 0x03); // return address low byte
}

// **********************************************
// *       CALL C,nn   ::   OP CODE: 0xDC       *
// **********************************************
// *                                            *
// *   Calls when the Carry flag is set         *
// *                                            *
// **********************************************
TEST_CASE_METHOD(CpuFixture, "CALL C,nn (0xDC) calls when C flag is set",
                 "[flow][call][conditional]")
{
    // 0000: DC 34 12   CALL C,0x1234
    bus.Write(0x0000, 0xDC);
    bus.Write(0x0001, 0x34);
    bus.Write(0x0002, 0x12);

    cpu.SetPc(0x0000);
    cpu.SetSp(0xFFFE);
    cpu.SetFlag(Cpu::FLAG_C, true);

    cpu.Step();

    REQUIRE(cpu.GetPc() == 0x1234);
    REQUIRE(cpu.GetSp() == 0xFFFC);
    REQUIRE(bus.Read(0xFFFD) == 0x00); // return address high byte
    REQUIRE(bus.Read(0xFFFC) == 0x03); // return address low byte
}