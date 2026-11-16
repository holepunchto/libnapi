cmake_minimum_required(VERSION 4.0)

find_package(cmake-fetch REQUIRED PATHS node_modules/cmake-fetch)
find_package(cmake-harden REQUIRED PATHS node_modules/cmake-harden)
find_package(cmake-prelude REQUIRED PATHS node_modules/cmake-prelude)
find_package(cmake-test REQUIRED PATHS node_modules/cmake-test)

project(napi LANGUAGES C VERSION 0.0.0)

fetch_package("github:holepunchto/libjs")

add_library_targets(
  napi
  HEADERS
    include/napi.h
    include/napi/module.h
  FEATURES
    c_std_99
  SOURCES
    src/napi.c
  LINK
    js::<TYPE>
    utf::<TYPE>
  EXPORT OFF
)

harden(napi)

add_test_subdirectory(test)
