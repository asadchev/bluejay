add_subdirectory(bluejay)

add_custom_target(
  bluejay-python
  COMMAND ${CMAKE_COMMAND} -E copy_directory python ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/python
)
