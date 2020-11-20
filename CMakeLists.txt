# To build on Linux/macOS:
#   cmake -H. -Bbuild -DCMAKE_BUILD_TYPE=Release [Debug]
#   cmake --build build

cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_COMPILER g++-10)

# set the project name and version
project(vrp_solver VERSION 0.5.0)

option(CXX "enable C++ compilation" ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif(NOT CMAKE_BUILD_TYPE)


message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

include_directories(${PROJECT_SOURCE_DIR}/headers)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -Wno-deprecated-copy -pedantic-errors -DGRB")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -g")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -DNDEBUG")
SET(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -fopenmp")

# determine the Gurobi's necessary include and linker commands
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
find_package(GUROBI REQUIRED)
include_directories(${GUROBI_INCLUDE_DIRS})

# list all source files here
set(SOURCES
    src/main.cpp
    src/config_parameters.cpp
    src/init_grb_model.cpp
    src/instance.cpp
    src/vrp_lp.cpp
    src/ext/loguru/loguru.cpp)
    # src/callback/callback_sec.cpp
    # src/ext/cvrpsep/basegrph.cpp
    # src/ext/cvrpsep/capsep.cpp
    # src/ext/cvrpsep/cnstrmgr.cpp
    # src/ext/cvrpsep/compcuts.cpp
    # src/ext/cvrpsep/compress.cpp
    # src/ext/cvrpsep/cutbase.cpp
    # src/ext/cvrpsep/fcapfix.cpp
    # src/ext/cvrpsep/grsearch.cpp
    # src/ext/cvrpsep/memmod.cpp
    # src/ext/cvrpsep/mxf.cpp
    # src/ext/cvrpsep/sort.cpp
    # src/ext/cvrpsep/strngcmp.cpp)

# list all header files here
set(HEADERS
    include/config_parameters.hpp
    include/init_grb_model.hpp
    include/instance.hpp
    include/vrp_lp.hpp
    include/ext/loguru/loguru.hpp)
    # include/callback/callback_sec.hpp
    # include/ext/cvrpsep/basegrph.h
    # include/ext/cvrpsep/capsep.h
    # include/ext/cvrpsep/cnstrmgr.h
    # include/ext/cvrpsep/compcuts.h
    # include/ext/cvrpsep/compress.h
    # include/ext/cvrpsep/cutbase.h
    # include/ext/cvrpsep/fcapfix.h
    # include/ext/cvrpsep/grsearch.h
    # include/ext/cvrpsep/memmod.h
    # include/ext/cvrpsep/mxf.h
    # include/ext/cvrpsep/sort.h
    # include/ext/cvrpsep/strngcmp.h)

# add the executable
add_executable(${CMAKE_PROJECT_NAME} ${SOURCES} ${HEADERS})

target_link_libraries(${CMAKE_PROJECT_NAME} optimized ${GUROBI_CXX_LIBRARY} debug ${GUROBI_CXX_DEBUG_LIBRARY})
target_link_libraries(${CMAKE_PROJECT_NAME} ${GUROBI_LIBRARY})
target_link_libraries(${CMAKE_PROJECT_NAME} dl) # for fix -ldl linker error