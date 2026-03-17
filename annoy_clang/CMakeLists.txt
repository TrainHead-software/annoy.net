cmake_minimum_required(VERSION 3.10)
project(annoy_c_wrapper)

set(CMAKE_CXX_STANDARD 17)
enable_testing()

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

function(add_annoy_wrapper_test test_name)
    add_executable(${test_name} test/${test_name}.c)
    target_link_libraries(${test_name} annoy_c)
    add_test(NAME ${test_name} COMMAND $<TARGET_FILE:${test_name}>)
    set_tests_properties(${test_name} PROPERTIES WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
endfunction()

add_annoy_wrapper_test(test_metric_enum)
add_annoy_wrapper_test(test_angular_lifecycle)
add_annoy_wrapper_test(test_angular_queries)
add_annoy_wrapper_test(test_angular_persistence)
add_annoy_wrapper_test(test_metric_distances)
add_annoy_wrapper_test(test_build_options)
add_annoy_wrapper_test(test_hamming_queries)
add_annoy_wrapper_test(test_hamming_persistence)
