# Z80Emu

Being a professional developer and being fascinated with all things retro tech & Z80, I have flirted with the idea of writing a Z80 emulator for a long time. It has been 47 years since I was first exposed to my first Z80 computer and I know it’s already been done a thousand times over however, for me it’s about the challenge of learning and understanding Z80 architecture to a highly granular level and writing it in a language I haven’t used in ‘anger’ for almost 30 years, C++. Needless to say, things have changed a little and the days of Borland Turbo C++ are long gone. So anyway, I’ve started this project and I plan on documenting progress as I go; both the progress of the emulator and my ‘rediscovery’ of C++!

---

A from-scratch Z80 CPU emulator written in modern C++20.

This project is a structured learning exercise in emulator development, focusing first on correctness, determinism, and architectural clarity before performance or hardware accuracy.

The goal is not to rush to a full machine emulator, but to build a solid, extensible CPU core with proper timing (T-state based), clean separation of concerns, and strong debugging visibility.

---

## 🎯 Project Philosophy

This emulator is built around a few key principles:

- **T-states, not Hz** – All timing is cycle-based and deterministic.
- **No real-time inside the CPU core** – The CPU never sleeps. Scheduling and pacing are external.
- **Clean bus abstraction** – Memory and I/O are accessed via a bus interface.
- **Step-first development** – Instruction stepping and tracing come before full-speed execution.
- **Incremental implementation** – Small opcode sets implemented and verified before expanding.

---

## 🏗 Architecture Overview

### CPU Core
- 8-bit and 16-bit register model (AF, BC, DE, HL, SP, PC)
- Flag handling (S, Z, H, P/V, N, C)
- `step_instruction()` returns T-states consumed
- `run_for_tstates(budget)` executes until T-state budget exhausted
- HALT support (interrupt behaviour stubbed for now)
- Interrupt placeholders (IFF1/IFF2, IM, NMI line)

### Bus Layer
- 64KB RAM implementation
- Memory read/write callbacks
- I/O read/write stubs (expandable later)

### Execution Model
- Deterministic execution
- Single-instruction stepping
- Cycle-budget execution
- Breakpoint support
- Optional trace hook per instruction

---

## ✅ Currently Implemented Instructions

Minimal bootstrap set:

- `NOP`
- `HALT`
- `LD r,n`
- `LD (nn),A`
- `LD A,(nn)`
- `JP nn`
- `JR e`
- `INC r`
- `DEC r`
- `XOR A` (optional)

This is enough to execute simple loop programs and validate flags and timing behaviour.

---

## 🧪 Test Program

The emulator loads a small hardcoded test program at `0x8000` which:

- Initializes a memory location
- Increments it in a loop
- Demonstrates memory access, flag updates, and relative jumps

You can step through execution interactively via the built-in monitor.

---

## 🖥 Build Instructions (Windows / MSYS2 UCRT64)

Compiler:
```
C:\msys64\ucrt64\bin\g++.exe
```

Build:
```
g++ -std=c++20 -g -Iinclude src/main.cpp src/Bus.cpp src/Z80.cpp -o build/Z80Emu.exe
```

Or use the provided VS Code tasks configuration.

---

## 🗺 Roadmap

Planned milestones:

- [ ] Expand opcode coverage
- [ ] Official Z80 validation test suite
- [ ] Full interrupt model (IM 0/1/2)
- [ ] I/O port device system
- [ ] Scheduler layer (real-time pacing)
- [ ] Frame-based execution model
- [ ] Optional machine targets (ZX Spectrum? CP/M system?)

---

## 📓 Development Journal

This project is being documented as a learning journal.  
Each session records:

- Goal
- Changes made
- Lessons learned
- Tests run
- Next step

---

## 🚀 Why?

Because writing an emulator is one of the best ways to deeply understand:

- CPU architecture
- Binary execution
- Timing models
- Hardware abstraction
- Clean software architecture

And because the Z80 deserves it. This year sees its 50th Birthday and despite its age it's still a powerhouse of a CPU..... and I love it! 😍
