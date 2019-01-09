add_library(libbluejay-ga SHARED ga.cc python.cc)

include_directories(${PROJECT_BINARY_DIR}/ga/include)

set_target_properties(libbluejay-ga PROPERTIES OUTPUT_NAME bluejay-ga)
target_link_libraries(
  libbluejay-ga
  ${PYTHON_LIBRARIES}
  ${GA_LIBRARIES}
  ${MPI_C_LIBRARIES}
  ${LAPACK_LIBRARIES}
  ${BLAS_LIBRARIES}
  )

add_test(NAME ga COMMAND bluejay test.py)
