include(ExternalProject)
ExternalProject_Add(
  libint
  PREFIX ${CMAKE_BINARY_DIR}/libint
  URL ${PROJECT_SOURCE_DIR}/external/libint-2.5.0-beta.2.tar.gz
  #URL_MD5
  TIMEOUT 600
  CONFIGURE_COMMAND ./autogen.sh
            COMMAND ./configure
                    --prefix=${CMAKE_BINARY_DIR}/libint
		    --with-boost=${CMAKE_BINARY_DIR}/boost
                    --enable-shared
		    CPPFLAGS=-I${CMAKE_BINARY_DIR}/boost/include
                    CXXFLAGS=${CMAKE_CXX_FLAGS}
                    CXX=${CMAKE_CXX_COMPILER}
  BUILD_IN_SOURCE ON
  # BUILD_COMMAND ""
)


set(LIBINT2_LIBRARY ${CMAKE_BINARY_DIR}/libint/lib/libint2.so)
