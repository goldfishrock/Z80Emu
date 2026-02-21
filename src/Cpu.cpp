#include "Cpu.h"

void Cpu::connect(Bus* bus) { bus_ = bus; }
void Cpu::reset() {}

bool Cpu::isConnected() const {
    return bus_ != nullptr;
}