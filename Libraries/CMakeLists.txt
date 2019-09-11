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

cmake_policy(SET CMP0003 NEW)
cmake_policy(SET CMP0020 NEW)

## #############################################################################
## List sources
## #############################################################################

list_source_files(${TARGET_NAME}
  ${CMAKE_CURRENT_SOURCE_DIR}
  vtkImageView
  vtkWidgetsAddOn
  vtkVisuManagement
  vtkVisuManagement/lut
  HWShading
  HWShading/helpers
  )

list_header_directories_to_include(${TARGET_NAME}
  ${${TARGET_NAME}_HEADERS}
  )

## #############################################################################
## write header shader
## #############################################################################


include(HWShading/cmake/shader_to_header.cmake)

# see HWShading/cmake/shader_to_header.cmake
shader_to_header(HWShading/shaders/AnisotropicLighting.vert
  AnisotropicLightingVertexText
  )

shader_to_header(HWShading/shaders/AnisotropicLighting.frag
  AnisotropicLightingFragmentText
  )

shader_to_header(HWShading/shaders/ShadowMapping.vert
  ShadowMappingVertexText
  )

shader_to_header(HWShading/shaders/ShadowMapping.frag
  ShadowMappingFragmentText
  )

shader_to_header(HWShading/shaders/Spotlight.func
  SpotlightFunctionsText
  )

shader_to_header(HWShading/shaders/AnisotropicLighting.func
  AnisotropicLightingFunctionsText
  )

shader_to_header(HWShading/shaders/AnisoLiShadowMap.frag
  AnisotropicLightingShadowMappingFragmentText
  )

shader_to_header(HWShading/shaders/AnisoLiShadowMap.vert
  AnisotropicLightingShadowMappingVertexText
  )

shader_to_header(HWShading/shaders/BuildShadowMap.vert
  BuildShadowMapVertexText
  )

shader_to_header(HWShading/shaders/BuildShadowMap.frag
  BuildShadowMapFragmentText
  )

shader_to_header(HWShading/shaders/BuildShadowMapLines.vert
  BuildShadowMapLinesVertexText
  )

shader_to_header(HWShading/shaders/BuildShadowMapLines.frag
  BuildShadowMapLinesFragmentText
  )

## #############################################################################
## add library
## #############################################################################

add_library(${TARGET_NAME} SHARED
  ${${TARGET_NAME}_CFILES}
  )

## #############################################################################
## include directorie.
## #############################################################################

target_include_directories(${TARGET_NAME}
  PUBLIC
  ${${TARGET_NAME}_INCLUDE_DIRS}
  ${CMAKE_BINARY_DIR}/shaders
  )

## #############################################################################
## Link
## #############################################################################

target_link_libraries(${TARGET_NAME}
  ${OPENGL_LIBRARIES}
  Qt5::Core
  Qt5::Widgets
  #medCoreLegacy
  vtkCommonCore
  vtkCommonSystem
  vtkImagingColor
  vtkImagingGeneral
  vtkImagingHybrid
  vtkIOCore
  vtkIOExportOpenGL2
  vtkIOGeometry
  vtkIOImage
  vtkIOLegacy
  vtkIOXML
  vtkRenderingAnnotation
  vtkRenderingCore
  vtkRenderingOpenGL2
  vtkRenderingVolumeOpenGL2
  vtkRenderingContextOpenGL2
  ${VTK_OSPRAY_RENDERING_LIBRARY}
  vtkInteractionWidgets
  vtkInteractionStyle
  vtkFiltersExtraction
  vtkFiltersModeling
  vtkImagingStencil
  vtkFiltersGeometry
  vtksys
  )


## #############################################################################
## install
## #############################################################################

set_lib_install_rules(${TARGET_NAME}
  ${${TARGET_NAME}_HEADERS}
  )


