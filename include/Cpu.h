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

    std::uint16_t fetchWord();
    std::uint16_t pc() const { return pc_; }
    std::uint16_t sp() const { return sp_; }

    // Define the registers
    std::uint16_t af() const { return af_; }
    std::uint16_t bc() const { return bc_; }
    std::uint16_t de() const { return de_; }
    std::uint16_t hl() const { return hl_; }

    // Public setters for the registers
    void setAF(std::uint16_t value) { af_ = value; }
    void setBC(std::uint16_t value) { bc_ = value; }
    void setDE(std::uint16_t value) { de_ = value; }
    void setHL(std::uint16_t value) { hl_ = value; }

    // For SP, we only have a setter since it's typically modified by instructions rather than directly read/written
    void setSP(std::uint16_t value) { sp_ = value; }

private:
    Bus* bus_ = nullptr;
    std::uint16_t pc_ = 0;
    std::uint16_t sp_ = 0;
    std::uint16_t af_ = 0;
    std::uint16_t bc_ = 0;
    std::uint16_t de_ = 0;
    std::uint16_t hl_ = 0;

    void loadImm8(void (Cpu::* setter)(uint8_t));
    void execLdRegReg(uint8_t opcode);
    void loadImm16(void (Cpu::* setter)(uint16_t));

    using Reg8Getter = uint8_t(Cpu::*)() const;
    using Reg8Setter = void (Cpu::*)(uint8_t);

    static constexpr std::array<Reg8Getter, 8> reg8Get = {
    &Cpu::b, &Cpu::c, &Cpu::d, &Cpu::e, &Cpu::h, &Cpu::l, nullptr, &Cpu::a
    };

    static constexpr std::array<Reg8Setter, 8> reg8Set = {
        &Cpu::setB, &Cpu::setC, &Cpu::setD, &Cpu::setE, &Cpu::setH, &Cpu::setL, nullptr, &Cpu::setA
    };
};
    