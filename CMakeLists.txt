cmake_minimum_required(VERSION 3.22)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

if(WIN32)
  set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()

project(napi C)

if(NOT TARGET js)
  add_subdirectory(vendor/libjs EXCLUDE_FROM_ALL)
endif()

add_library(napi OBJECT)

set_target_properties(
  napi
  PROPERTIES
  C_STANDARD 99
  POSITION_INDEPENDENT_CODE 1
)

target_sources(
  napi
  INTERFACE
    include/napi.h
  PRIVATE
    src/napi.c
)

target_include_directories(
  napi
  PUBLIC
    include
    $<TARGET_PROPERTY:js,INCLUDE_DIRECTORIES>
)

add_library(napi_shared SHARED $<TARGET_OBJECTS:napi>)

set_target_properties(
  napi_shared
  PROPERTIES
  OUTPUT_NAME napi
)

target_link_libraries(
  napi_shared
  PUBLIC
    js_shared
)

add_library(napi_static STATIC $<TARGET_OBJECTS:napi>)

set_target_properties(
  napi_static
  PROPERTIES
  OUTPUT_NAME napi
)

target_link_libraries(
  napi_static
  PUBLIC
    js_static
)

install(TARGETS napi_shared napi_static)

install(FILES include/napi.h DESTINATION include)

if(PROJECT_IS_TOP_LEVEL)
  enable_testing()
  add_subdirectory(test)
endif()