
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
  add_library(Bench_Event SHARED src/Bench/Event/Hit.cxx)
  target_link_libraries(Bench_Event PUBLIC fire::io)
  target_include_directories(Bench_Event PUBLIC 
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_PREFIX}/include>")
  if (fire_USE_ROOT)
    root_generate_dictionary(Bench_EventDict
      include/Bench/Event/Hit.h
      LINKDEF ${CMAKE_CURRENT_SOURCE_DIR}/include/Bench/Event/LinkDef.h
      MODULE Bench_Event)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/Bench_Event_rdict.pcm DESTINATION lib)
  endif()
  
  add_library(Bench SHARED 
    src/Bench/Produce.cxx
    src/Bench/Recon.cxx
    )
  target_include_directories(Bench PUBLIC include)
  target_link_libraries(Bench PUBLIC Bench_Event fire::framework)
  
  install(TARGETS Bench_Event Bench DESTINATION lib)
endif()
