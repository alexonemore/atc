# This file is part of ATC (Adiabatic Temperature Calculator).
# Copyright (c) 2022 Alexandr Shchukin
# Corresponding email: shchukin@ism.ac.ru
#
# ATC (Adiabatic Temperature Calculator) is free software:
# you can redistribute it and/or modify it under the terms of
# the GNU General Public License as published by
# the Free Software Foundation, version 3.
#
# ATC (Adiabatic Temperature Calculator) is distributed in the hope that
# it will be useful, but WITHOUT ANY WARRANTY; without even the implied
# warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
# See the GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with ATC (Adiabatic Temperature Calculator).
# If not, see <http://www.gnu.org/licenses/>.

cmake_minimum_required(VERSION 3.21)

project(atc VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt5 Qt6 REQUIRED COMPONENTS Widgets Sql Concurrent)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Sql Concurrent)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS DataVisualization)

message(STATUS "Qt version: Qt${QT_VERSION_MAJOR}")

set(PROJECT_SOURCES
	# core
	src/main.cpp
	src/mainwindow.h
	src/mainwindow.cpp
	src/mainwindow.ui
	src/coreapplication.h
	src/coreapplication.cpp

	# tmp
	src/heavycontainer.h
	src/heavycontainer.cpp

	# atc
	src/atc/thermodynamics.h
	src/atc/thermodynamics.cpp

	# plots
	src/plots/plots.h
	src/plots/plot2dbase.h
	src/plots/plot2dbase.cpp
	src/plots/plot2dgraph.h
	src/plots/plot2dgraph.cpp
	src/plots/plot2dheatmap.h
	src/plots/plot2dheatmap.cpp
	src/plots/plot3dsurface.h
	src/plots/plot3dsurface.cpp
	src/plots/plot3dsurface.ui

	# plots auxiliary dialogs
	src/plots/dialogchangetext.h
	src/plots/dialogchangetext.cpp
	src/plots/dialogchangetext.ui
	src/plots/dialogchangegraphsettings.h
	src/plots/dialogchangegraphsettings.cpp
	src/plots/dialogchangegraphsettings.ui

	# models

	# views

	# misc
	src/misc/utilities.h
	src/misc/utilities.cpp
	
	# logo
	images/logo.rc
)

set(ICON_FILES
	images/bc.svg
	images/bl.svg
	images/br.svg
	images/cancel.svg
	images/clear.svg
	images/copy.svg
	images/crosshair.svg
	images/mc.svg
	images/ml.svg
	images/mr.svg
	images/ok.svg
	images/tc.svg
	images/tl.svg
	images/tr.svg
	images/update.svg
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
	qt_add_executable(${PROJECT_NAME}
		MANUAL_FINALIZATION
		${PROJECT_SOURCES}
	)
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET mta APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
	if(ANDROID)
		add_library(${PROJECT_NAME} SHARED
			${PROJECT_SOURCES}
		)
	# Define properties for Android with Qt 5 after find_package() calls as:
	#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
else()
	add_executable(${PROJECT_NAME}
		${PROJECT_SOURCES}
	)
endif()
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(${PROJECT_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::Sql)
target_link_libraries(${PROJECT_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::Concurrent)
target_link_libraries(${PROJECT_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::DataVisualization)

set_target_properties(${PROJECT_NAME} PROPERTIES
	MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
	MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
	MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
	MACOSX_BUNDLE TRUE
	WIN32_EXECUTABLE TRUE
)

if(QT_VERSION_MAJOR EQUAL 6)
	qt_finalize_executable(${PROJECT_NAME})
endif()

target_include_directories(${PROJECT_NAME}
	PRIVATE ${CMAKE_SOURCE_DIR}/src
	PRIVATE ${CMAKE_SOURCE_DIR}/src/atc
	PRIVATE ${CMAKE_SOURCE_DIR}/src/misc
	PRIVATE ${CMAKE_SOURCE_DIR}/src/models
	PRIVATE ${CMAKE_SOURCE_DIR}/src/plots
	PRIVATE ${CMAKE_SOURCE_DIR}/src/views
	PRIVATE ${CMAKE_SOURCE_DIR}/images
	PRIVATE ${CMAKE_SOURCE_DIR}/databases
)

# libs
if(WIN32)	
	set(BUILD_SHARED_LIBS ON CACHE BOOL "" FORCE)
	set(QCUSTOMPLOT_BUILD_TYPE SHARED)
	target_compile_definitions(${PROJECT_NAME} PRIVATE QCUSTOMPLOT_USE_LIBRARY)
else()
	set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
	set(QCUSTOMPLOT_BUILD_TYPE STATIC)	
endif()
message(STATUS "BUILD_SHARED_LIBS: ${BUILD_SHARED_LIBS}")

# qcustomplot
if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
	add_subdirectory(libs/qcustomplot-qt6-fixed)
else()
	add_subdirectory(libs/qcustomplot)
endif()
target_link_libraries(${PROJECT_NAME} PRIVATE qcustomplot)

# nlopt
add_subdirectory(libs/nlopt)
target_link_libraries(${PROJECT_NAME} PRIVATE nlopt)

# install
install(FILES ${ICON_FILES} DESTINATION ${CMAKE_INSTALL_PREFIX}/bin/images)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/databases
	DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
install(TARGETS ${PROJECT_NAME} qcustomplot nlopt
	RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
	LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
	ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
)

# copy files while building by QtCreator
if(DEFINED QT_QMAKE_EXECUTABLE)
	message(STATUS "QT_QMAKE_EXECUTABLE = ${QT_QMAKE_EXECUTABLE}")
	file(COPY ${ICON_FILES} DESTINATION ${CMAKE_BINARY_DIR}/images)
	file(COPY ${CMAKE_SOURCE_DIR}/databases/
		 DESTINATION ${CMAKE_BINARY_DIR}/databases/)
endif()

# no cmd window
if(WIN32 AND MSVC)
	if(CMAKE_CL_64)
		set_target_properties(${PROJECT_NAME} PROPERTIES
			LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS,5.02 /ENTRY:mainCRTStartup")
	else()
		set_target_properties(${PROJECT_NAME} PROPERTIES
			LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS,5.01 /ENTRY:mainCRTStartup")
	endif()
endif()
if(MINGW)
	set(CMAKE_EXE_LINKER_FLAGS
		"${CMAKE_EXE_LINKER_FLAGS} -Wl,-subsystem,windows")
endif()

