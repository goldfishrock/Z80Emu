#pragma once
#include <cstdint>
#include <array>

class Bus;

class Cpu
{
	public:
	    void Reset();
	    void Reset(uint16_t pc);
	    void Connect(Bus* bus);
	    void PushByte(std::uint8_t value);
	    void ExecScf();
	    void ExecIncReg(uint8_t opcode);
	    void ExecDecReg(uint8_t opcode);
	    void ExecLdRegReg(uint8_t opcode);
	    void SetFlag(uint8_t mask, bool on);
		void ExecAddAReg(uint8_t opcode);
		void ExecAdcAReg(uint8_t opcode);
		void Step();
	    bool is_connected() const;
	    bool is_halted() const { return halted_; }
		std::uint16_t FetchWord();

	    // Flag masks (standard Z80)
	    static constexpr uint8_t FLAG_S = 0x80;
	    static constexpr uint8_t FLAG_Z = 0x40;
	    static constexpr uint8_t FLAG_H = 0x10;
	    static constexpr uint8_t FLAG_PV = 0x04;
	    static constexpr uint8_t FLAG_N = 0x02;
	    static constexpr uint8_t FLAG_C = 0x01;

		// Public getters for the 8-bit registers
		std::uint8_t PopByte();
		std::uint8_t FetchByte();
		std::uint8_t GetA() const;
		std::uint8_t GetB() const;
		std::uint8_t GetC() const;
		std::uint8_t GetD() const;
		std::uint8_t GetE() const;
		std::uint8_t GetF() const;
		std::uint8_t GetH() const;
		std::uint8_t GetL() const;

		// Public setters for the 8-bit registers
		void SetA(std::uint8_t value);
		void SetB(std::uint8_t value);
		void SetC(std::uint8_t value);
		void SetD(std::uint8_t value);
		void SetE(std::uint8_t value);
		void SetF(std::uint8_t value);
		void SetG(std::uint8_t value);
		void SetH(std::uint8_t value);
	    void SetL(std::uint8_t value);

	    // Public getters for the 16-bit registers
		std::uint16_t GetAf() const { return af_; }
		std::uint16_t GetBc() const { return bc_; }
		std::uint16_t GetDe() const { return de_; }
		std::uint16_t GetHl() const { return hl_; }
		std::uint16_t GetPc() const { return pc_; }
		std::uint16_t GetSp() const { return sp_; }

		// Public setters for the 16-bit registers
		void SetAf(std::uint16_t value) { af_ = value; }
		void SetBc(std::uint16_t value) { bc_ = value; }
		void SetDe(std::uint16_t value) { de_ = value; }
		void SetHl(std::uint16_t value) { hl_ = value; }
		void SetSp(std::uint16_t value) { sp_ = value; }

	private:
	    using Reg8Getter = uint8_t(Cpu::*)() const;
	    using Reg8Setter = void (Cpu::*)(uint8_t);

	    Bus* bus_ = nullptr;
	    std::uint16_t pc_ = 0;
	    std::uint16_t sp_ = 0;
	    std::uint16_t af_ = 0;
	    std::uint16_t bc_ = 0;
	    std::uint16_t de_ = 0;
	    std::uint16_t hl_ = 0;

	    bool halted_ = false;

	    void ExecLdRegImm8(void (Cpu::* setter)(uint8_t));
	    void ExecLdRegImm16(void (Cpu::* setter)(uint16_t));
		void ExecAddAImm();
		void ExecSubAReg(uint8_t opcode);
		void ExecSbcAReg(uint8_t opcode);
		void SetFlagsAdd8(uint8_t a, uint8_t b_val, uint8_t carryIn, uint8_t result);
		void SetFlagsSub8(uint8_t a, uint8_t b, uint8_t carryIn, uint8_t result);

	    uint8_t Inc8(uint8_t v);
	    uint8_t Dec8(uint8_t v);

	    static constexpr std::array<Reg8Getter, 8> reg8Get = {
	    &Cpu::GetB, &Cpu::GetC, &Cpu::GetD, &Cpu::GetE, &Cpu::GetH, &Cpu::GetL, nullptr, &Cpu::GetA
	    };

		static constexpr std::array<Reg8Setter, 8> reg8Set = {
			&Cpu::SetB, &Cpu::SetC, &Cpu::SetD, &Cpu::SetE, &Cpu::SetH, &Cpu::SetL, nullptr, &Cpu::SetA
		};
};
    