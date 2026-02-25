#include "Bus.h"
#include "Cpu.h"

void Cpu::ExecScf()
{
	// C=1, N=0, H=0. Everything else unchanged.
	SetFlag(FLAG_C, true);
	SetFlag(FLAG_N, false);
	SetFlag(FLAG_H, false);
}

void Cpu::ExecIncReg(uint8_t opcode)
{
	const uint8_t r = (opcode >> 3) & 0x07;

	if (r == 6)
	{
		// INC (HL)
		const std::uint16_t addr = hl_;                 // since we store hl_ directly
		const std::uint8_t  v = bus_->Read(addr);
		const std::uint8_t  res = Inc8(v);
		bus_->Write(addr, res);
		return;
	}

	const std::uint8_t v = (this->*reg8Get[r])();
	(this->*reg8Set[r])(Inc8(v));
}

void Cpu::ExecDecReg(uint8_t opcode)
{
	const std::uint8_t r = (opcode >> 3) & 0x07;

	if (r == 6)
	{
		// DEC (HL)
		const std::uint16_t addr = hl_;
		const std::uint8_t  v = bus_->Read(addr);
		const std::uint8_t  res = Dec8(v);
		bus_->Write(addr, res);
		return;
	}

	const std::uint8_t v = (this->*reg8Get[r])();
	(this->*reg8Set[r])(Dec8(v));
}

void Cpu::ExecLdRegReg(uint8_t opcode)
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
		const uint8_t value = bus_->Read(hl);     // adjust member name if needed
		(this->*reg8Set[dst])(value);
		return;
	}

	// LD (HL), r
	if (dst == 6)
	{
		const uint8_t value = (this->*reg8Get[src])();
		bus_->Write(hl, value);                   // adjust member name if needed
		return;
	}

	// LD r, r
	const uint8_t value = (this->*reg8Get[src])();
	(this->*reg8Set[dst])(value);
}

void Cpu::ExecAddAImm()
{
	const uint8_t lhs = GetA();
	const uint8_t rhs = FetchByte();          // immediate n
	const uint16_t sum = static_cast<uint16_t>(lhs) + rhs;
	const uint8_t result = static_cast<uint8_t>(sum);

	SetFlagsAdd8(lhs, rhs, 0, result);
	SetA(result);
}

void Cpu::ExecSubAReg(uint8_t opcode)
{
	const uint8_t src = opcode & 0x07;

	// Skip (HL) for now
	if (src == 6)
		return;

	const uint8_t lhs = GetA();
	const uint8_t rhs = (this->*reg8Get[src])();

	const uint16_t diff = static_cast<uint16_t>(lhs) - rhs;
	const uint8_t result = static_cast<uint8_t>(diff);

	SetFlagsSub8(lhs, rhs, 0, result);
	SetA(result);
}

void Cpu::ExecSbcAReg(uint8_t opcode)
{
	const uint8_t src = opcode & 0x07;

	// Skip (HL) for now
	if (src == 6)
		return;

	const uint8_t lhs = GetA();
	const uint8_t rhs = (this->*reg8Get[src])();

	const uint8_t carry_in = (GetF() & FLAG_C) ? 1 : 0;

	const uint16_t diff = static_cast<uint16_t>(lhs) - rhs - carry_in;
	const uint8_t result = static_cast<uint8_t>(diff);

	SetFlagsSub8(lhs, rhs, carry_in, result);
	SetA(result);
}

void Cpu::Step()
{
	uint8_t opcode = FetchByte();

	// LD r,r' block (0x40–0x7F except 0x76 (HALT))
	// Used to cover the 49 'ld r,r' instructions.
	if (opcode >= 0x40 && opcode <= 0x7F || opcode == 0x76)
	{
		ExecLdRegReg(opcode);
		return;
	}

	switch (opcode)
	{
	case 0x01:                      // LD BC,nn
		ExecLdRegImm16(&Cpu::SetBc);
		break;

	case 0x11:                      // LD DE,nn
		ExecLdRegImm16(&Cpu::SetDe);
		break;

	case 0x21:                      // LD HL,nn
		ExecLdRegImm16(&Cpu::SetHl);
		break;

	case 0x31: // LD SP,nn
		ExecLdRegImm16(&Cpu::SetSp);     // LD SP,nn
		break;						// is a special case since SP doesn't have a corresponding getter like the other registers,
		// but we can still use the same ld_rr_nn helper function to fetch the 16-bit immediate value and set SP accordingly.

	case 0x03:                      // INC BC
		SetBc(static_cast<std::uint16_t>(GetBc() + 1));
		break;

	case 0x13:                      // INC DE
		SetDe(static_cast<std::uint16_t>(GetDe() + 1));
		break;

	case 0x23:                      // INC HL
		SetHl(static_cast<std::uint16_t>(GetHl() + 1));
		break;

	case 0x33:                      // INC SP
		sp_ = static_cast<std::uint16_t>(sp_ + 1);
		break;

	case 0x0B:                      // DEC BC
		SetBc(static_cast<std::uint16_t>(GetBc() - 1));
		break;

	case 0x1B:                      // DEC DE
		SetDe(static_cast<std::uint16_t>(GetDe() - 1));
		break;

	case 0x2B:                      // DEC HL
		SetHl(static_cast<std::uint16_t>(GetHl() - 1));
		break;

	case 0x3B:                      // DEC SP
		sp_ = static_cast<std::uint16_t>(sp_ - 1);
		break;

	case 0x3E:                      // LD A,n
		ExecLdRegImm8(&Cpu::SetA);
		break;

	case 0x06:                      // LD B,n
		ExecLdRegImm8(&Cpu::SetB);
		break;

	case 0x0E:                      // LD C,n
		ExecLdRegImm8(&Cpu::SetC);
		break;

	case 0x16:                      // LD D,n
		ExecLdRegImm8(&Cpu::SetD);
		break;

	case 0x1E:                      // LD E,n
		ExecLdRegImm8(&Cpu::SetE);
		break;

	case 0x26:                      // LD H,n
		ExecLdRegImm8(&Cpu::SetH);
		break;

	case 0x2E:                      // LD L,n
		ExecLdRegImm8(&Cpu::SetL);
		break;

	case 0x04: case 0x0C: case 0x14: case 0x1C:	// INC r (including (HL))
	case 0x24: case 0x2C: case 0x34: case 0x3C:
		ExecIncReg(opcode);
		break;

	case 0x05: case 0x0D: case 0x15: case 0x1D:	// DEC r (including (HL))
	case 0x25: case 0x2D: case 0x35: case 0x3D:
		ExecDecReg(opcode);
		break;

	case 0x37:						// SCF
		ExecScf();
		break;

	case 0x80: // ADD A,B
	{
		uint8_t lhs = GetA();
		uint8_t rhs = GetB();

		uint16_t sum = static_cast<uint16_t>(lhs) + rhs;
		uint8_t result = static_cast<uint8_t>(sum);

		SetFlagsAdd8(lhs, rhs, 0, result);

		SetA(result);
	}
	break;

	case 0xC6:						// ADD A,n
		ExecAddAImm();
		break;

	case 0x88: case 0x89: case 0x8A: case 0x8B:			// ADC A,r
	case 0x8C: case 0x8D: case 0x8E: case 0x8F:
		ExecAdcAReg(opcode);
		break;
		
	case 0x90: case 0x91: case 0x92: case 0x93:			// SUB r
	case 0x94: case 0x95: case 0x96: case 0x97:
		ExecSubAReg(opcode);
		break;

	case 0x98: case 0x99: case 0x9A: case 0x9B:			// SBC A,r
	case 0x9C: case 0x9D: case 0x9E: case 0x9F:
		ExecSbcAReg(opcode);
		break;

	default:
		// TODO :: For now, do nothing (we’ll tighten this later)
		break;
	}
};