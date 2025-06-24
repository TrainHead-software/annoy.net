cmake_minimum_required(VERSION 3.10)
project(annoy_c_wrapper)

set(CMAKE_CXX_STANDARD 17)

set(ANNOY_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../annoy/src)

include_directories(${ANNOY_SRC_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)

add_library(annoy_c SHARED
        ${ANNOY_SRC_DIR}/annoylib.h
        ${ANNOY_SRC_DIR}/kissrandom.h
        src/annoy_c_wrapper.h
        src/annoy_c_wrapper.cpp
)

target_compile_definitions(annoy_c PRIVATE BUILDING_ANNOY_C_DLL)
target_include_directories(annoy_c PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(test_annoy test/test.c)
target_link_libraries(test_annoy annoy_c)
