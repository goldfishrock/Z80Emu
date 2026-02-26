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

## 🧪 Test cleanup: fixture refactor

Every test used to start with:

```cpp
Bus bus;
Cpu cpu;
cpu.connect(&bus);
cpu.reset();

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

### Core / Flow
- NOP  
- PC fetch/increment discipline  
- 16-bit register pairs (BC, DE, HL, SP)

### Loads / Data movement
- LD r,r block (opcode bitfield decode + dynamic member-function dispatch)  
- `(HL)` memory handling within load/increment paths

### Arithmetic (so far)
- INC r (all 8-bit regs + `(HL)`)  
- DEC r (all 8-bit regs + `(HL)`)  
- SCF (Set Carry Flag)

### Flags
- Correct behaviour for INC/DEC:
- S, Z, H, P/V, N updated properly
- Carry preserved (unchanged) — verified by tests

## 🏆 Architectural Wins

- 🚫 No switch explosion  
- 🧮 Bitfield decoding  
- 🧠 Member-function dispatch  
- ♻️ Opcode-family handlers (INC/DEC now scalable)
- 🧪 Catch2 fixture cleanup (less boilerplate, clearer tests)

## 🛠️ Infrastructure

- Structured CMake build  
- Catch2 test harness  
- Tests split/organised by instruction groups (continuing trend)


---


# 🔮 Where This Is Heading

## 🔮 Next Logical Steps (the ubiquitous TODO)
- ➕ Arithmetic group (ADD, SUB, ADC, SBC)  
- ⏱️ Proper T-state timing  

### 🧮 More ALU / Flag-Critical Ops
- ➕ `AND`, `OR`, `XOR`, `CP`
- 🔁 `INC rr` / `DEC rr` (16-bit inc/dec)
- 🧷 `DAA` (annoying, but important for correctness)
- 🧊 `CPL`, `SCF`, `CCF` (flag twiddlers)

### 🧠 Shifts / Rotates / Bit Ops
- 🔄 `RLCA`, `RRCA`, `RLA`, `RRA`
- 🌀 CB-prefix group: `RLC/RRC/RL/RR`, `SLA/SRA/SRL`
- 🧩 `BIT`, `SET`, `RES` (CB-prefixed bit manipulation)

### 🧵 Memory Addressing & Data Movement
- 🧳 `LD (nn),A` / `LD A,(nn)`
- 🧱 `LD rr,nn` / `LD (nn),rr` / `LD rr,(nn)`
- 🔁 `EX`, `EXX` (register bank swaps)

### 📚 Stack + Subroutines
- 📌 `PUSH rr` / `POP rr`
- 📞 `CALL nn` / `RET`
- 🧷 `RST n` (quick call vectors)
- 🔀 Conditional forms: `CALL cc,nn`, `RET cc`

### 🧭 Branching / Control Flow
- 🦘 `JP nn` / `JR e`
- 🎯 Conditional jumps: `JP cc,nn`, `JR cc,e`
- 🔁 `DJNZ e` (super common; great for tests)

### 🧨 Prefixes / Extended Instruction Sets
- 🧩 `CB` prefix (bit ops) — big milestone
- 🧱 `ED` prefix (block ops, I/O, 16-bit extras)
- 🦶 `DD` / `FD` (IX/IY + displacement addressing) — later, but essential

### 🔌 I/O System
- 📥 `IN A,(n)` / `OUT (n),A`
- 🔄 `IN r,(C)` / `OUT (C),r` (ED-prefixed)
- 📦 Block I/O: `INI/IND/INIR/INDR`, `OUTI/OUTD/OTIR/OTDR` (ED-prefixed)

### 🧱 Interrupts + CPU Modes
- ⚡ Interrupt flip-flops: `DI`, `EI`
- 🧯 `IM 0/1/2`
- ⏳ `HALT` behaviour (timing + interrupt exit)
- 🧭 Refresh register `R` and interrupt vector `I`

### ⏱️ Timing + Accuracy Work
- ⏲️ Per-instruction T-state tables
- 🧬 Correct extra cycles for taken/not-taken branches
- 🧵 Memory contention / wait-states (optional depending on target machine)
- 🎛️ Accurate behaviour for prefixes and `(IX+d)/(IY+d)` timings

### 🧠 Memory System (ROM / RAM / Memory Map)
- 🧱 Implement explicit **ROM + RAM regions** in the `Bus`
  - ROM: readable, **writes ignored** 
  - RAM: readable + writable
- 🗺️ Define a simple **memory map**
  - Example: `0x0000–0x7FFF` ROM, `0x8000–0xFFFF` RAM 
- 📦 Add helpers for loading images:
  - Load **ROM binary** into ROM region at reset
  - Load small **test programs** into RAM for integration tests
- 🧪 Add tests for memory correctness:
  - ROM write attempts do not change ROM contents
  - RAM writes persist
  - Reads are correct across boundaries
- 🔁Bank switching / paging (for targetting Spectrum / CPM machines etc)



---

Creating the emulator is no longer an experiment or dream. Line by line it is becoming an engineered system.