#include "Cpu.h"
#include "Bus.h"

void Cpu::connect(Bus* bus)
{
	bus_ = bus;
}

void Cpu::reset()
{
	pc_ = 0x0000;
    sp_ = 0xFFFF;
}

bool Cpu::isConnected() const
{
    return bus_ != nullptr;
}

std::uint8_t Cpu::popByte()
{
    const auto value = bus_->read(sp_);
    ++sp_;
    return value;
}

void Cpu::pushByte(std::uint8_t value)
{
    --sp_;                      // stack grows downward
    bus_->write(sp_, value);
}

std::uint8_t Cpu::fetchByte()
{
    //TODO :: (Decide how to handle “not connected” in a later step.)
    const auto value = bus_->read(pc_);
    pc_++;
    return value;
}

std::uint16_t Cpu::fetchWord()
{
    const std::uint16_t lo = fetchByte();
    const std::uint16_t hi = fetchByte();
    return static_cast<std::uint16_t>((hi << 8) | lo);
}

std::uint8_t Cpu::a() const
{
    return static_cast<std::uint8_t>(af_ >> 8);
}

std::uint8_t Cpu::f() const
{
    return static_cast<std::uint8_t>(af_ & 0x00FF);
}

void Cpu::setA(std::uint8_t value)
{
    af_ = static_cast<std::uint16_t>((af_ & 0x00FF) | (static_cast<std::uint16_t>(value) << 8));
}

void Cpu::setF(std::uint8_t value)
{
    af_ = static_cast<std::uint16_t>((af_ & 0xFF00) | value);
}

void Cpu::step()
{
    const auto opcode = fetchByte();

    switch (opcode)
    {
    case 0x00:                      // NOP
        break;
    case 0x01:                      // LD BC,nn
        setBC(fetchWord());
        break;
    case 0x11:                      // LD DE,nn
        setDE(fetchWord());
        break;
    case 0x21:                      // LD HL,nn
        setHL(fetchWord());
        break;
    case 0x31:                      // LD SP,nn
        sp_ = fetchWord();
        break;
    case 0x03:                      // INC BC
        setBC(static_cast<std::uint16_t>(bc() + 1));
        break;

    case 0x13:                      // INC DE
        setDE(static_cast<std::uint16_t>(de() + 1));
        break;

    case 0x23:                      // INC HL
        setHL(static_cast<std::uint16_t>(hl() + 1));
        break;

    case 0x33:                      // INC SP
        sp_ = static_cast<std::uint16_t>(sp_ + 1);
        break;

    case 0x0B:                      // DEC BC
        setBC(static_cast<std::uint16_t>(bc() - 1));
        break;

    case 0x1B:                      // DEC DE
        setDE(static_cast<std::uint16_t>(de() - 1));
        break;

    case 0x2B:                      // DEC HL
        setHL(static_cast<std::uint16_t>(hl() - 1));
        break;

    case 0x3B:                      // DEC SP
        sp_ = static_cast<std::uint16_t>(sp_ - 1);
        break;
    default:
        // TODO :: For now, do nothing (we’ll tighten this later)
        break;
    }
}

