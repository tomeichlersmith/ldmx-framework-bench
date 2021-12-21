# This is a sample project to test integration via add_subdirectory and CMakeConfig
cmake_minimum_required(VERSION 3.12)

project(ldmx-rootless VERSION 0.1)

# If an install location hasn't been set via CMAKE_INSTALL_PREFIX, set it to 
# a reasonable default ($PWD/install).  
if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/.container-install CACHE PATH "" FORCE)
  message(STATUS "Install directory set to ${CMAKE_INSTALL_PREFIX}")
endif()

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

if(NOT DEFINED CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 17)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
  set(CMAKE_CXX_EXTENSIONS OFF)
endif()

add_subdirectory(fire)

add_subdirectory(Bench)
