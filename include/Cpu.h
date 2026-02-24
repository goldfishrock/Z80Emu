#pragma once
#include <cstdint>
#include <array>

class Bus;

class Cpu
{
public:
    void reset();
    void reset(uint16_t pc);
    void connect(Bus* bus);
    void pushByte(std::uint8_t value);
    void setA(std::uint8_t value);
    void setB(std::uint8_t value);
    void setC(std::uint8_t value);
    void setD(std::uint8_t value);
    void setE(std::uint8_t value);
    void setF(std::uint8_t value);
    void setH(std::uint8_t value);
    void setL(std::uint8_t value);
    void step();

    bool isConnected() const;
    void setFlag(uint8_t mask, bool on);
    bool isHalted() const { return halted_; }

    // Define the 8-bit registers
    std::uint8_t popByte();
    std::uint8_t fetchByte();
    std::uint8_t a() const;
    std::uint8_t b() const;
    std::uint8_t c() const;
    std::uint8_t d() const;
    std::uint8_t e() const;
    std::uint8_t f() const;
    std::uint8_t h() const;
    std::uint8_t l() const;

    // Define the 16-bit registers
    std::uint16_t af() const { return af_; }
    std::uint16_t bc() const { return bc_; }
    std::uint16_t de() const { return de_; }
    std::uint16_t hl() const { return hl_; }
    std::uint16_t pc() const { return pc_; }
    std::uint16_t sp() const { return sp_; }
    std::uint16_t fetchWord();

    // Flag masks (standard Z80)
    static constexpr uint8_t FLAG_S = 0x80;
    static constexpr uint8_t FLAG_Z = 0x40;
    static constexpr uint8_t FLAG_H = 0x10;
    static constexpr uint8_t FLAG_PV = 0x04;
    static constexpr uint8_t FLAG_N = 0x02;
    static constexpr uint8_t FLAG_C = 0x01;

    // Public setters for the registers
    void setAF(std::uint16_t value) { af_ = value; }
    void setBC(std::uint16_t value) { bc_ = value; }
    void setDE(std::uint16_t value) { de_ = value; }
    void setHL(std::uint16_t value) { hl_ = value; }
    void setSP(std::uint16_t value) { sp_ = value; }

private:
    Bus* bus_ = nullptr;
    std::uint16_t pc_ = 0;
    std::uint16_t sp_ = 0;
    std::uint16_t af_ = 0;
    std::uint16_t bc_ = 0;
    std::uint16_t de_ = 0;
    std::uint16_t hl_ = 0;

    bool halted_ = false;

    void loadImm8(void (Cpu::* setter)(uint8_t));
    void execScf();
    void execIncReg(uint8_t opcode);
    void execDecReg(uint8_t opcode);
    void execLdRegReg(uint8_t opcode);
    void loadImm16(void (Cpu::* setter)(uint16_t));

    uint8_t inc8(uint8_t v);
    uint8_t dec8(uint8_t v);

    using Reg8Getter = uint8_t(Cpu::*)() const;
    using Reg8Setter = void (Cpu::*)(uint8_t);

    static constexpr std::array<Reg8Getter, 8> reg8Get = {
    &Cpu::b, &Cpu::c, &Cpu::d, &Cpu::e, &Cpu::h, &Cpu::l, nullptr, &Cpu::a
    };

    static constexpr std::array<Reg8Setter, 8> reg8Set = {
        &Cpu::setB, &Cpu::setC, &Cpu::setD, &Cpu::setE, &Cpu::setH, &Cpu::setL, nullptr, &Cpu::setA
    };
};
    