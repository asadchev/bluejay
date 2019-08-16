# Installation prefix
set(eigen_install_prefix ${CMAKE_BINARY_DIR}/eigen)

include(ExternalProject)
ExternalProject_Add(
  rapidjson
  PREFIX ${CMAKE_BINARY_DIR}/rapidjson
  URL ${PROJECT_SOURCE_DIR}/external/rapidjson-1.1.0.tar.gz
  #URL_MD5
  TIMEOUT 600
  CONFIGURE_COMMAND ""
  BUILD_COMMAND ""
  BUILD_IN_SOURCE ON
  INSTALL_COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_BINARY_DIR}/rapidjson/src/rapidjson/include ${CMAKE_BINARY_DIR}/rapidjson/include
)
