#include <catch2/catch_test_macros.hpp>
#include "Cpu.h"
#include "Bus.h"

TEST_CASE("CPU can connect to a bus", "[cpu]") {
    Bus bus;
    Cpu cpu;

    REQUIRE_FALSE(cpu.isConnected());

    cpu.connect(&bus);

    REQUIRE(cpu.isConnected());
}