add_executable(bluejay bluejay.cc)
add_dependencies(bluejay bluejay-python)
target_link_libraries(
  bluejay
  -Wl,--no-as-needed
  libbluejay-core
  libbluejay-ga
  -Wl,--as-needed
  ${MPI_C_LIBRARIES}
  )
#set_target_properties(bluejay PROPERTIES LINK_WHAT_YOU_USE TRUE)
