cmake_minimum_required(VERSION 3.0)

if (NOT CMAKE_BUILD_TYPE)
  set(default_build_type DEBUG)
  message(STATUS "Setting build type to '${default_build_type}' as none was specified.")
  set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE STRING "Choose the type of build." FORCE)
  # Set the possible values of build type for cmake-gui
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

#set (CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wall") # CACHE STRING "CXX Flags" FORCE)
message(STATUS "CXX Flags: ${CMAKE_CXX_FLAGS}")

project("BlueJay" CXX)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include(external/boost.cmake)
include_directories(${CMAKE_BINARY_DIR}/boost/include)

find_package(PythonLibs 3 REQUIRED)
message(STATUS "Found Python: ${PYTHON_LIBRARIES}")
include_directories(${PYTHON_INCLUDE_PATH})

include(external/pybind11.cmake)
include_directories(${CMAKE_BINARY_DIR}/pybind11/include)

include(external/eigen.cmake)
include_directories(${CMAKE_BINARY_DIR}/eigen/include)

include(external/rapidjson.cmake)
include_directories(${CMAKE_BINARY_DIR}/rapidjson/include)

find_package(MPI REQUIRED)
message(STATUS "Found MPI: ${MPI_C_LIBRARIES}")

find_package(BLAS REQUIRED)
message(STATUS "Found BLAS: ${BLAS_LIBRARIES}")

find_package(LAPACK REQUIRED)
message(STATUS "Found LAPACK: ${LAPACK_LIBRARIES}")

include(external/ga.cmake)

enable_testing()
add_subdirectory(src)
