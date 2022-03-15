
# Set the minimum version of CMake that's required
cmake_minimum_required(VERSION 3.12)

# Set the project name
project(Bench DESCRIPTION "Benchmarking fire" LANGUAGES CXX)

configure_file(${PROJECT_SOURCE_DIR}/include/Bench/Version.h.in 
               ${PROJECT_SOURCE_DIR}/include/Bench/Version.h)

if (USE_ROOT_FRAMEWORK)
  option(BUILD_EVENT_ONLY "Build the event library." ON)
  if(BUILD_EVENT_ONLY)
    # Search and configure ROOT
    find_package(ROOT CONFIG REQUIRED)
  
    register_event_object(module_path "Bench/Event" namespace "bench" 
                          class "Hit" type "collection" )
  
    setup_library(module Bench name Event
                  dependencies ROOT::Core
                  register_target)
    
    return()
  endif()
  
  setup_library(module Bench
    dependencies Framework::Framework Bench::Event)
  
else()
  find_package(fire)
  
  add_library(Bench_Event SHARED src/Bench/Event/Hit.cxx)
  target_include_directories(Bench_Event PUBLIC include)
  target_link_libraries(Bench_Event PUBLIC fire::h5)
  
  add_library(Bench SHARED 
    src/Bench/Produce.cxx
    src/Bench/Recon.cxx
    )
  target_include_directories(Bench PUBLIC include)
  target_link_libraries(Bench PUBLIC Bench_Event fire::framework)
  
  install(TARGETS Bench_Event Bench DESTINATION lib)
endif()
