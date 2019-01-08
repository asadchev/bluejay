add_library(libbluejay-libint2 MODULE libint2.cc python.cc)

set_target_properties(libbluejay-libint2 PROPERTIES
  PREFIX ""
  OUTPUT_NAME libint2
  LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib/python/bluejay
  )

include_directories(${PROJECT_BINARY_DIR}/libint/include)

target_link_libraries(
  libbluejay-libint2
  libbluejay-core
  ${LIBINT2_LIBRARY}
  )

add_test(NAME libint2 COMMAND bluejay test.py)
