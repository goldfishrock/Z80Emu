#include "Bus.h"
#include "Cpu.h"

void Cpu::ExecScf()
{
	// C=1, N=0, H=0. Everything else unchanged.
	SetFlag(Cpu::FLAG_C, true);
	SetFlag(Cpu::FLAG_N, false);
	SetFlag(Cpu::FLAG_H, false);
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

	const uint8_t carry_in = (GetF() & Cpu::FLAG_C) ? 1 : 0;

	const uint16_t diff = static_cast<uint16_t>(lhs) - rhs - carry_in;
	const uint8_t result = static_cast<uint8_t>(diff);

	SetFlagsSub8(lhs, rhs, carry_in, result);
	SetA(result);
}

bool Cpu::Parity(uint8_t value)
{
	bool parity = true; // even parity
	while (value)
	{
		parity = !parity;
		value &= (value - 1);
	}
	return parity;
}

void Cpu::execAddHl(uint16_t value)
{
	const uint32_t hl = GetHl();
	const uint32_t rhs = value;
	const uint32_t sum = hl + rhs;

	// H: carry from bit 11
	SetFlag(Cpu::FLAG_H, ((hl & 0x0FFFu) + (rhs & 0x0FFFu)) > 0x0FFFu);

	// C: carry from bit 15
	SetFlag(Cpu::FLAG_C, sum > 0xFFFFu);

	// N cleared
	SetFlag(Cpu::FLAG_N, false);

	SetHl(static_cast<uint16_t>(sum));
}

void Cpu::ExecPush(uint16_t value)
{
	const uint8_t hi = static_cast<uint8_t>(value >> 8);
	const uint8_t lo = static_cast<uint8_t>(value & 0xFF);

	// Z80 push order:
	// SP-- ; (SP) = high
	// SP-- ; (SP) = low
	sp_ = static_cast<uint16_t>(sp_ - 1);
	bus_->Write(sp_, hi);

	sp_ = static_cast<uint16_t>(sp_ - 1);
	bus_->Write(sp_, lo);
}

uint16_t Cpu::ExecPop()
{
	// Z80 pop order:
	// low = (SP) ; SP++
	// high = (SP) ; SP++
	const uint8_t lo = bus_->Read(sp_);
	sp_ = static_cast<uint16_t>(sp_ + 1);

	const uint8_t hi = bus_->Read(sp_);
	sp_ = static_cast<uint16_t>(sp_ + 1);

	return static_cast<uint16_t>((hi << 8) | lo);
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
		// When we have all the OP codes implemented I shall order them properly.
		// It's really upsetting my OCD at the moment ;-)
		case 0x01: ExecLdRegImm16(&Cpu::SetBc);	break;						// LD BC,nn
		case 0x11: ExecLdRegImm16(&Cpu::SetDe); break;						// LD DE,nn
		case 0x21: ExecLdRegImm16(&Cpu::SetHl); break;						// LD HL,nn
		case 0x31: ExecLdRegImm16(&Cpu::SetSp); break;						// LD SP,nn
		case 0x03: SetBc(static_cast<std::uint16_t>(GetBc() + 1)); break;	// INC BC
		case 0x13: SetDe(static_cast<std::uint16_t>(GetDe() + 1)); break;	// INC DE
		case 0x23: SetHl(static_cast<std::uint16_t>(GetHl() + 1)); break;	// INC HL
		case 0x33: sp_ = static_cast<std::uint16_t>(sp_ + 1); break;		// INC SP
		case 0x0B: SetBc(static_cast<std::uint16_t>(GetBc() - 1)); break;	// DEC BC
		case 0x1B: SetDe(static_cast<std::uint16_t>(GetDe() - 1)); break;	// DEC DE
		case 0x2B: SetHl(static_cast<std::uint16_t>(GetHl() - 1)); break;	// DEC HL
		case 0x3B: sp_ = static_cast<std::uint16_t>(sp_ - 1); break;		// DEC SP
		case 0x3E: ExecLdRegImm8(&Cpu::SetA); break;						// LD A,n
		case 0x06: ExecLdRegImm8(&Cpu::SetB); break;						// LD B,n
		case 0x0E: ExecLdRegImm8(&Cpu::SetC); break;						// LD C,n
		case 0x16: ExecLdRegImm8(&Cpu::SetD); break;						// LD D,n
		case 0x1E: ExecLdRegImm8(&Cpu::SetE); break;						// LD E,n
		case 0x26: ExecLdRegImm8(&Cpu::SetH); break;						// LD H,n
		case 0x2E: ExecLdRegImm8(&Cpu::SetL); break;						// LD L,n
		case 0x04: case 0x0C: case 0x14: case 0x1C:							// INC r (including (HL))
		case 0x24: case 0x2C: case 0x34: case 0x3C: ExecIncReg(opcode);	break;
		case 0x05: case 0x0D: case 0x15: case 0x1D:							// DEC r (including (HL))
		case 0x25: case 0x2D: case 0x35: case 0x3D: ExecDecReg(opcode);	break;
		case 0x37: ExecScf(); break;										// SCF
		case 0x88: case 0x89: case 0x8A: case 0x8B:							// ADC A,r
		case 0x8C: case 0x8D: case 0x8E: case 0x8F: ExecAdcAReg(opcode); break;
		case 0x90: case 0x91: case 0x92: case 0x93:							// SUB r
		case 0x94: case 0x95: case 0x96: case 0x97: ExecSubAReg(opcode); break;
		case 0x98: case 0x99: case 0x9A: case 0x9B:							// SBC A,r
		case 0x9C: case 0x9D: case 0x9E: case 0x9F: ExecSbcAReg(opcode); break;
		case 0xC6: ExecAddAImm(); break;									// ADD A,n
		case 0x09: execAddHl(GetBc()); break;
		case 0x19: execAddHl(GetDe()); break;
		case 0x29: execAddHl(GetHl()); break;
		case 0x39: execAddHl(GetSp()); break;
		case 0x80: case 0x81: case 0x82: case 0x83:							// ADD A,r
		case 0x84: case 0x85: case 0x86: case 0x87: ExecAddAReg(opcode); break;
		case 0xC5: ExecPush(GetBc()); break;								// PUSH BC
		case 0xD5: ExecPush(GetDe()); break; 								// PUSH DE
		case 0xE5: ExecPush(GetHl()); break;								// PUSH HL
		case 0xF5: ExecPush(GetAf()); break;								// PUSH AF
		case 0xC1: SetBc(ExecPop()); break;							// POP BC
		case 0xD1: SetDe(ExecPop()); break;							// POP DE
		case 0xE1: SetHl(ExecPop()); break;							// POP HL
		case 0xF1: SetAf(ExecPop()); break;							// POP AF


		case 0xE6:											// AND n
		{
			uint8_t value = FetchByte();   // however you fetch immediate
			uint8_t result = GetA() & value;

			SetA(result);

			// Flags
			SetFlag(Cpu::FLAG_S,(result & 0x80) != 0);
			SetFlag(Cpu::FLAG_Z, (result == 0));
			SetFlag(Cpu::FLAG_H ,true);
			SetFlag(Cpu::FLAG_PV, Parity(result)); 
			SetFlag(Cpu::FLAG_N, false);
			SetFlag(Cpu::FLAG_C , false);

			break;
		}

		case 0xF6: // OR n
		{
			const uint8_t value = FetchByte();
			const uint8_t result = GetA() | value;

			SetA(result);

			SetFlag(Cpu::FLAG_S, (result & 0x80) != 0);
			SetFlag(Cpu::FLAG_Z, (result == 0));
			SetFlag(Cpu::FLAG_H, false);
			SetFlag(Cpu::FLAG_PV, Parity(result));
			SetFlag(Cpu::FLAG_N, false);
			SetFlag(Cpu::FLAG_C, false);

			break;
		}

		case 0xEE:											// XOR n
		{
			const uint8_t value = FetchByte();
			const uint8_t result = GetA() ^ value;

			SetA(result);

			SetFlag(Cpu::FLAG_S, (result & 0x80) != 0);
			SetFlag(Cpu::FLAG_Z, (result == 0));
			SetFlag(Cpu::FLAG_H, false);
			SetFlag(Cpu::FLAG_PV, Parity(result));
			SetFlag(Cpu::FLAG_N, false);
			SetFlag(Cpu::FLAG_C, false);

			break;
		}

		case 0xFE: // CP n
		{
			const uint8_t value = FetchByte();
			const uint8_t a = GetA();
			const uint8_t result = static_cast<uint8_t>(a - value);

			//SetFlag(Cpu::FLAG_S, (result & 0x80) != 0);
			SetFlag(Cpu::FLAG_Z, (result == 0));
			//SetFlag(Cpu::FLAG_H, (a & 0x0F) < (value & 0x0F)); // half-borrow
			//SetFlag(Cpu::FLAG_PV, ((a ^ value) & (a ^ result) & 0x80) != 0); // overflow
			SetFlag(Cpu::FLAG_N, true);
			SetFlag(Cpu::FLAG_C, a < value);

			break;
		}

		

		default:
			// TODO :: For now, do nothing (we’ll tighten this later)
			break;
		}
};