#pragma once
class Bus;

class Cpu {
public:
    void reset();
    void connect(Bus* bus);
    bool isConnected() const;

private:
    Bus* bus_ = nullptr;
};