cmake_minimum_required(VERSION 3.10)

set(CMAKE_AUTOMOC ON)

find_package(QT NAMES Qt5 REQUIRED COMPONENTS Widgets PrintSupport)
find_package(Qt5 REQUIRED COMPONENTS Widgets PrintSupport)

set(CMAKE_INCLUDE_CURRENT_DIR_IN_INTERFACE TRUE)

if(NOT DEFINED QCUSTOMPLOT_BUILD_TYPE)
	set(QCUSTOMPLOT_BUILD_TYPE SHARED)
endif()

message(STATUS "QCUSTOMPLOT_BUILD_TYPE: ${QCUSTOMPLOT_BUILD_TYPE}")

set(QCUSTOMPLOT_SOURCES
	qcustomplot.h
	qcustomplot.cpp
)

add_library(qcustomplot ${QCUSTOMPLOT_BUILD_TYPE} ${QCUSTOMPLOT_SOURCES})

if(QCUSTOMPLOT_BUILD_TYPE STREQUAL SHARED)
	target_compile_definitions(qcustomplot PRIVATE QCUSTOMPLOT_COMPILE_LIBRARY)
endif()

target_link_libraries(qcustomplot PRIVATE
	Qt${QT_VERSION_MAJOR}::Widgets
	Qt${QT_VERSION_MAJOR}::PrintSupport
)


