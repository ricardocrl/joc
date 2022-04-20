cmake_minimum_required (VERSION 3.8)

project (joc)

set(REPOSITORY_ROOT ${CMAKE_CURRENT_SOURCE_DIR})

set(TESTS_DIR ${REPOSITORY_ROOT}/tests)
set(EXTERNAL_DIR ${REPOSITORY_ROOT}/external)
set(JOCLIB_INCLUDE_DIR ${REPOSITORY_ROOT}/include)
set(JOCLIB_SOURCE_DIR ${REPOSITORY_ROOT}/src)

#################################
## Build joclib
#################################

file(GLOB JOCLIB_SOURCES ${JOCLIB_SOURCE_DIR}/*)
add_library(joclib ${JOCLIB_SOURCES})

target_include_directories(joclib PUBLIC ${JOCLIB_INCLUDE_DIR})
target_include_directories(joclib SYSTEM PUBLIC ${EXTERNAL_DIR}/nlohmann/json/single_include)

target_compile_options(joclib PRIVATE
  -std=c++17
  -Werror
  -Wno-error=pointer-arith
)
    
#################################
## Sample executable using joblib
#################################

add_executable(joc_sample joc_sample.cpp)

target_compile_options(joc_sample PRIVATE
  -std=c++17
)
target_link_libraries(joc_sample joclib)
