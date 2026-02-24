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
	halted_ = false;
}

bool Cpu::isConnected() const
{
    return bus_ != nullptr;
}

void Cpu::setFlag(uint8_t mask, bool on)
{
	std::uint8_t flags = f();
	if (on) flags |= mask;
	else    flags = static_cast<std::uint8_t>(flags & static_cast<std::uint8_t>(~mask));
	setF(flags);
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

void Cpu::execScf()
{
	// C=1, N=0, H=0. Everything else unchanged.
	setFlag(FLAG_C, true);
	setFlag(FLAG_N, false);
	setFlag(FLAG_H, false);
}

void Cpu::execIncReg(uint8_t opcode)
{
	const uint8_t r = (opcode >> 3) & 0x07;

	if (r == 6)
	{
		// INC (HL)
		const std::uint16_t addr = hl_;                 // since we store hl_ directly
		const std::uint8_t  v = bus_->read(addr);
		const std::uint8_t  res = inc8(v);
		bus_->write(addr, res);
		return;
	}

	const std::uint8_t v = (this->*reg8Get[r])();
	(this->*reg8Set[r])(inc8(v));
}

void Cpu::execDecReg(uint8_t opcode)
{
	const std::uint8_t r = (opcode >> 3) & 0x07;

	if (r == 6)
	{
		// DEC (HL)
		const std::uint16_t addr = hl_;
		const std::uint8_t  v = bus_->read(addr);
		const std::uint8_t  res = dec8(v);
		bus_->write(addr, res);
		return;
	}

	const std::uint8_t v = (this->*reg8Get[r])();
	(this->*reg8Set[r])(dec8(v));
}

uint8_t Cpu::inc8(uint8_t v)
{
	const uint8_t r = static_cast<uint8_t>(v + 1);

	setFlag(FLAG_S, (r & 0x80) != 0);
	setFlag(FLAG_Z, r == 0);
	setFlag(FLAG_H, (v & 0x0F) == 0x0F);
	setFlag(FLAG_PV, v == 0x7F);     // +127 -> -128 overflow
	setFlag(FLAG_N, false);         // INC resets N
	// C unchanged

	return r;
}

uint8_t Cpu::dec8(uint8_t v)
{
	const uint8_t r = static_cast<uint8_t>(v - 1);

	setFlag(FLAG_S, (r & 0x80) != 0);
	setFlag(FLAG_Z, r == 0);
	setFlag(FLAG_H, (v & 0x0F) == 0x00);
	setFlag(FLAG_PV, v == 0x80);     // -128 -> +127 overflow
	setFlag(FLAG_N, true);          // DEC sets N
	// C unchanged

	return r;
}

void Cpu::execLdRegReg(uint8_t opcode)
{
    const uint8_t dst = (opcode >> 3) & 0x07;
    const uint8_t src = opcode & 0x07;

	// 0x76 is HALT (LD (HL),(HL) doesn't exist)
	if (dst == 6 && src == 6)
	{
		halted_ = true;
		return;
	}

	const uint16_t hl = hl_; // or however you read HL

	// LD r, (HL)
	if (src == 6)
	{
		const uint8_t value = bus_->read(hl);     // adjust member name if needed
		(this->*reg8Set[dst])(value);
		return;
	}

	// LD (HL), r
	if (dst == 6)
	{
		const uint8_t value = (this->*reg8Get[src])();
		bus_->write(hl, value);                   // adjust member name if needed
		return;
	}

	// LD r, r
	const uint8_t value = (this->*reg8Get[src])();
	(this->*reg8Set[dst])(value);
}

void Cpu::step()
{
    uint8_t opcode = fetchByte();

    // LD r,r' block (0x40–0x7F except 0x76 (HALT))
	// Used to cover the 49 'ld r,r' instructions.
    if (opcode >= 0x40 && opcode <= 0x7F || opcode == 0x76)
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

	    case 0x1E:                      // LD E,n
	        loadImm8(&Cpu::setE);
	        break;

	    case 0x26:                      // LD H,n
	        loadImm8(&Cpu::setH);
	        break;

	    case 0x2E:                      // LD L,n
	        loadImm8(&Cpu::setL);
	        break;
			
		case 0x04: case 0x0C: case 0x14: case 0x1C:	// INC r (including (HL))
		case 0x24: case 0x2C: case 0x34: case 0x3C:
			execIncReg(opcode);
			break;

		case 0x05: case 0x0D: case 0x15: case 0x1D:	// DEC r (including (HL))
		case 0x25: case 0x2D: case 0x35: case 0x3D:
			execDecReg(opcode);
			break;

		case 0x37:						// SCF
			execScf();
			break;

	    default:
	        // TODO :: For now, do nothing (we’ll tighten this later)
	        break;
    }
}

