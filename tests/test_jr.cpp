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
// *        JR e       ::    OP CODE: 0x18      *
// **********************************************
// *                                            *
// *     Relative Jump (Signed 8-bit Offset)    *
// *                                            *
// **********************************************
TEST_CASE_METHOD(CpuFixture, "JR e (0x18) jumps forward by signed displacement", "[flow][jr]")
{
    // Program at 0x0000:
    // 0000: 18 02   JR +2
    // After fetch PC = 0x0002, +2 => 0x0004
    bus.Write(0x0000, 0x18);
    bus.Write(0x0001, 0x02);

    cpu.SetPc(0x0000);

    cpu.Step();

    REQUIRE(cpu.GetPc() == 0x0004);
}

// **********************************************
// *        JR e       ::    OP CODE: 0x18      *
// **********************************************
// *                                            *
// *     Relative Jump (Signed 8-bit Offset)    *
// *                                            *
// **********************************************
TEST_CASE_METHOD(CpuFixture, "JR e (0x18) jumps backward by signed displacement", "[flow][jr]")
{
    // Put JR at 0x0010: JR -2 should jump back to 0x0010 (infinite loop)
    // After fetch PC = 0x0012, +(-2) => 0x0010
    bus.Write(0x0010, 0x18);
    bus.Write(0x0011, 0xFE); // -2

    cpu.SetPc(0x0010);

    cpu.Step();

    REQUIRE(cpu.GetPc() == 0x0010);
}

// **********************************************
// *        JR e       ::    OP CODE: 0x18      *
// **********************************************
// *                                            *
// *     Relative Jump (Signed 8-bit Offset)    *
// *                                            *
// **********************************************
TEST_CASE_METHOD(CpuFixture, "JR e (0x18) with displacement 0 stays at next instruction", "[flow][jr]")
{
    // JR +0: after fetch PC = 0x0002, +0 => 0x0002
    bus.Write(0x0000, 0x18);
    bus.Write(0x0001, 0x00);

    cpu.SetPc(0x0000);

    cpu.Step();

    REQUIRE(cpu.GetPc() == 0x0002);
}

// **********************************************
// *     JR NZ, e    ::    OP CODE: 0x20        *
// **********************************************
// *                                            *
// *   Relative Jump if Z flag is NOT set       *
// *                                            *
// **********************************************
TEST_CASE_METHOD(CpuFixture, "JR NZ,e (0x20) jumps when Z flag is clear", "[flow][jr][nz]")
{
    // 0000: 20 02  JR NZ, +2
    // After fetch PC = 0x0002, +2 => 0x0004
    bus.Write(0x0000, 0x20);
    bus.Write(0x0001, 0x02);

    cpu.SetPc(0x0000);
    cpu.SetFlag(Cpu::FLAG_Z, false);   // NZ condition met

    cpu.Step();

    REQUIRE(cpu.GetPc() == 0x0004);
}

// **********************************************
// *     JR NZ, e    ::    OP CODE: 0x20        *
// **********************************************
// *                                            *
// *   Relative Jump if Z flag is NOT set       *
// *                                            *
// **********************************************
TEST_CASE_METHOD(CpuFixture, "JR NZ,e (0x20) does not jump when Z flag is set", "[flow][jr][nz]")
{
    // 0000: 20 7F  JR NZ, +127 (should NOT jump)
    // If not taken, PC should just be 0x0002
    bus.Write(0x0000, 0x20);
    bus.Write(0x0001, 0x7F);

    cpu.SetPc(0x0000);
    cpu.SetFlag(Cpu::FLAG_Z, true);    // NZ condition fails

    cpu.Step();

    REQUIRE(cpu.GetPc() == 0x0002);
}

// **********************************************
// *     JR NZ, e    ::    OP CODE: 0x20        *
// **********************************************
// *                                            *
// *   Relative Jump if Z flag is NOT set       *
// *                                            *
// **********************************************
TEST_CASE_METHOD(CpuFixture, "JR NZ,e (0x20) jumps backwards when Z flag is clear", "[flow][jr][nz]")
{
    // Put JR at 0x0010: JR NZ, -2 should loop back to 0x0010
    // After fetch PC = 0x0012, +(-2) => 0x0010
    bus.Write(0x0010, 0x20);
    bus.Write(0x0011, 0xFE); // -2

    cpu.SetPc(0x0010);
    cpu.SetFlag(Cpu::FLAG_Z, false);

    cpu.Step();

    REQUIRE(cpu.GetPc() == 0x0010);
}

// **********************************************
// *     JR Z, e     ::    OP CODE: 0x28        *
// **********************************************
// *                                            *
// *     Relative Jump if Z flag IS set         *
// *                                            *
// **********************************************
TEST_CASE_METHOD(CpuFixture, "JR Z,e (0x28) jumps when Z flag is set", "[flow][jr][z]")
{
    bus.Write(0x0000, 0x28);
    bus.Write(0x0001, 0x02);

    cpu.SetPc(0x0000);
    cpu.SetFlag(Cpu::FLAG_Z, true);

    cpu.Step();

    REQUIRE(cpu.GetPc() == 0x0004);
}

// **********************************************
// *     JR Z, e     ::    OP CODE: 0x28        *
// **********************************************
// *                                            *
// *     Relative Jump if Z flag IS set         *
// *                                            *
// **********************************************
TEST_CASE_METHOD(CpuFixture, "JR Z,e (0x28) does not jump when Z flag is clear", "[flow][jr][z]")
{
    bus.Write(0x0000, 0x28);
    bus.Write(0x0001, 0x7F);

    cpu.SetPc(0x0000);
    cpu.SetFlag(Cpu::FLAG_Z, false);

    cpu.Step();

    REQUIRE(cpu.GetPc() == 0x0002);
}

// **********************************************
// *     JR NC, e    ::    OP CODE: 0x30        *
// **********************************************
// *                                            *
// *   Relative Jump if C flag is NOT set       *
// *                                            *
// **********************************************
TEST_CASE_METHOD(CpuFixture, "JR NC,e (0x30) jumps when C flag is clear", "[flow][jr][nc]")
{
    bus.Write(0x0000, 0x30);
    bus.Write(0x0001, 0x02);

    cpu.SetPc(0x0000);
    cpu.SetFlag(Cpu::FLAG_C, false);

    cpu.Step();

    REQUIRE(cpu.GetPc() == 0x0004);
}

// **********************************************
// *     JR NC, e    ::    OP CODE: 0x30        *
// **********************************************
// *                                            *
// *   Relative Jump if C flag is NOT set       *
// *                                            *
// **********************************************
TEST_CASE_METHOD(CpuFixture, "JR NC,e (0x30) does not jump when C flag is set", "[flow][jr][nc]")
{
    bus.Write(0x0000, 0x30);
    bus.Write(0x0001, 0x7F);

    cpu.SetPc(0x0000);
    cpu.SetFlag(Cpu::FLAG_C, true);

    cpu.Step();

    REQUIRE(cpu.GetPc() == 0x0002);
}

// **********************************************
// *     JR C, e     ::    OP CODE: 0x38        *
// **********************************************
// *                                            *
// *     Relative Jump if C flag IS set         *
// *                                            *
// **********************************************
TEST_CASE_METHOD(CpuFixture, "JR C,e (0x38) jumps when C flag is set", "[flow][jr][c]")
{
    bus.Write(0x0000, 0x38);
    bus.Write(0x0001, 0x02);

    cpu.SetPc(0x0000);
    cpu.SetFlag(Cpu::FLAG_C, true);

    cpu.Step();

    REQUIRE(cpu.GetPc() == 0x0004);
}

// **********************************************
// *     JR C, e     ::    OP CODE: 0x38        *
// **********************************************
// *                                            *
// *     Relative Jump if C flag IS set         *
// *                                            *
// **********************************************
TEST_CASE_METHOD(CpuFixture, "JR C,e (0x38) does not jump when C flag is clear", "[flow][jr][c]")
{
    bus.Write(0x0000, 0x38);
    bus.Write(0x0001, 0x7F);

    cpu.SetPc(0x0000);
    cpu.SetFlag(Cpu::FLAG_C, false);

    cpu.Step();

    REQUIRE(cpu.GetPc() == 0x0002);
}