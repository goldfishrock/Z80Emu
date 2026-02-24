# Z80Emu

Being a professional developer and being fascinated with all things retro tech & Z80, I have flirted with the idea of writing a Z80 emulator for a long time. It has been 47 years since I was first exposed to my first Z80 computer and I know it’s already been done a thousand times over however, for me it’s about the challenge of learning and understanding Z80 architecture to a highly granular level and writing it in a language I haven’t used in ‘anger’ for almost 30 years, C++. Needless to say, things have changed a little and the days of Borland Turbo C++ are long gone. So anyway, I’ve started this project and I plan on documenting progress as I go; both the progress of the emulator and my ‘rediscovery’ of C++!

---

A from-scratch Z80 CPU emulator written in modern C++20.

This project is a structured learning exercise in emulator development, focusing first on correctness, determinism, and architectural clarity before performance or hardware accuracy.

The goal is not to rush to a full machine emulator, but to build a solid, extensible CPU core with proper timing (T-state based), clean separation of concerns, and strong debugging visibility.

---

# What is Z80Emu

A slightly obsessive, thoroughly geeky, modern (C++20) Z80 emulator.

This isn’t a “let’s throw something together” emulator. I really wanted to try a proper project approach of a 
**learn it properly, build it cleanly, test it properly, understand every opcode** kind of emulator.

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

The goal is understanding. Every instruction. Every flag. Every tick.

---

## 🏗 Architecture Overview

The emulator is deliberately split into simple, clear layers.

### CPU Core

The CPU class handles:

- Registers (8-bit and 16-bit)
- Flags
- Instruction decode
- Execution logic
- PC/SP behaviour
- T-states (eventually fully accurate)

The aim is clarity over cleverness.  
If something looks “too magic”, it probably gets rewritten.

---

### Bus Layer

The bus abstracts memory and I/O access.

This keeps the CPU independent from:

- RAM layout
- ROM layout
- Future memory-mapped devices
- IO port implementations

It also makes testing easier — which is important because we’re doing this properly!!!

---

### Execution Model

The model is simple and explicit:

1. Fetch opcode
2. Decode
3. Execute
4. Advance PC
5. Update timing

No hidden tricks. No macro madness.  
Just readable, testable execution flow.

---

## ✅ Currently Implemented

The instruction set is being implemented incrementally and tested as we go.

Implemented so far (as of 24/02/26):

- NOP
- LD r,r
- Basic register operations
- PC handling
- SP handling
- Core fetch helpers

Each instruction is added with tests passing fully before moving on.

Green tests = sleep at night.

---

## 🧪 Test Program

The project includes unit tests (Catch2) covering:

- Register behaviour
- PC increment logic
- SP operations
- Instruction decoding
- Execution correctness

If something breaks, it should break loudly.

This emulator grows under test protection.

---

## 🖥 Build Instructions (Windows / MSYS2 UCRT64)

This project builds cleanly using:

- CMake
- MSYS2 (UCRT64 environment)
- A modern C++20 compiler

Typical build flow:

```bash
cmake -S . -B out/build
cmake --build out/build
ctest --test-dir out/build
```

If all goes well, you’ll see a nice wall of passing tests.  
If not… that’s why we have tests.

---

## 🗺 Roadmap

Planned work (in roughly sensible order):

- Complete 8-bit load group
- 16-bit load group
- Arithmetic & flag accuracy
- Stack operations
- Jumps / Calls / Returns
- Interrupt handling
- Timing refinement
- Possibly memory-mapped devices

Eventually:

- Enough correctness to run real Z80 programs
- Maybe CP/M
- Maybe something a bit more ambitious

One step at a time.

---

## 📓 Development Journal

This project is being developed incrementally, with small commits and proper branching (mostly).

Each session usually focuses on:

- One instruction group
- One architectural refinement
- Or one testing improvement

The idea is steady progress, not huge heroic rewrites.

---

## 🚀 Why?

Because writing an emulator is one of the best ways to truly understand:

- CPU architecture
- Binary execution
- Timing models
- Instruction decoding
- Clean software design

And because the Z80 deserves it.

It turned 50 this year and it’s still everywhere — embedded systems, retro machines, radio gear, industrial hardware.

It’s simple.  
It’s elegant.  
It’s brilliant.  

And I love it.

---

If you’re here because you’re building your own emulator — welcome.  
If you’re here because you love the Z80 — even better.
