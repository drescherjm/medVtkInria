################################################################################
#
# medInria
#
# Copyright (c) INRIA 2013 - 2018. All rights reserved.
# See LICENSE.txt for details.
#
#  This software is distributed WITHOUT ANY WARRANTY; without even
#  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
#  PURPOSE.
#
################################################################################

if(DEFINED ENV{VCPKG_DEFAULT_TRIPLET} AND NOT DEFINED VCPKG_TARGET_TRIPLET)
  set(VCPKG_TARGET_TRIPLET "$ENV{VCPKG_DEFAULT_TRIPLET}" CACHE STRING "")
endif()

if(DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
      CACHE STRING "")
endif()

include(CMake/SupportSwitchingQtVersionAsAFeature.cmake)

cmake_policy(SET CMP0003 NEW)
cmake_policy(SET CMP0020 NEW)
cmake_policy(SET CMP0074 NEW)
cmake_policy(SET CMP0011 NEW)

cmake_minimum_required(VERSION 3.21)
project(medVtkInriaEx)

set (${PROJECT_NAME}_VERSION_MAJOR 3)
set (${PROJECT_NAME}_VERSION_MINOR 0)
set (${PROJECT_NAME}_VERSION_PATCH 0)

set (${PROJECT_NAME}_VERSION_PATCH_CLEAN ${${PROJECT_NAME}_VERSION_PATCH})

if (DEFINED VCPKG_TARGET_TRIPLET)
	find_package(cmake-modules REQUIRED)
	LIST(APPEND CMAKE_MODULE_PATH ${cmake-modules_MODULE_DIR})
	LIST(APPEND CMAKE_MODULE_PATH ${cmake-modules_SCRIPTS_DIR})
	
	find_package(cmake-git-versioner REQUIRED)
	LIST(APPEND CMAKE_MODULE_PATH ${cmake-git-versioner_DIR})
	
else(DEFINED VCPKG_TARGET_TRIPLET)
	LIST(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/CMake/External/Modules)
	LIST(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/CMake/External/Scripts)
endif(DEFINED VCPKG_TARGET_TRIPLET)

if (EXISTS ${PROJECT_SOURCE_DIR}/.git) 
	set (DU_USE_GIT 1)
	include(GitVersioner)
	set (GIT_VERSIONER_AUTO_DETECT_CI false)	
    git_versioner_get_version(GIT_VERSION_RESULT)
	set (${PROJECT_NAME}_VERSION_PATCH ${${PROJECT_NAME}_VERSION_PATCH}.${GIT_VERSION_RESULT}-git)
else()
	set (DU_USE_SVN 1)
	include(${PROJECT_SOURCE_DIR}/CMake/External/Scripts/SubversionVersionInfo.cmake)
endif()

include(GNUInstallDirs)
include(GetCompilerVersion)
set (${PROJECT_NAME}_VERSION_STRING "${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}.${${PROJECT_NAME}_VERSION_PATCH}")
set(TARGET_NAME ${PROJECT_NAME})

include(SupportQt56)
include(HelperMacros)
include(SupportQt)
include(CMake/SetupQt.cmake)

#Use solution folders.
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

SET(EXECUTABLE_OUTPUT_PATH 
 ${CMAKE_BINARY_DIR}/bin 
 CACHE PATH "Single output directory for building all executables."
)

SET(LIBRARY_OUTPUT_PATH 
 ${CMAKE_BINARY_DIR}/lib 
 CACHE PATH "Single output directory for building all libraries."
)

option(${PROJECT_NAME}_BUILD_TESTS   		"Build tests"   OFF )
option(${PROJECT_NAME}_BUILD_DEVELOPMENT   	"Build development examples"   OFF )
option(DEBUG_DUMP_EXTRA_INFO "Add additional debug output in debug mode." OFF)

#########################################################################################

macro( conditional_define VariableName DefineName)
	if (${VariableName})
	
		message( STATUS "add_definitions(-D${DefineName})" )
		add_definitions(-D${DefineName})
		
	endif(${VariableName})
endmacro( conditional_define )

#########################################################################################

macro( conditional_not_define VariableName DefineName)
	if (NOT ${VariableName})
	
		message( STATUS "add_definitions(-D${DefineName})" )
		add_definitions(-D${DefineName})
		
	endif(NOT ${VariableName})
endmacro( conditional_not_define )

#########################################################################################

conditional_define(DEBUG_DUMP_EXTRA_INFO DEBUG_DUMP_EXTRA_INFO)

if(${PROJECT_NAME}_BUILD_TESTS)
    enable_testing()
endif()

if(MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

## #############################################################################
## Find required
## #############################################################################

if (VTK_VERSION VERSION_LESS "8.90.0")
find_package(VTK REQUIRED COMPONENTS 
  vtkCommonCore   
  vtkViewsQt
  vtkGUISupportQt
  vtkRenderingQt
  vtkRenderingVolumeOpenGL2 
  vtkInteractionWidgets 
  vtkRenderingAnnotation 
  vtkInteractionStyle 
  vtkImagingStatistics 
  vtkIOLegacy 
  vtkCommonDataModel
  vtkFiltersCore
  vtkFiltersGeometry
  vtkImagingSources
)
else()
find_package(VTK REQUIRED COMPONENTS 
  CommonCore   
  ViewsQt
  GUISupportQt
  RenderingQt
  RenderingVolumeOpenGL2 
  InteractionWidgets 
  RenderingAnnotation 
  InteractionStyle 
  ImagingStatistics 
  ImagingSources
  IOLegacy 
  FiltersCore
  CommonDataModel
  FiltersGeometry
)
endif()

include(${VTK_USE_FILE})

message (STATUS "VTK_VERSION: ${VTK_VERSION}")

find_package(OpenGL REQUIRED)

# find_package(Qt5 REQUIRED COMPONENTS
  # Concurrent
  # Core
  # Gui
  # Network
  # OpenGL
  # Quick
  # Sql
  # Svg
  # Test
  # Widgets
  # Xml
  # )
  
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

if (USE_OSPRay)
  add_definitions(-DMED_USE_OSPRAY_4_VR_BY_CPU)
  set(VTK_OSPRAY_RENDERING_LIBRARY "vtkRenderingOSPRay")
ELSE()
  set(VTK_OSPRAY_RENDERING_LIBRARY "")
endif()

## Additionals modules
## #############################################################################

set(${PROJECT_NAME}_CMAKE_DIR
  ${CMAKE_CURRENT_SOURCE_DIR}/CMake
  ${CMAKE_CURRENT_SOURCE_DIR}/CMake/module
  ${CMAKE_CURRENT_SOURCE_DIR}/CMake/External/Modules
  ${CMAKE_CURRENT_SOURCE_DIR}/CMake/External/Scripts
  )

set(CMAKE_MODULE_PATH
  ${${PROJECT_NAME}_CMAKE_DIR}
  ${CMAKE_MODULE_PATH}
  )

include(list_source_files)
include(list_header_directories_to_include)
include(set_exe_install_rules)
include(set_lib_install_rules)
include(set_lib_properties_variables)
include(set_plugin_install_rules)
include(set_plugin_install_rules_legacy)
include(HelperMacros)
include(GetCompilerVersion)
include(NamingConvention)
include(SubversionVersionInfo)
include(UnoptimizedRelWithDebInfo)
include(SetupProject)
include(SupportQt)
include(GetRuntime)

#dump_all_variables()

setup_project(${PROJECT_NAME})

include(add_plugins)

add_subdirectory(Libraries)

## #############################################################################
## Build tests
## #############################################################################

if(${PROJECT_NAME}_BUILD_TESTS)
  add_subdirectory(tests)
endif()


if(${PROJECT_NAME}_BUILD_DEVELOPMENT)
  add_subdirectory(Development)
endif()

#set (QT_LIBRARIES Qt5::Core Qt5::Network Qt5::Sql Qt5::Widgets Qt5::Gui Qt5::WinMain Qt5::Xml Qt5::PrintSupport)
#set (QT_MODULES Qt5::Core Qt5::Network Qt5::Sql Qt5::Widgets Qt5::Gui Qt5::WinMain Qt5::Xml Qt5::PrintSupport)
#set (QT_PLATFORM_MODULES Qt5::QWindowsIntegrationPlugin)

include(GetQtRuntime)

# install(TARGETS medVtkInriaEx EXPORT medVtkInriaEx)
# install(EXPORT medVtkInriaEx DESTINATION cmake)

export(TARGETS ${TARGET_NAME} FILE "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake")

