cmake_minimum_required(VERSION 3.0)

set(CMAKE_BUILD_TYPE Debug)

#set (CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wall")
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

include(external/rapidjson.cmake)
include_directories(${CMAKE_BINARY_DIR}/rapidjson/include)
