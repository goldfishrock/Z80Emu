# 🧠 Z80Emu – Development Journal

For a long time now I have wanted to try writing an emulator. I have considered all sorts over the years however, I keep coming back to the Z80. As a child of the (very) late 60s, I was 10 years old when I got my first computer, a Sinclair ZX81, sporting a massive 1Kb of RAM and the worst keyboard known to man. Despite that, I squirrelled myself away in my bedroom and taught myself BASIC and some Assembly. Although it was limited (even with the 16K RAM Pack I installed), it was still thrilling to write a program of my own and run it. I loved it and I fell in love with the Z80 wonder right there and then. After a spell in the forces, I finally went to Uni, did Computer Science and became a professional developer, all because of my love for those early Z80 machines I’d learned to code. To this day, almost 50 years later I still have a passion for this retro tech and so it only seems fitting I see if I can use my development skills and Z80 understanding to develop an Emulator. I am not sure quite where this will go at this point in time but it would good to see just how far I can push it.

I wanted to develop it in C++ rather than the C# that I use day to day. I think being a retro emulator it deserves to be written in the original retro language. I have always loved C++ but never been able to really use it outside of a bit of tinkering. It’s evolved massively over the last 30 years since I first learned it so it’s been great to get updated and be able to tackle this in ‘modern’ C++ (C++20).

I also wanted to use GITHUB to manage the codebase and use Incremental TDD (Test-Driven Development) as this is something I rarely get to do in my professional life.

My bible for this project is the original Zilog Z80 CPU user Manual

(https://www.zilog.com/docs/z80/um0080.pdf)


---

## 🎯 Philosophy

This project is not about “getting something working quickly.”

It is about:

- 🧱 Clean architecture  
- 🧮 Opcode-driven decoding  
- ♻️ Clean Code :: Zero duplication where possible  
- 🧪 Test-First incremental progress  (Really important)
- 📚 Refresh learning of modern C++ to a deep level while building something real  


---

# 🏗️ Phase 1 – Clean Foundations

## 🧩 Architecture

I separated the system into:

- 🚌 `Bus` – owns memory and read/write logic  
- 🧠 `Cpu` – owns registers and instruction execution  
- 📦 `z80core` – reusable core library  

```cpp
class Cpu
{
public:
    void connect(Bus* bus);
    void reset();
    void step();

private:
    Bus* m_bus = nullptr;
};
```

Dependency injection:

```cpp
cpu.connect(&bus);
```

This makes the CPU completely testable.


---

# 🟢 Phase 2 – First Instruction (NOP)

The first milestone: opcode `0x00`. The simplest of them all. NOP. NO OPERATION. Simply put it makes the CPU sit there and burn through it's T-Cycles while doing nothing. The darling of the reverse engineering movement as it's often used to remove software protection routines! Very simple to implement, pick up the OPCODE and do nothing what so ever apart from the CPU T-Cycles (to be impelented. For now it just returns).

```cpp
void Cpu::step()
{
    uint8_t opcode = fetchByte();

    switch (opcode)
    {
        case 0x00: // NOP
            break;
    }
}
```

Test:

```cpp
bus.write(0x0000, 0x00);
cpu.reset();
cpu.step();

REQUIRE(cpu.getPC() == 0x0001);
```

First green test = first psychological win.


---

# 🧮 Phase 3 – LD r,r Block (0x40–0x7F)

First real challenge. There are 49 separate cases of LD r,r. I really didn't want to write 49 CASE statements or anything similar. Far too inefficient and beside looking terrible would cause a nightmare reading the code. So, instead of writing 49 separate cases:

```cpp
case 0x40: B = B; break;
case 0x41: B = C; break;
case 0x42: B = D; break;
```

I came up with an algorithm that decoded the opcode bits.

Z80 encoding format:

```
01 ddd sss
```

Where:

- `ddd` = destination register  
- `sss` = source register  

Extraction:

```cpp
uint8_t dst = (opcode >> 3) & 0x07;
uint8_t src = opcode & 0x07;
```


---

## 🚀 The Big Epiphany – Dynamic Member Function Dispatch

Instead of a giant switch…

I created two lookup tables of member function pointers.

```cpp
using Reg8Getter = uint8_t (Cpu::*)() const;
using Reg8Setter = void (Cpu::*)(uint8_t);

std::array<Reg8Getter, 8> reg8Get =
{
    &Cpu::getB, &Cpu::getC, &Cpu::getD, &Cpu::getE,
    &Cpu::getH, &Cpu::getL, nullptr,    &Cpu::getA
};

std::array<Reg8Setter, 8> reg8Set =
{
    &Cpu::setB, &Cpu::setC, &Cpu::setD, &Cpu::setE,
    &Cpu::setH, &Cpu::setL, nullptr,    &Cpu::setA
};
```

Then the breakthrough line:

```cpp
const std::uint8_t v = (this->*reg8Get[r])();
(this->*reg8Set[r])(inc8(v));
```

What’s happening here?

- 🧭 `reg8Get[r]` selects a function pointer at runtime  
- 🔁 `(this->*ptr)()` invokes a member function dynamically  
- ❌ No switch  
- ❌ No duplication  
- ✅ Pure opcode-driven behaviour  

This was the moment the emulator stopped being procedural code…

…and became decoded hardware logic expressed cleanly in C++. 

More accurately:

> Using opcode bitfields to select CPU register accessors at runtime via member function pointer tables. In effect being able to generate the program flow dynamically by choosing the correct function to call based on a lookup variable. Powerful stuff..... That’s a serious architectural upgrade.


---

# 🔁 Phase 4 – INC / DEC Refactor

**Originally:**

```cpp
case 0x04: B++; break;
case 0x05: B--; break;
```

Refactored using grouped handling.

**Opcode structure:**

```
00 rrr 100  (INC r)
00 rrr 101  (DEC r)
```

**Implementation:**

First I wanted to extract three specific bits from the opcode. I shifted the whole opcode (in binary) to the right three places and then 'masked' it to make sure that I only extracted the 'bottom' three bits (taking a 4 byte binary number and ANDing it with 0x07 (00000111) extracts the last/bottom three bits).
```cpp
uint8_t r = (opcode >> 3) & 0x07;
```
This way I was able to extract the source and destination 'registers'. I used these extracted values to call a lookup table which then gave me the true register.

```cpp
const std::uint8_t v = (this->*reg8Get[r])();
(this->*reg8Set[r])(inc8(v));
```

**Result:**

- ✨ Cleaner code  
- ♻️ No opcode duplication  
- 📈 Scales naturally across registers  


---

# 🧭 Phase 5 – Program Counter Discipline

Implemented proper fetch logic:

```cpp
uint8_t Cpu::fetchByte()
{
    return m_bus->read(m_pc++);
}
```

This ensures:

- ⏱️ Correct PC increment timing  
- 🔄 Accurate instruction sequencing  
- 🧩 Easier future T-state modelling 


---

# 🧮 Phase 6 – 8-bit Arithmetic Begins (INC/DEC done properly)

Today was the first proper “flags matter” phase.

I already had completed some early INC/DEC work, but it was partial and a bit special-cased.
This phase finished the 8-bit INC/DEC family properly and made it scalable.

## ✅ What landed

### 🧱 Full INC/DEC family (register + memory)

Implemented the full opcode groups:

- `INC r`  → `04 0C 14 1C 24 2C 3C` (+ now `(HL)` too)
- `DEC r`  → `05 0D 15 1D 25 2D 3D` (+ now `(HL)` too)

And importantly:

- `INC (HL)` → `0x34`
- `DEC (HL)` → `0x35`

So it now supports INC/DEC across:

- A, B, C, D, E, H, L
- and memory via `(HL)`

### ♻️ Refactor: opcode-family dispatch (no one-off cases)

Instead of keeping `INC B` / `DEC B` as individual switch cases,
I routed the whole family into shared handlers:

- `execIncReg(opcode)`
- `execDecReg(opcode)`

This matches the same “decode-and-dispatch” mindset as the `LD r,r` block.

### 🚩 Flags: correct behaviour (and tests that prove it)

`INC/DEC` now correctly updates:

- **S** (sign)
- **Z** (zero)
- **H** (half carry / half borrow)
- **P/V** (overflow)
- **N** (INC clears, DEC sets)

And critically:

- **C is unchanged** by INC/DEC (classic Z80 behaviour)

### 🏳️ SCF implemented (because we needed it for real flag testing)

Added:

- `SCF` (0x37) — Set Carry Flag

This lets me lock down the “carry unchanged” behaviour via tests.

---

# 🧮 Phase 7 – Logical & Compare Instructions (Flags Get Serious)

Today was a proper ALU day.

Up until now the arithmetic work had focused on `INC` / `DEC` and the early arithmetic group.  
This session expanded the ALU into logical operations and comparison — and with that came proper parity handling and full subtraction-flag behaviour.

This was less about “new opcodes” and more about tightening architectural correctness.

---

## ✅ What Landed

### 🔀 Immediate Logical Instructions

Implemented:

- `AND n` (0xE6)
- `OR n`  (0xF6)
- `XOR n` (0xEE)
- `CP n`  (0xFE)

These are small instructions, but flag-heavy.

Each instruction now:

- Fetches immediate byte
- Performs operation
- Updates flags correctly
- Leaves CPU state deterministic and testable

---

## 🚩 Flag Architecture Cleanup

This phase exposed a small but important design issue.

Originally, `GetFlag()` returned the raw masked value from the `F` register.

Which meant:

- `FLAG_S` = `0x80`
- Test expected `1`
- Result was `128`

Correct behaviour… wrong abstraction for tests.

---

## 🧱 Phase 8 — Stack Foundations (PUSH / POP)

Today marked a structural milestone in the emulator:  
**Stack operations are now implemented and fully tested.**

### ✅ Implemented

#### 📦 PUSH rr
- `0xC5` → PUSH BC  
- `0xD5` → PUSH DE  
- `0xE5` → PUSH HL  

#### 📦 POP rr
- `0xC1` → POP BC  
- `0xD1` → POP DE  
- `0xE1` → POP HL  

### 🧠 Behaviour Implemented

- Stack grows **downwards** in memory.
- PUSH:
  - `SP--`
  - store high byte
  - `SP--`
  - store low byte
- POP:
  - read low byte
  - `SP++`
  - read high byte
  - `SP++`
- No flags are affected.
- Exact Z80 byte ordering verified.

### 🧪 Test Coverage

- Verified correct SP movement (±2)
- Verified memory write order
- Verified register restoration
- 100% green test suite

**Test Count: 60 passing, 0 failing**

---

### 🏗 Architectural Impact

This unlocks:

- `CALL nn`
- `RET`
- `RST`
- Interrupt handling later

The emulator now has:
- Working 8-bit ALU
- Working 16-bit arithmetic
- Working 16-bit register increment/decrement
- Working stack primitives

This is the first step toward real program flow.


---


# 🧪 Testing Strategy

Every instruction is tested in isolation.

Fixture pattern:

```cpp
struct CpuFixture
{
    Bus bus;
    Cpu cpu;

    CpuFixture()
    {
        cpu.connect(&bus);
        cpu.reset();
    }
};
```

Instruction injection:

```cpp
bus.write(0x0000, 0x04); // INC B
cpu.step();
REQUIRE(cpu.getB() == 1);
```

Deterministic, Repeatable & Safe.


---

# 📊 Current Status

## ✅ Implemented

### 🧠 Core / Flow
- NOP  
- PC fetch/increment discipline  
- 16-bit register pairs (BC, DE, HL, SP)

---

### 📦 Loads / Data Movement
- LD r,r block (opcode bitfield decode + dynamic member-function dispatch)  
- `(HL)` memory handling within load/increment paths  

---

### 🧮 8-bit ALU – Arithmetic & Logical

#### 🔁 Increment / Decrement
- INC r (all 8-bit registers + `(HL)`)  
- DEC r (all 8-bit registers + `(HL)`)

Fully correct flag behaviour:
- S
- Z
- H
- P/V
- N
- Carry preserved (unchanged) — verified via SCF-based tests

---

#### ➕ Logical Immediate Instructions
- AND n (0xE6)
- OR n  (0xF6)
- XOR n (0xEE)
- CP n  (0xFE)

Correct flag handling implemented and tested:
- S
- Z
- H (where appropriate)
- P/V (true parity implementation)
- N
- C (correct subtraction semantics for CP)

Parity logic centralised and reusable.

---

### 🧷 Flag Instructions
- SCF (Set Carry Flag)

---

### 📦 Stack Operations (Phase 8)

#### PUSH rr
- PUSH BC (0xC5)
- PUSH DE (0xD5)
- PUSH HL (0xE5)

#### POP rr
- POP BC (0xC1)
- POP DE (0xD1)
- POP HL (0xE1)

Correct Z80 behaviour:
- Stack grows downward
- Correct byte ordering (high byte first on PUSH)
- SP adjusted correctly (±2)
- No flags modified

---

## 🏆 Architectural Wins

- 🚫 No switch explosion  
- 🧮 Bitfield decoding  
- 🧠 Member-function dispatch  
- ♻️ Opcode-family handlers (INC/DEC scalable)  
- 🧪 Clean Catch2 fixture pattern  
- 🧱 Deterministic ALU flag logic  
- 📦 Stack abstraction now unlocks real program flow  

---

## 🧪 Test Status

- 60 passing tests  
- 0 failures  
- Instruction-level isolation  
- Flag behaviour verified explicitly  
- Stack correctness verified (SP movement + memory order) 

---


# 🔮 Where This Is Heading

## ➕ Next Logical Steps

### 🧮 Full Arithmetic Group (8-bit)
- ADD A,r  
- ADD A,n  
- SUB r  
- SUB n  
- ADC  
- SBC  

These will complete the core 8-bit arithmetic engine.

---

### 🧮 16-bit Arithmetic Completion
- ADD HL,rr  
- INC rr  
- DEC rr  

---

### 🧷 Remaining Flag Instructions
- DAA (BCD adjust — subtle and important)
- CPL
- CCF

---

### 🧠 Shifts / Rotates / Bit Ops
- RLCA, RRCA, RLA, RRA  
- CB-prefix group:
  - RLC / RRC / RL / RR  
  - SLA / SRA / SRL  
  - BIT / SET / RES  

Major milestone once CB group lands.

---

### 🧵 Subroutines & Control Flow
- CALL nn  
- RET  
- RST n  
- Conditional CALL/RET  

---

### 🧭 Branching
- JP nn  
- JR e  
- Conditional JP / JR  
- DJNZ  

---

### 🧨 Prefix Instruction Sets
- CB prefix (bit ops)
- ED prefix (block ops, extended arithmetic, I/O)
- DD / FD prefixes (IX / IY + displacement)

---

### 🔌 I/O System
- IN / OUT instructions  
- Block I/O (ED-prefixed forms)

---

### ⚡ Interrupts & CPU Modes
- DI / EI  
- IM 0 / 1 / 2  
- HALT behaviour  
- I and R registers  

---

### ⏱️ Timing & Accuracy
- Per-instruction T-state modelling  
- Taken vs not-taken cycle differences  
- Prefix timing corrections  
- Optional memory contention modelling  

---

### 🧱 Memory System (ROM / RAM Mapping)
- Explicit ROM region (write-protected)
- Explicit RAM region
- Defined memory map
- ROM loading helpers
- Integration-level test programs
- Future bank switching support

---

Line by line... it's becomming a full Z80 core.