#include "Cpu.h"
#include "Bus.h"

void Cpu::connect(Bus* bus)
{
    bus_ = bus;
}

void Cpu::reset(uint16_t pc)
{
    pc_ = pc;
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

std::uint8_t Cpu::b() const
{
    return static_cast<std::uint8_t>(bc_ >> 8);
}

std::uint8_t Cpu::c() const
{
    return static_cast<std::uint8_t>(bc_);
}

std::uint8_t Cpu::d() const
{
    return static_cast<std::uint8_t>(de_ >> 8);
}

std::uint8_t Cpu::e() const
{
    return static_cast<std::uint8_t>(de_);
}

std::uint8_t Cpu::f() const
{
    return static_cast<std::uint8_t>(af_ & 0x00FF);
}

std::uint8_t Cpu::h() const
{
    return static_cast<std::uint8_t>(hl_ >> 8);
}

std::uint8_t Cpu::l() const
{
    return static_cast<std::uint8_t>(hl_);
}

void Cpu::setA(std::uint8_t value)
{
    af_ = (af_ & 0x00FFu) | (uint16_t(value) << 8);
}

void Cpu::setB(std::uint8_t value)
{
    bc_ = (bc_ & 0x00FFu) | (uint16_t(value) << 8);
}

void Cpu::setC(std::uint8_t value)
{
    bc_ = (bc_ & 0xFF00u) | (uint16_t(value));
}

void Cpu::setD(std::uint8_t value)
{
    de_ = (de_ & 0xFF00u) | (uint16_t(value) << 8);
}

void Cpu::setE(std::uint8_t value)
{
    de_ = (de_ & 0xFF00u) | (uint16_t(value));
}

void Cpu::setF(std::uint8_t value)
{
    af_ = static_cast<std::uint16_t>((af_ & 0xFF00) | value);
}

void Cpu::setH(std::uint8_t value)
{
    hl_ = (hl_ & 0x00FFu) | (uint16_t(value) << 8);
}

void Cpu::setL(std::uint8_t value)
{
    hl_ = static_cast<std::uint16_t>((hl_ & 0xFF00) | value);
}

void Cpu::loadImm16(void (Cpu::* setter)(uint16_t))
{
    (this->*setter)(fetchWord());
}

void Cpu::loadImm8(void (Cpu::* setter)(uint8_t))
{
    (this->*setter)(fetchByte());
}

void Cpu::execLdRegReg(uint8_t opcode)
{
    const uint8_t dst = (opcode >> 3) & 0x07;
    const uint8_t src = opcode & 0x07;

    // Skip (HL) for now
    if (dst == 6 || src == 6)
        return; // we'll implement later with memory reads/writes

    const uint8_t value = (this->*reg8Get[src])();
    (this->*reg8Set[dst])(value);
	const uint8_t xxxx = 0xA;
}

void Cpu::step()
{
    uint8_t opcode = fetchByte();

    // LD r,r' block (0x40–0x7F except 0x76 (HALT))
	// Used to cover the 49 'ld r,r' instructions.
    if (opcode >= 0x40 && opcode <= 0x7F && opcode != 0x76)
    {
        execLdRegReg(opcode);
        return;
    }

	switch (opcode)
    {
	    case 0x01:                      // LD BC,nn
	        loadImm16(&Cpu::setBC);
	        break;

	    case 0x11:                      // LD DE,nn
	        loadImm16(&Cpu::setDE);
	        break;

	    case 0x21:                      // LD HL,nn
	        loadImm16(&Cpu::setHL);
	        break;

	    case 0x31: // LD SP,nn
	        loadImm16(&Cpu::setSP);     // LD SP,nn
	        break;						// is a special case since SP doesn't have a corresponding getter like the other registers,
									    // but we can still use the same ld_rr_nn helper function to fetch the 16-bit immediate value and set SP accordingly.

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

	    case 0x3E:                      // LD A,n
	        loadImm8(&Cpu::setA);
	        break;

	    case 0x06:                      // LD B,n
	        loadImm8(&Cpu::setB);
	        break;

	    case 0x0E:                      // LD C,n
	        loadImm8(&Cpu::setC);
	        break;

	    case 0x16:                      // LD D,n
	        loadImm8(&Cpu::setD);
	        break;

	    case 0x1E:                      // LD D,n
	        loadImm8(&Cpu::setE);
	        break;

	    case 0x26:                      // LD D,n
	        loadImm8(&Cpu::setH);
	        break;

	    case 0x2E:                      // LD D,n
	        loadImm8(&Cpu::setL);
	        break;

	    default:
	        // TODO :: For now, do nothing (we’ll tighten this later)
	        break;
    }
}

