include(ExternalProject)
ExternalProject_Add(
  pybind11
  PREFIX ${CMAKE_BINARY_DIR}/pybind11
  URL ${PROJECT_SOURCE_DIR}/external/pybind11-2.2.4.tar.gz
  #URL_MD5 
  TIMEOUT 600
  PATCH_COMMAND patch -p0 < ${PROJECT_SOURCE_DIR}/external/pybind11.patch
  # CONFIGURE_COMMAND ""
  BUILD_IN_SOURCE ON
  BUILD_COMMAND ""
  CMAKE_ARGS -DPYBIND11_TEST=FALSE -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/pybind11
  # INSTALL_COMMAND ${CMAKE_COMMAND} -E copy_directory ${EigenInstallPrefix}/src/eigen/Eigen ${EigenInstallPrefix}/include/Eigen
  #         COMMAND ${CMAKE_COMMAND} -E copy_directory ${EigenInstallPrefix}/src/eigen/unsupported ${EigenInstallPrefix}/include/unsupported
)
