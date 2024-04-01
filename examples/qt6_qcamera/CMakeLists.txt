
cmake_minimum_required( VERSION 3.1 )

if( NOT CMAKE_BUILD_TYPE )
	set( CMAKE_BUILD_TYPE "Release"
		CACHE STRING "Choose the type of build."
		FORCE)
endif( NOT CMAKE_BUILD_TYPE )

SET( CMAKE_CXX_STANDARD 14 )

SET( CMAKE_CXX_STANDARD_REQUIRED ON )

project( Qt6QCamera )

set( CMAKE_AUTOMOC ON )
set( CMAKE_AUTORCC ON )
set( CMAKE_AUTOUIC ON )

find_package( Qt6Core REQUIRED )
find_package( Qt6Qml REQUIRED )
find_package( Qt6Multimedia REQUIRED )
find_package( Qt6Gui REQUIRED )
find_package( Qt6Quick REQUIRED )
find_package( Qt6Widgets REQUIRED )

set( SRC frames.cpp
    frames.hpp
    main.cpp
    main.qml )

set( CMAKE_CXX_STANDARD 14 )

set( CMAKE_CXX_STANDARD_REQUIRED ON )

qt6_add_resources( SRC resources.qrc )

qt_add_executable( qt6_qcamera
	MANUAL_FINALIZATION
	${SRC} )

set_property( TARGET qt6_qcamera APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
	${CMAKE_CURRENT_SOURCE_DIR}/android )

target_link_libraries( qt6_qcamera PUBLIC Qt::Qml Qt::Quick Qt::Multimedia Qt::Widgets Qt::Gui Qt::Core )

qt_finalize_executable( qt6_qcamera )
