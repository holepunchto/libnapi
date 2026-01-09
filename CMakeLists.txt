cmake_minimum_required(VERSION 3.22)

find_package(cmake-fetch REQUIRED PATHS node_modules/cmake-fetch)
find_package(cmake-harden REQUIRED PATHS node_modules/cmake-harden)

project(napi C)

fetch_package("github:holepunchto/libjs")

add_library(napi OBJECT)

harden(napi)

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
)

target_link_libraries(
  napi
  PUBLIC
    js
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
