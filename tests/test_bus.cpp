#include <catch2/catch_test_macros.hpp>
#include "Bus.h"

TEST_CASE("TEST :: Bus starts cleared", "[bus]") {
    Bus bus;

    REQUIRE(bus.read(0x0000) == 0x00);
    REQUIRE(bus.read(0xFFFF) == 0x00);
}

TEST_CASE("TEST :: Bus write then read returns the same byte", "[bus]") {
    Bus bus;

    bus.write(0x1234, 0xAB);

    REQUIRE(bus.read(0x1234) == 0xAB);
    REQUIRE(bus.read(0x1235) != 0xAB);
}