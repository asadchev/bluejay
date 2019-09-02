add_library(bluejay_hf MODULE hf.cc fock.cc python.cc)

set(hf_module_path ${CMAKE_BINARY_DIR}/lib/python/bluejay/hf/)

set_target_properties(bluejay_hf PROPERTIES
  PREFIX ""
  OUTPUT_NAME bluejay_hf
  LIBRARY_OUTPUT_DIRECTORY ${hf_module_path}
  )

set(
  python_files ${CMAKE_CURRENT_SOURCE_DIR}/python/__init__.py
  )

add_custom_target(
  bluejay-hf-python
  #COMMENT "Copying python files to ${hf_module_path}"
  COMMAND ${CMAKE_COMMAND} -E copy ${python_files} ${hf_module_path}
  #COMMAND ${CMAKE_COMMAND} -E copy ${python_files} ${hf_module_path}
  )

add_dependencies(bluejay_hf bluejay-hf-python)

target_link_libraries(
  bluejay_hf
  ${PYTHON_LIBRARIES}
  )

add_test(NAME hf-test COMMAND bluejay test.py)
