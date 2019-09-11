myproject(SynchronizedViews_0)

SET(${PROJECT_NAME}_SRC
	SynchronizedViews.cxx
	smvtkInteractorStyleImageView2D.cxx
	smvtkInteractorEvent.cxx
	smvtkInteractorEventWL.cxx
	smvtkInteractorEventSliceMove.cxx
	smvtkInteractorStyle2D.cxx
)

SET(${PROJECT_NAME}_HDR
	smvtkInteractorStyleImageView2D.h
	smvtkInteractorStyle2D.h
)

SET(${PROJECT_NAME}_MOC_HDR
	smvtkInteractorEvent.h
	smvtkInteractorEventWL.h
	smvtkInteractorEventSliceMove.h
)

QT45_WRAP_CPP(${PROJECT_NAME}_MOC_SRCS ${${PROJECT_NAME}_MOC_HDR} )
 
SOURCE_GROUP("Generated" FILES
	${${PROJECT_NAME}_MOC_SRCS}
)

ADD_EXECUTABLE(${PROJECT_NAME} 
	${${PROJECT_NAME}_SRC}
	${${PROJECT_NAME}_HDR}
	${${PROJECT_NAME}_MOC_HDR}
	${${PROJECT_NAME}_MOC_SRCS}
)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} 
  vtkRenderingAddOn
  vtkWidgetsAddOn
  vtkRendering 
  vtkImaging
  vtkHybrid
  vtkWidgets
  vtkVolumeRendering
  ${QT_LIBRARIES}
)



