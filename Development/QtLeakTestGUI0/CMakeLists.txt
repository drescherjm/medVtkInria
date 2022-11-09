
option(${PROJECT_NAME}_BUILD_DEVELOPMENT_QtLeakTestGUI0  	"Build QtLeakTestGUI0"   ON )

if (${PROJECT_NAME}_BUILD_DEVELOPMENT_QtLeakTestGUI0)

myproject(QtLeakTestGUI0)

find_package( Boost REQUIRED )

if (VTK_VERSION VERSION_LESS "8.90.0")
find_package(VTK
  COMPONENTS
    vtkCommonCore
    vtkGUISupportQt
    vtkIOImage
    vtkImagingColor
    vtkImagingGeneral
    vtkInteractionImage
    vtkInteractionStyle
    vtkInteractionWidgets
	vtkDICOM)
	
	include(${VTK_USE_FILE})
		
else()
find_package(VTK
  COMPONENTS
    CommonCore
    GUISupportQt
    IOImage
    ImagingColor
    ImagingGeneral
    InteractionImage
    InteractionStyle
    InteractionWidgets)
endif()

if (NOT VTK_FOUND)
  message("Skipping example: ${VTK_NOT_FOUND_MESSAGE}")
  return ()
endif ()

find_package(Qt5 COMPONENTS Widgets)
if (NOT TARGET Qt5::Widgets)
  message("Skipping example: ${Qt5_NOT_FOUND_MESSAGE}")
  return ()
endif ()

# Set your files and resources here
set(Srcs
  src/QtVTKRenderWindowsApp.cxx
  src/QtVTKRenderWindows.cxx
  src/vtkResliceCursorCallback.cxx
  src/smvtkImageView2D.cxx
  src/smvtkImageView2DCommand.cxx
)

set(Hdrs
  Include/QtVTKRenderWindows.h
  Include/vtkResliceCursorCallback.h
  Include/smvtkImageView2D.h
  Include/VTKQtWidget.h
  Include/smvtkImageView2DCommand.h
  Include/DicomReader.h
  Include/MammographyViewOrientatationHelper.h
)

set(UIs
  rc/QtVTKRenderWindow_vtk7.ui
  rc/QtVTKRenderWindow.ui
  )

set(CMAKE_AUTOUIC_SEARCH_PATHS rc)

# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

# CMAKE_AUTOMOC in ON so the MocHdrs will be automatically wrapped.
add_executable(${LOCAL_PROJECT_NAME} 
  ${Srcs} ${Hdrs} ${UIs})

target_link_libraries(${LOCAL_PROJECT_NAME} 
  PRIVATE
    ${VTK_LIBRARIES}
    Qt5::Widgets 
	${TARGET_NAME})
	
add_custom_command(TARGET ${LOCAL_PROJECT_NAME} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_RUNTIME_DLLS:${LOCAL_PROJECT_NAME}> $<TARGET_FILE_DIR:${LOCAL_PROJECT_NAME}>
  COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:Qt5::QWindowsIntegrationPlugin> $<TARGET_FILE_DIR:${LOCAL_PROJECT_NAME}>/platforms
  COMMAND_EXPAND_LISTS
  )

target_include_directories(${LOCAL_PROJECT_NAME} PRIVATE Include)
set_target_properties(${LOCAL_PROJECT_NAME} PROPERTIES FOLDER Development)

endif()
