#   Copyright 2017 James Fong
#
#   Licensed under the Apache License, Version 2.0 (the "License");
#   you may not use this file except in compliance with the License.
#   You may obtain a copy of the License at
#
#       http://www.apache.org/licenses/LICENSE-2.0
#
#   Unless required by applicable law or agreed to in writing, software
#   distributed under the License is distributed on an "AS IS" BASIS,
#   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#   See the License for the specific language governing permissions and
#   limitations under the License.

##### PEPPERGRAINS ENGINE CMAKE CONFIG #####

### Basic CMake project and build information ###

# CMake minimum version
cmake_minimum_required(VERSION 3.0.2)

# Project name/version
project(PEPPERGRAINS VERSION 0.1.0)
message(STATUS "Peppergrains Engine version ${PEPPERGRAINS_VERSION}")

# Where to find modules
list(APPEND CMAKE_MODULE_PATH "${PEPPERGRAINS_SOURCE_DIR}/cmake")

### Variables starting with "PGLOCAL_" are not CMake built-ins

# Where the .cpp files reside
set(PGLOCAL_SOURCE_DIR "${PEPPERGRAINS_SOURCE_DIR}/src/PepperGrains")

# Where the header files reside
set(PGLOCAL_INCLUDE_DIRS ${PGLOCAL_SOURCE_DIR})

# Name of target executable
set(PGLOCAL_ENGINE_TARGET "Pegr")

### User options ###

### Build target configuration ###

# Load the sources list from a file as PGLOCAL_SOURCES_LIST
include("EngineSrcList")

# Add a new build target
add_executable(${PGLOCAL_ENGINE_TARGET} ${PGLOCAL_SOURCES_LIST})

# Add required features
set_property(TARGET ${PGLOCAL_ENGINE_TARGET} PROPERTY CXX_STANDARD 11)

### Locate packages ###

set(PGLOCAL_ALL_REQUIRED_READY TRUE)
message(STATUS "Package status: ")

# Lua #
message(STATUS "Lua 5.3:")
find_package(Lua)
if(LUA_FOUND)
    if(LUA_VERSION_MAJOR EQUAL 5 AND NOT LUA_VERSION_MINOR LESS 3)
        message(STATUS "\tFOUND VERSION: " ${LUA_VERSION_STRING})
        message(STATUS "\tInclude Dirs: " ${LUA_INCLUDE_DIR})
        message(STATUS "\tLibraries: " ${LUA_LIBRARY})
        list(APPEND PGLOCAL_INCLUDE_DIRS ${LUA_INCLUDE_DIR})
        target_link_libraries(${PGLOCAL_ENGINE_TARGET} ${LUA_LIBRARY})
    else()
        message("\tINCOMPATIBLE VERSION: " ${LUA_VERSION_STRING})
        set(PGLOCAL_ALL_REQUIRED_READY FALSE)
    endif()
else()
    message("\tNOT FOUND")
    set(PGLOCAL_ALL_REQUIRED_READY FALSE)
endif()

# GLFW #
message(STATUS "GLFW 3.2")
find_package(GLFW)
if(GLFW_FOUND)
    if(GLFW_VERSION_MAJOR EQUAL 3 AND NOT GLFW_VERSION_MINOR LESS 2)
        message(STATUS "\tFOUND VERSION: " ${GLFW_VERSION_STRING})
        message(STATUS "\tInclude Dirs: " ${GLFW_INCLUDE_DIR})
        message(STATUS "\tLibraries: " ${GLFW_LIBRARY})
        list(APPEND PGLOCAL_INCLUDE_DIRS ${GLFW_INCLUDE_DIR})
        target_link_libraries(${PGLOCAL_ENGINE_TARGET} ${GLFW_LIBRARY})
    else()
        message("\tINCOMPATIBLE VERSION: " ${GLFW_VERSION_STRING})
        set(PGLOCAL_ALL_REQUIRED_READY FALSE)
    endif()
else()
    message("\tNOT FOUND")
    set(PGLOCAL_ALL_REQUIRED_READY FALSE)
endif()

# BOOST #
message(STATUS "Boost 1.59")
find_package(Boost COMPONENTS system filesystem)
if(Boost_FOUND)
    if(Boost_MAJOR_VERSION EQUAL 1 AND NOT Boost_MINOR_VERSION LESS 59)
        message(STATUS "\tFOUND VERSION: " ${Boost_VERSION})
        message(STATUS "\tInclude Dirs: " ${Boost_INCLUDE_DIRS})
        message(STATUS "\tLibraries: " ${Boost_LIBRARIES})
        list(APPEND PGLOCAL_INCLUDE_DIRS ${Boost_INCLUDE_DIRS})
        target_link_libraries(${PGLOCAL_ENGINE_TARGET} ${Boost_LIBRARIES})
    else()
        message("\tINCOMPATIBLE VERSION: " ${Boost_VERSION})
        set(PGLOCAL_ALL_REQUIRED_READY FALSE)
    endif()
else()
    message("\tNOT FOUND")
    set(PGLOCAL_ALL_REQUIRED_READY FALSE)
endif()


# Helpful information
if(PGLOCAL_ALL_REQUIRED_READY)
    message(STATUS "All packages found and are compatible")
else()
    message("One or more packages are missing, build will fail!")
endif()

# Setup include directories
include_directories(${PGLOCAL_INCLUDE_DIRS})
