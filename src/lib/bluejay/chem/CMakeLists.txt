add_library(libbluejay-chem SHARED
  atom.cc basis.cc molecule.cc
  python.cc python/molecule.cc python/basis.cc
  python/integral.cc
  )

include_directories(${PROJECT_BINARY_DIR}/libint/include)

set_target_properties(libbluejay-chem PROPERTIES OUTPUT_NAME bluejay-chem)
target_link_libraries(
  libbluejay-chem
  libbluejay-core
  )

add_custom_target(
  bluejay-chem-basis
  COMMAND ${CMAKE_COMMAND} -E copy_directory basis ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/bluejay/chem/basis
)

add_dependencies(libbluejay-chem bluejay-chem-basis)

add_test(NAME basis COMMAND bluejay test/basis.py)
