# Installation prefix
set(eigen_install_prefix ${CMAKE_BINARY_DIR}/eigen)

include(ExternalProject)
ExternalProject_Add(
  eigen
  PREFIX ${CMAKE_BINARY_DIR}/eigen
  URL ${PROJECT_SOURCE_DIR}/external/eigen-3.3.7.tar.gz
  #URL_MD5
  TIMEOUT 600
  CONFIGURE_COMMAND ""
  BUILD_COMMAND ""
  BUILD_IN_SOURCE ON
  INSTALL_COMMAND ${CMAKE_COMMAND} -E copy_directory ${eigen_install_prefix}/src/eigen/Eigen ${eigen_install_prefix}/include/Eigen
          COMMAND ${CMAKE_COMMAND} -E copy_directory ${eigen_install_prefix}/src/eigen/unsupported ${eigen_install_prefix}/include/unsupported
)
