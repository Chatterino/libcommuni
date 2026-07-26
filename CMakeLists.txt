cmake_minimum_required(VERSION 3.15)

project(LibCommuni VERSION 3.7.0)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (POLICY CMP0140)
    cmake_policy(SET CMP0140 NEW)
endif()

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

option(BUILD_WITH_QT6 "Build with Qt6" On)
option(LIBCOMMUNI_BUILD_TESTS "Build libcommuni tests" ${PROJECT_IS_TOP_LEVEL})
option(LIBCOMMUNI_BUILD_BENCHMARKS "Build libcommuni benchmarks" ${PROJECT_IS_TOP_LEVEL})

if (BUILD_WITH_QT6)
    set(MAJOR_QT_VERSION "6")
else()
    set(MAJOR_QT_VERSION "5")
endif()

find_package(Qt${MAJOR_QT_VERSION} REQUIRED COMPONENTS Core Network)

set(LibCommuni_INCLUDE_DIR "${CMAKE_CURRENT_LIST_DIR}/include/")
set(LibCommuni_SOURCE_DIR  "${CMAKE_CURRENT_LIST_DIR}/src/")

set(LibCommuni_definitions "IRC_STATIC IRC_NAMESPACE=Communi")

add_subdirectory(src/core)
add_subdirectory(src/model)
add_subdirectory(src/util)

add_library(LibCommuni)

target_link_libraries(LibCommuni Core Model Util)

add_library(LibCommuni::LibCommuni ALIAS LibCommuni)

if (LIBCOMMUNI_BUILD_BENCHMARKS OR LIBCOMMUNI_BUILD_TESTS)
    include(CTest)
    enable_testing()
    add_subdirectory(tests)
endif()

if (CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    include(FeatureSummary)
    feature_summary(WHAT ALL)
endif ()
