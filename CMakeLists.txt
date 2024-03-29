cmake_minimum_required(VERSION 3.22)

project(napi C)

if(NOT TARGET js)
  add_subdirectory(vendor/libjs EXCLUDE_FROM_ALL)
endif()

add_library(napi OBJECT)

set_target_properties(
  napi
  PROPERTIES
  C_STANDARD 99
  POSITION_INDEPENDENT_CODE ON
)

target_sources(
  napi
  INTERFACE
    include/napi.h
    include/napi/module.h
  PRIVATE
    src/napi.c
)

target_include_directories(
  napi
  PUBLIC
    include
    $<TARGET_PROPERTY:js,INTERFACE_INCLUDE_DIRECTORIES>
)

add_library(napi_shared SHARED)

set_target_properties(
  napi_shared
  PROPERTIES
  OUTPUT_NAME napi
  WINDOWS_EXPORT_ALL_SYMBOLS ON
)

target_link_libraries(
  napi_shared
  PUBLIC
    napi
    js_shared
)

add_library(napi_static STATIC)

set_target_properties(
  napi_static
  PROPERTIES
  OUTPUT_NAME napi
  PREFIX lib
)

target_link_libraries(
  napi_static
  PUBLIC
    napi
    js_static
)

install(TARGETS napi_shared napi_static)

install(FILES include/napi.h DESTINATION include)

install(DIRECTORY include/napi DESTINATION include)

if(PROJECT_IS_TOP_LEVEL)
  enable_testing()
  add_subdirectory(test)
endif()
