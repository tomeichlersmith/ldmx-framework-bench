
# Set the minimum version of CMake that's required
cmake_minimum_required(VERSION 3.12)

# Set the project name
project(Bench DESCRIPTION "Benchmarking fire" LANGUAGES CXX)

find_package(fire)

add_library(Bench_Event SHARED src/Bench/Event/Hit.cxx)
target_include_directories(Bench_Event PUBLIC include)
target_link_libraries(Bench_Event PUBLIC fire::h5)

add_library(Bench SHARED src/Bench/Produce.cxx)
target_include_directories(Bench PUBLIC include)
target_link_libraries(Bench PUBLIC Bench_Event fire::framework)

install(TARGETS Bench_Event Bench
        DESTINATION lib)
