cmake_minimum_required(VERSION 3.15)

project(LibCommuni VERSION 3.7.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

option(BUILD_WITH_QT6 "Build with Qt6" On)

if (BUILD_WITH_QT6)
    set(MAJOR_QT_VERSION "6")
else()
    set(MAJOR_QT_VERSION "5")
endif()

find_package(Qt${MAJOR_QT_VERSION} REQUIRED COMPONENTS Core Network)

if (BUILD_WITH_QT6)
    find_package(Qt${MAJOR_QT_VERSION} REQUIRED COMPONENTS Core5Compat)
endif()

set(LibCommuni_INCLUDE_DIR "${CMAKE_CURRENT_LIST_DIR}/include/")
set(LibCommuni_SOURCE_DIR  "${CMAKE_CURRENT_LIST_DIR}/src/")

set(LibCommuni_definitions "IRC_STATIC IRC_NAMESPACE=Communi")

add_subdirectory(src/core)
add_subdirectory(src/model)
add_subdirectory(src/util)

add_library(LibCommuni)

target_link_libraries(LibCommuni Core Model Util)

add_library(LibCommuni::LibCommuni ALIAS LibCommuni)

if (CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    include(FeatureSummary)
    feature_summary(WHAT ALL)
endif ()
