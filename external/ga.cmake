include(ExternalProject)
ExternalProject_Add(
  ga
  PREFIX ${CMAKE_BINARY_DIR}/ga
  URL ${PROJECT_SOURCE_DIR}/external/ga-5.7.tar.gz
  #URL_MD5
  TIMEOUT 600
  PATCH_COMMAND patch -p0 < ${PROJECT_SOURCE_DIR}/external/ga-5.7.patch
  # CONFIGURE_COMMAND ""
  # BUILD_COMMAND ""
  BUILD_IN_SOURCE ON
  CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/ga
             -DCMAKE_POSITION_INDEPENDENT_CODE=ON
)

set(GA_LIBRARIES ${CMAKE_BINARY_DIR}/ga/lib/libga.a ${CMAKE_BINARY_DIR}/ga/lib/libarmci.a)
