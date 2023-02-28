list(APPEND tests
  basic
  get-value-string-utf8
  get-value-string-utf8-length
  get-value-string-utf8-truncate
  module
)

foreach(test IN LISTS tests)
  add_executable(${test} ${test}.c)

  target_link_libraries(
    ${test}
    PRIVATE
      napi_shared
  )

  target_include_directories(
    ${test}
    PRIVATE
      $<TARGET_PROPERTY:napi,INTERFACE_INCLUDE_DIRECTORIES>
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
