myproject(LAGUIExample3B)

find_package(VTK
  COMPONENTS
    vtkCommonCore
    vtkGUISupportQt
    vtkIOImage
    vtkImagingColor
    vtkImagingGeneral
    vtkInteractionImage
    vtkInteractionStyle
    vtkInteractionWidgets)
if (NOT VTK_FOUND)
  message("Skipping example: ${VTK_NOT_FOUND_MESSAGE}")
  return ()
endif ()
include(${VTK_USE_FILE})

find_package( ITK REQUIRED )
include( ${ITK_USE_FILE} )

set( ${LOCAL_PROJECT_NAME}_SRCS
	./src/main.cxx
	./src/LungApp.cxx
	./src/lavtkViewImage2D.cxx
	./src/lavtkInteractorStyleImageView2D.cxx
	./src/lavtkKeyEvent.cxx
	./src/lavtkImageViewCornerAnnotation.cxx
	./src/lavtkViewImage2DCallback.cxx
	./src/vtkImageAlgorithmFilter.cxx
)

set( ${LOCAL_PROJECT_NAME}_HDRS 
	./Include/lavtkViewImage2D.h
)

set( ${LOCAL_PROJECT_NAME}_MOC_HDRS 
	./Include/LungApp.h
	./Include/SliceOrientation.h
	./Include/lavtkImageViewCornerAnnotation.h
	./Include/SliceChangedCommand.h
	./Include/lavtkViewImage2DCallback.h
	./Include/lavtkInteractorStyleImageView2D.h
	./Include/lavtkKeyEvent.h
	./Include/vtkImageAlgorithmFilter.h
)

add_definitions(-DPREV_VTKRENDERINGADDON_COMPATIBILITY -DUSING_VTKINRIA3D)

# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

SOURCE_GROUP("Generated" FILES
	  ${${LOCAL_PROJECT_NAME}_RC_SRCS}
	  ${${LOCAL_PROJECT_NAME}_MOC_SRCS}
	  ${${LOCAL_PROJECT_NAME}_UI_HDRS}
)

SOURCE_GROUP("Resources" FILES
	  ${${LOCAL_PROJECT_NAME}_UIS}
	  ${${LOCAL_PROJECT_NAME}_RCS}
)

add_executable(${LOCAL_PROJECT_NAME} ${${LOCAL_PROJECT_NAME}_SRCS} 
    ${${LOCAL_PROJECT_NAME}_MOC_SRCS} 
	${${LOCAL_PROJECT_NAME}_HDRS}
	${${LOCAL_PROJECT_NAME}_MOC_HDRS}
)

target_link_libraries(${LOCAL_PROJECT_NAME} 
  PRIVATE
    ${VTK_LIBRARIES}
	${TARGET_NAME}
)

target_include_directories(${LOCAL_PROJECT_NAME} PUBLIC PRIVATE Include)
set_target_properties(${LOCAL_PROJECT_NAME} PROPERTIES FOLDER Development)