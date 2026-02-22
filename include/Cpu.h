#pragma once
#include <cstdint>

class Bus;

class Cpu {
public:
    void reset();
    void connect(Bus* bus);
    void pushByte(std::uint8_t value);
    void setA(std::uint8_t value);
    void setF(std::uint8_t value);
	void step();

	bool isConnected() const;

    std::uint8_t popByte();
    std::uint8_t fetchByte();
    std::uint8_t a() const;
    std::uint8_t f() const;

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
};