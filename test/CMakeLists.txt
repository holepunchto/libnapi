list(APPEND tests
  basic
  get-buffer-info-uint8array
  get-buffer-info-uint16array
  get-value-string-utf8
  get-value-string-utf8-length
  get-value-string-utf8-truncate
  module
  module-constructor
)

foreach(test IN LISTS tests)
  add_executable(${test} ${test}.c)

  target_link_libraries(
    ${test}
    PRIVATE
      napi_shared
  )

  add_test(
    NAME ${test}
    COMMAND ${test}
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
  )

  set_tests_properties(
    ${test}
    PROPERTIES
    TIMEOUT 30
  )
endforeach()
