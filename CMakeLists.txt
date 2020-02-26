cmake_minimum_required(VERSION 3.0.0)

project(omegah_examples VERSION 0.0.0 LANGUAGES CXX)

#Settings options for testing
enable_testing()
include(CTest)

#get the mpirun binary/script
get_filename_component(COMPILER_DIR "${CMAKE_CXX_COMPILER}" PATH)
find_program(MPIRUN NAMES mpirun PATHS "${COMPILER_DIR}")
set(MPIRUN_PROCFLAG "-np" CACHE STRING
  "the command line flag to give process count to MPIRUN")

find_package(Omega_h 9 REQUIRED)
set(pumipic_USE_Omega_h_DEFAULT ON)
message(STATUS "Found Omega_h: ${Omega_h_VERSION}")

set(debug_flag)
if (OHE_ENABLE_DEBUG_SYMBOLS)
  set(debug_flag "-g")
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${debug_flag}")
message(STATUS "CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS}")

function(example exename srcname)
  add_executable(${exename} ${srcname})
  target_link_libraries(${exename} Omega_h::omega_h)
endfunction(example)

example(classification classification.cpp)
example(reduction reduction.cpp)
example(synchronization synchronization.cpp)
example(partitioning partitioning.cpp)
example(ghosting ghosting.cpp)
include(testing.cmake)
