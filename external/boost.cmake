set(libraries system,filesystem)
set(user_config "using gcc : : ${CMAKE_CXX_COMPILER} $<SEMICOLON>")

set(b2_command ./b2 link=shared threading=multi runtime-link=shared variant=release toolset=gcc)
if (CMAKE_CXX_FLAGS)
  set(b2_command ${b2_command} cxxflags=${CMAKE_CXX_FLAGS})
endif()

include(ExternalProject)
ExternalProject_Add(boost
  PREFIX ${CMAKE_BINARY_DIR}/boost
  URL ${PROJECT_SOURCE_DIR}/external/boost_1_67_0.tar.bz2
  #URL_MD5 
  TIMEOUT 600
  BUILD_IN_SOURCE ON
  CONFIGURE_COMMAND ${CMAKE_COMMAND} -E echo ${user_config} > tools/build/src/user-config.jam
            COMMAND ./bootstrap.sh --prefix=${CMAKE_BINARY_DIR}/boost toolset=gcc --with-libraries=${libraries}
  BUILD_COMMAND ${b2_command} install
  INSTALL_COMMAND ""
)

include_directories(${CMAKE_BINARY_DIR}/boost/include)
set(Boost_FILESYSTEM_LIBRARY ${CMAKE_BINARY_DIR}/boost/lib/libboost_filesystem.so)
