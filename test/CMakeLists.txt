list(APPEND tests
  basic
  callback-uncaught-exception
  cleanup-hook-errors
  create-async-work
  create-async-work-without-complete
  create-buffer-copy
  create-buffer-from-arraybuffer
  create-view-out-of-range
  define-properties-const-descriptors
  get-buffer-info-uint8array
  get-buffer-info-uint16array
  get-info-type-mismatch
  get-instance-data
  get-all-property-names-filter
  get-version
  get-value-string-utf8
  get-value-string-utf8-length
  get-value-string-utf8-truncate
  get-value-string-null-buffer
  get-value-type-mismatch
  invalid-args
  module
  module-constructor
  null-args
  object-freeze
  object-seal
  object-type-mismatch
  post-finalizer
  run-script-uncaught-exception
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
