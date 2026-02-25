#include "Cpu.h"
#include "Bus.h"

void Cpu::Connect(Bus* bus)
{
    bus_ = bus;
}

void Cpu::Reset(uint16_t pc)
{
    pc_ = pc;
}

void Cpu::Reset()
{
	pc_ = 0x0000;
    sp_ = 0xFFFF;
	halted_ = false;
}

bool Cpu::is_connected() const
{
    return bus_ != nullptr;
}

std::uint8_t Cpu::GetA() const
{
	return static_cast<std::uint8_t>(af_ >> 8);
}

std::uint8_t Cpu::GetB() const
{
	return static_cast<std::uint8_t>(bc_ >> 8);
}

std::uint8_t Cpu::GetC() const
{
	return static_cast<std::uint8_t>(bc_);
}

std::uint8_t Cpu::GetD() const
{
	return static_cast<std::uint8_t>(de_ >> 8);
}

std::uint8_t Cpu::GetE() const
{
	return static_cast<std::uint8_t>(de_);
}

std::uint8_t Cpu::GetF() const
{
	return static_cast<std::uint8_t>(af_ & 0x00FF);
}

std::uint8_t Cpu::GetH() const
{
	return static_cast<std::uint8_t>(hl_ >> 8);
}

std::uint8_t Cpu::GetL() const
{
	return static_cast<std::uint8_t>(hl_);
}

void Cpu::SetA(std::uint8_t value)
{
	af_ = (af_ & 0x00FFu) | (uint16_t(value) << 8);
}

void Cpu::SetB(std::uint8_t value)
{
	bc_ = (bc_ & 0x00FFu) | (uint16_t(value) << 8);
}

void Cpu::SetC(std::uint8_t value)
{
	bc_ = (bc_ & 0xFF00u) | (uint16_t(value));
}

void Cpu::SetD(std::uint8_t value)
{
	de_ = (de_ & 0xFF00u) | (uint16_t(value) << 8);
}

void Cpu::SetE(std::uint8_t value)
{
	de_ = (de_ & 0xFF00u) | (uint16_t(value));
}

void Cpu::SetF(std::uint8_t value)
{
	af_ = static_cast<std::uint16_t>((af_ & 0xFF00) | value);
}

void Cpu::SetH(std::uint8_t value)
{
	hl_ = (hl_ & 0x00FFu) | (uint16_t(value) << 8);
}

void Cpu::SetL(std::uint8_t value)
{
	hl_ = static_cast<std::uint16_t>((hl_ & 0xFF00) | value);
}

void Cpu::SetFlag(uint8_t mask, bool on)
{
	std::uint8_t flags = GetF();
	if (on) flags |= mask;
	else    flags = static_cast<std::uint8_t>(flags & static_cast<std::uint8_t>(~mask));
	SetF(flags);
}

void Cpu::ExecAddAReg(uint8_t opcode)
{
	const uint8_t src = opcode & 0x07;

	// Skip (HL) for now, same rule as your LD r,r
	if (src == 6)
		return;

	const uint8_t aVal = GetA();
	const uint8_t value = (this->*reg8Get[src])();

	const uint16_t sum = static_cast<uint16_t>(aVal) + value;
	const uint8_t result = static_cast<uint8_t>(sum);

	SetFlagsAdd8(aVal, value, 0, result);
	SetA(result);
}

void Cpu::ExecAdcAReg(uint8_t opcode)
{
	const uint8_t src = opcode & 0x07;

	// Skip (HL) for now
	if (src == 6)
		return;

	const uint8_t lhs = GetA();
	const uint8_t rhs = (this->*reg8Get[src])();

	const uint8_t carry_in = (GetF() & FLAG_C) ? 1 : 0;

	const uint16_t sum = static_cast<uint16_t>(lhs) + rhs + carry_in;
	const uint8_t result = static_cast<uint8_t>(sum);

	SetFlagsAdd8(lhs, rhs, carry_in, result);
	SetA(result);
}

std::uint8_t Cpu::PopByte()
{
    const auto value = bus_->Read(sp_);
    ++sp_;
    return value;
}

void Cpu::PushByte(std::uint8_t value)
{
    --sp_;                      // stack grows downward
    bus_->Write(sp_, value);
}

std::uint8_t Cpu::FetchByte()
{
    //TODO :: (Decide how to handle “not connected” in a later step.)
    const auto value = bus_->Read(pc_);
    pc_++;
    return value;
}

std::uint16_t Cpu::FetchWord()
{
    const std::uint16_t lo = FetchByte();
    const std::uint16_t hi = FetchByte();
    return static_cast<std::uint16_t>((hi << 8) | lo);
}

void Cpu::SetFlagsAdd8(uint8_t lhs, uint8_t rhs, uint8_t carryIn, uint8_t result)
{
	const uint16_t sum = static_cast<uint16_t>(lhs) + rhs + carryIn;

	uint8_t f = 0;
	if (result & 0x80) f |= FLAG_S;
	if (result == 0)   f |= FLAG_Z;
	if (((lhs & 0x0F) + (rhs & 0x0F) + carryIn) > 0x0F) f |= FLAG_H;
	if ((~(lhs ^ rhs) & (lhs ^ result) & 0x80) != 0)      f |= FLAG_PV;
	// N=0
	if (sum > 0xFF) f |= FLAG_C;

	// Save the value to F register
	SetF(f);
}

void Cpu::SetFlagsSub8(uint8_t lhs, uint8_t rhs, uint8_t carryIn, uint8_t result)
{
	const uint16_t subtrahend = static_cast<uint16_t>(rhs) + carryIn;

	uint8_t f = 0;
	if (result & 0x80) f |= FLAG_S;
	if (result == 0)   f |= FLAG_Z;
	if ((lhs & 0x0F) < ((rhs & 0x0F) + carryIn)) f |= FLAG_H;
	if (((lhs ^ rhs) & (lhs ^ result) & 0x80) != 0) f |= FLAG_PV;
	f |= FLAG_N;

	if (static_cast<uint16_t>(lhs) < subtrahend) f |= FLAG_C;

	// Save the value to F register
	SetF(f);
}

void Cpu::ExecLdRegImm16(void (Cpu::* setter)(uint16_t))
{
    (this->*setter)(FetchWord());
}

void Cpu::ExecLdRegImm8(void (Cpu::* setter)(uint8_t))
{
    (this->*setter)(FetchByte());
}

uint8_t Cpu::Inc8(uint8_t v)
{
	const uint8_t r = static_cast<uint8_t>(v + 1);

	SetFlag(FLAG_S, (r & 0x80) != 0);
	SetFlag(FLAG_Z, r == 0);
	SetFlag(FLAG_H, (v & 0x0F) == 0x0F);
	SetFlag(FLAG_PV, v == 0x7F);     // +127 -> -128 overflow
	SetFlag(FLAG_N, false);         // INC resets N
	// C unchanged

	return r;
}

uint8_t Cpu::Dec8(uint8_t v)
{
	const uint8_t r = static_cast<uint8_t>(v - 1);

	SetFlag(FLAG_S, (r & 0x80) != 0);
	SetFlag(FLAG_Z, r == 0);
	SetFlag(FLAG_H, (v & 0x0F) == 0x00);
	SetFlag(FLAG_PV, v == 0x80);     // -128 -> +127 overflow
	SetFlag(FLAG_N, true);          // DEC sets N
	// C unchanged

	return r;
}

