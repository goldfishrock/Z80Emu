# Z80Emu

Being a professional developer and being fascinated with all things retro tech & Z80, I have flirted with the idea of writing a Z80 emulator for a long time. It has been 47 years since I was first exposed to my first Z80 computer and I know it’s already been done a thousand times over however, for me it’s about the challenge of learning and understanding Z80 architecture to a highly granular level and writing it in a language I haven’t used in ‘anger’ for almost 30 years, C++. Needless to say, things have changed a little and the days of Borland Turbo C++ are long gone. So anyway, I’ve started this project and I plan on documenting progress as I go; both the progress of the emulator and my ‘rediscovery’ of C++!

---

A from-scratch Z80 CPU emulator written in modern C++20.

This project is a structured learning exercise in emulator development, focusing first on correctness, determinism, and architectural clarity before performance or hardware accuracy.

The goal is not to rush to a full machine emulator, but to build a solid, extensible CPU core with proper timing (T-state based), clean separation of concerns, and strong debugging visibility.

---

# What is Z80Emu?

A slightly obsessive, thoroughly geeky, modern C++20 Z80 emulator.

This isn’t a “let’s throw something together” emulator.  
It’s a **learn it properly, build it cleanly, test it properly, understand every opcode** kind of emulator.

And yes… it’s being built because the Z80 is awesome.

---

## 🎯 What This Project Is (And Isn’t)

This project is:

- A ground-up Z80 emulator written in modern C++20  
- Built with CMake  
- Test-driven using Catch2  
- Structured to be clean, readable, and extendable  
- A learning vehicle as much as a functional emulator  

This project is not:

- A copy-paste from an existing emulator  
- A “good enough” hack  
- A speed-optimised black box  

The goal here is understanding. Every instruction. Every flag. Every tick.

---

## 🏗 Architecture Overview

The emulator is deliberately split into simple, clear layers.

### CPU Core

The CPU class handles:

- 8-bit and 16-bit register pairs (AF, BC, DE, HL)
- Flag manipulation via helper functions
- Instruction decoding via switch dispatch
- Opcode family grouping (e.g. `LD r,r`, `INC r`, `DEC r`)
- Clean helper functions for arithmetic (`inc8`, `dec8`, etc.)

The aim is clarity over cleverness.  
If something looks “too magic”, it probably gets rewritten.

---

### Bus Layer

The bus abstracts memory access.

The CPU connects to the bus and performs:

- Instruction fetch
- Memory reads
- Memory writes

This separation makes unit testing clean and predictable.

---

### Execution Model

Each step:

1. Fetch opcode from memory (PC)
2. Decode via switch statement
3. Execute instruction
4. Update PC
5. Update flags as required

No macro trickery. No hidden state mutation.

---

## ✅ Currently Implemented Instructions

### 📥 16-bit Load Instructions
- `LD BC,nn` (0x01)
- `LD DE,nn` (0x11)
- `LD HL,nn` (0x21)
- `LD SP,nn` (0x31)

---

### 📥 8-bit Load Instructions
- `LD r,n`
  - A, B, C, D, E, H, L
- `LD r,r`

---

### 🔁 8-bit Increment / Decrement
- `INC r` (including `(HL)`)
- `DEC r` (including `(HL)`)

---

### 🔁 16-bit Increment / Decrement
- `INC BC` (0x03)
- `INC DE` (0x13)
- `INC HL` (0x23)
- `INC SP` (0x33)

- `DEC BC` (0x0B)
- `DEC DE` (0x1B)
- `DEC HL` (0x2B)
- `DEC SP` (0x3B)

---

### 🧮 8-bit Arithmetic
- `ADD A,r` (0x80–0x87)
- `ADD A,n` (0xC6)
- `ADC A,r` (0x88–0x8F)
- `SUB r` (0x90–0x97)
- `SBC A,r` (0x98–0x9F)

---

### 🧠 8-bit Logical Operations
- `AND n` (0xE6)
- `OR n`  (0xF6)
- `XOR n` (0xEE)
- `CP n`  (0xFE)

---

### 🔢 16-bit Arithmetic
- `ADD HL,BC` (0x09)
- `ADD HL,DE` (0x19)
- `ADD HL,HL` (0x29)
- `ADD HL,SP` (0x39)

---

### 📦 Stack Operations
- `PUSH BC` (0xC5)
- `PUSH DE` (0xD5)
- `PUSH HL` (0xE5)

- `POP BC`  (0xC1)
- `POP DE`  (0xD1)
- `POP HL`  (0xE1)

---

### 🏳 Flag Control
- `SCF` (0x37)

---

### 🧪 Test Coverage

- 60 passing tests
- ALU flag behaviour verified
- 16-bit half-carry verified
- Stack byte order verified
- Stack pointer movement verified

---

The emulator now has:

- Core 8-bit ALU functionality
- 16-bit arithmetic
- 16-bit register increment/decrement
- Functional stack primitives
- Verified flag behaviour for implemented instructions

---

## 🖥 Build Instructions (Windows / MSYS2 UCRT64)

```bash
cmake -S . -B out/build
cmake --build out/build
ctest --test-dir out/build
```

If all goes well, you’ll see a nice wall of passing tests.  
If not… that’s why we have tests.

---

## 🗺 Roadmap

### Next Instruction Groups

- `CCF` (Complement Carry)
- 8-bit arithmetic group:
  - `ADD A,r`
  - `ADD A,(HL)`
  - `ADC A,r`
  - `SUB r`
  - `SBC A,r`
- 16-bit increment/decrement:
  - `INC rr`
  - `DEC rr`

### ALU / Flag-Critical Operations

- `AND`
- `OR`
- `XOR`
- `CP`
- `CPL`
- `DAA` (inevitable… and slightly terrifying)

### Structural / Accuracy Work

- Proper T-state timing model
- Interrupt handling
- Stack operations
- Jump / Call / Return instructions

### Longer Term

- Real program execution
- CP/M experiments
- Possibly memory-mapped peripherals
- Something mildly ridiculous

---

## 📓 Development Philosophy

This emulator grows incrementally.

Each session:

- One instruction group
- Full flag correctness
- Tests first
- Everything green before moving on

No rushed megacomits. No “we’ll fix flags later”.

---

## 🚀 Why?

Because writing an emulator is one of the best ways to truly understand:

- CPU architecture  
- Binary execution  
- Instruction decoding  
- Timing behaviour  
- Clean software structure  

And because the Z80 deserves it.

......It’s 50 years old.  
......It’s elegant.  
......It’s still everywhere.
......And it’s a joy to implement properly.

---

If you're building your own emulator — welcome.  
If you love the Z80 — even better.