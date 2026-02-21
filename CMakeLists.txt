cmake_minimum_required(VERSION 3.20)
project(Z80Emu LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ---- Catch2 (portable) ----
include(FetchContent)

FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG v3.5.4
)

FetchContent_MakeAvailable(Catch2)

# ---- Core library (shared by app + tests) ----
add_library(z80core
    src/Bus.cpp
    src/Cpu.cpp
)

target_include_directories(z80core
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(z80core PUBLIC cxx_std_20)

# ---- Main app ----
add_executable(Z80Emu
    src/main.cpp
)

target_link_libraries(Z80Emu PRIVATE z80core)

# ---- Tests ----
enable_testing()

add_executable(z80_tests
    tests/test_bus.cpp
    tests/test_cpu.cpp
)

target_link_libraries(z80_tests PRIVATE Catch2::Catch2WithMain z80core)

include(Catch)
catch_discover_tests(z80_tests)