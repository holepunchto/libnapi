enable_language(CXX)

list(APPEND tests
  basic
  basic-inline
  module
)

foreach(test IN LISTS tests)
  add_executable(${test} ${test}.cc)

  target_link_libraries(
    ${test}
    PRIVATE
      napi_shared
  )

  target_include_directories(
    ${test}
    PRIVATE
      $<TARGET_PROPERTY:napi,INCLUDE_DIRECTORIES>
  )

  add_test(
    NAME ${test}
    COMMAND ${test}
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
  )
endforeach()
