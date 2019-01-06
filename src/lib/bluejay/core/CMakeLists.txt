add_library(libbluejay-core SHARED
  bluejay.cc system.cc json.cc
  python.cc python/json.cc
  )

set_target_properties(libbluejay-core PROPERTIES OUTPUT_NAME bluejay-core)
target_link_libraries(
  libbluejay-core
  ${PYTHON_LIBRARIES}
  ${Boost_FILESYSTEM_LIBRARY}
  )
