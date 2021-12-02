
# Set the minimum version of CMake that's required
cmake_minimum_required(VERSION 3.12)

# Set the project name
project(Bench DESCRIPTION "Benchmarking fire" LANGUAGES CXX)

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

