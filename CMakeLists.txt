# This file is part of ATC (Adiabatic Temperature Calculator).
# Copyright (c) 2023 Alexandr Shchukin
# Corresponding email: alexonemoreemail@gmail.com
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

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt5 Qt6 REQUIRED COMPONENTS Widgets Sql Concurrent)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Sql Concurrent)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS DataVisualization)

message(STATUS "Qt version: Qt${QT_VERSION_MAJOR}")

set(STATIC_BUILD ON)

if(STATIC_BUILD)
	set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
	# set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
	# set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
endif()

set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -ffast-math")

set(PROJECT_SOURCES
	# core
	src/main.cpp
	src/coreapplication.h
	src/coreapplication.cpp

	# GUI
	src/mainwindow.h
	src/mainwindow.cpp
	src/mainwindow.ui
	src/calculationparameters.h
	src/calculationparameters.cpp
	src/calculationparameters.ui
	src/periodictable.h
	src/periodictable.cpp
	src/periodictable.ui

	# atc
	src/atc/thermodynamics.h
	src/atc/thermodynamics.cpp
	src/atc/parameters.h
	src/atc/parameters.cpp
	src/atc/database.h
	src/atc/database.cpp
	src/atc/optimization.h
	src/atc/optimization.cpp

	# plots
	src/plots/plots.h
	src/plots/plots.cpp
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
	src/models/amountsmodel.h
	src/models/amountsmodel.cpp
	src/models/substancestablemodel.h
	src/models/substancestablemodel.cpp
	src/models/substancestemprangemodel.h
	src/models/substancestemprangemodel.cpp
	src/models/substancestabulatedtfmodel.h
	src/models/substancestabulatedtfmodel.cpp
	src/models/plottfmodel.h
	src/models/plottfmodel.cpp
	src/models/specialdelegates.h
	src/models/specialdelegates.cpp
	src/models/resultmodel.h
	src/models/resultmodel.cpp

	# views
	src/views/substancestableview.h
	src/views/substancestableview.cpp
	src/views/table.h
	src/views/table.cpp
	src/views/plottfview.h
	src/views/plottfview.cpp
	src/views/resultview.h
	src/views/resultview.cpp

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
	images/logo_atc.svg
)

set(DATABASE_FILES
	databases/database_hsc.db
	databases/database_thermo.db
)

# add images to binary file for qt5
if(${QT_VERSION_MAJOR} LESS 6)
	set(IMAGES_QRC ${CMAKE_BINARY_DIR}/images.qrc)
	message(STATUS "images.qrc is ${IMAGES_QRC}")
	file(WRITE ${IMAGES_QRC} "<!DOCTYPE RCC><RCC version=\"1.0\">\n<qresource>\n")
	foreach(ICON_FILE ${ICON_FILES})
	  file(APPEND ${IMAGES_QRC} "    <file alias=\"${ICON_FILE}\">${CMAKE_SOURCE_DIR}/${ICON_FILE}</file>\n")
    endforeach()
	file(APPEND ${IMAGES_QRC} "</qresource>\n</RCC>\n")
	qt5_add_resources(PROJECT_SOURCES ${IMAGES_QRC})
endif()


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

# add images to binary file for qt6
if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
	qt_add_resources(${PROJECT_NAME} "images" PREFIX "/" FILES ${ICON_FILES})
endif()

# libs
if(STATIC_BUILD)
	set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
	set(QCUSTOMPLOT_BUILD_TYPE STATIC)
else()
	set(BUILD_SHARED_LIBS ON CACHE BOOL "" FORCE)
	set(QCUSTOMPLOT_BUILD_TYPE SHARED)
	target_compile_definitions(${PROJECT_NAME} PRIVATE QCUSTOMPLOT_USE_LIBRARY)
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
install(FILES ${DATABASE_FILES} DESTINATION ${CMAKE_INSTALL_PREFIX}/bin/databases)
install(TARGETS ${PROJECT_NAME} qcustomplot nlopt
	RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
	LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
	ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
)

# copy files while building by QtCreator
if(DEFINED QT_QMAKE_EXECUTABLE)
	message(STATUS "QT_QMAKE_EXECUTABLE = ${QT_QMAKE_EXECUTABLE}")
	file(COPY ${DATABASE_FILES} DESTINATION ${CMAKE_BINARY_DIR}/databases)
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

