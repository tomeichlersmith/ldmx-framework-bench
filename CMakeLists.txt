# This is a sample project to test integration via add_subdirectory and CMakeConfig
cmake_minimum_required(VERSION 3.12)

project(ldmx-framework-bench) 

# If a user is building outside of a Docker or Singularity environment, 
# warn them. 
if (NOT EXISTS /.dockerenv AND NOT EXISTS /singularity)
  message(WARNING "You are not inside a container; you may be working in an untested environment.")
endif()

# Set the default release type to "Release". If a release type is specified 
# at the command line, it's respected.
set(default_build_type "Release")
if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to '${default_build_type}' as none was specified.")
  set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE STRING "Choose the type of build." FORCE)
endif()

# If an install location hasn't been set via CMAKE_INSTALL_PREFIX, set it to 
# a reasonable default ($PWD/install).  
if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX $ENV{LDMX_BASE}/.container-install CACHE PATH "" FORCE)
  message(STATUS "Install directory set to ${CMAKE_INSTALL_PREFIX}")
endif()

if(NOT DEFINED CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 17)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
  set(CMAKE_CXX_EXTENSIONS OFF)
endif()

option(USE_ROOT_FRAMEWORK "Use ROOT-based framework instead of HDF5-based one" OFF)
if (USE_ROOT_FRAMEWORK)

  # Load the BuildMacros module. If loaded correctly, the variable 
  # 'build_macros_found' will be set to the path of the module.  Otherwise, it
  # is set to NOTFOUND.
  list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake/)
  include(BuildMacros RESULT_VARIABLE build_macros_found)

  # Set the python installation path
  set(PYTHON_PACKAGE_NAME LDMX)
  
  # Start by building all of the classes needed for building the event bus and 
  # ROOT dictionary. This is on by forcing the global "BUILD_EVENT_ONLY" option
  # here.
  set(BUILD_EVENT_ONLY ON CACHE BOOL "Build the event dictionary." FORCE)
  
  add_subdirectory(Bench ${CMAKE_BINARY_DIR}/BenchEvent)
  
  # Once the event libraries have been built, turn off the global option. 
  set(BUILD_EVENT_ONLY OFF CACHE BOOL "Build the event dictionary." FORCE)
  
  # The framework has all the backend code needed to run all processors.
  add_subdirectory(Framework)
else()
  find_package(fire QUIET)
  if (NOT fire_FOUND)
    # pre-compiled version not installed
    #   compile it here
    #   need to promote HighFive package find
    #   to this package scope
    find_package(HighFive REQUIRED)
    add_subdirectory(fire)
    # add aliases so the Bench/CMakeLists
    #   doesn't have to change
    add_library(fire::io ALIAS io)
    add_library(fire::framework ALIAS framework)
  endif()
endif()

add_subdirectory(Bench)

