###############################################################################
### This script finds Qt for this project.
###############################################################################

if ( NOT DEFINED ${PROJECT_NAME}_QT_VERSION )
	include(${PROJECT_SOURCE_DIR}/CMake/External/Scripts/SupportQt.cmake)
endif()

if(${PROJECT_NAME}_QT_VERSION VERSION_GREATER "4")
	
	message(STATUS QT_VERSION_MAJOR=${QT_VERSION_MAJOR}\n${PROJECT_NAME}_QT_VERSION=${${PROJECT_NAME}_QT_VERSION})

	set (QT_LIBRARIES Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Gui Qt${QT_VERSION_MAJOR}::Xml Qt${QT_VERSION_MAJOR}::PrintSupport Qt${QT_VERSION_MAJOR}::Sql)
	set (QT_MODULES Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Gui Qt${QT_VERSION_MAJOR}::Xml Qt${QT_VERSION_MAJOR}::PrintSupport Qt${QT_VERSION_MAJOR}::Sql)
	
if (WIN32)
	set (QT_PLATFORM_MODULES Qt${QT_VERSION_MAJOR}::QWindowsIntegrationPlugin)	
else(WIN32)
	set(QT_PLATFORM_MODULES Qt${QT_VERSION_MAJOR}::QXcbIntegrationPlugin)
endif(WIN32)
	
	if(${PROJECT_NAME}_QT_VERSION VERSION_LESS "6")
		set( QT_LIBRARIES ${QT_LIBRARIES} Qt${QT_VERSION_MAJOR}::WinMain)
		set (QT_MODULES ${QT_MODULES} Qt${QT_VERSION_MAJOR}::WinMain)
	else ()
		set( QT_LIBRARIES ${QT_LIBRARIES} Qt${QT_VERSION_MAJOR}::Core5Compat Qt${QT_VERSION_MAJOR}::StateMachine)
		set (QT_MODULES ${QT_MODULES} Qt${QT_VERSION_MAJOR}::Core5Compat Qt${QT_VERSION_MAJOR}::StateMachine)
	endif()
	
	find_qt5_packages()
	
else(${PROJECT_NAME}_QT_VERSION VERSION_GREATER "4")
	message(FATAL_ERROR "Qt4 is not supported")
endif(${PROJECT_NAME}_QT_VERSION VERSION_GREATER "4")

