myproject(SynchronizedViews_0)

SET(${LOCAL_PROJECT_NAME}_SRC
	SynchronizedViews.cxx
	smvtkInteractorStyleImageView2D.cxx
	smvtkInteractorEvent.cxx
	smvtkInteractorEventWL.cxx
	smvtkInteractorEventSliceMove.cxx
	smvtkInteractorStyle2D.cxx
)

SET(${LOCAL_PROJECT_NAME}_HDR
	smvtkInteractorStyleImageView2D.h
	smvtkInteractorStyle2D.h
)

SET(${LOCAL_PROJECT_NAME}_MOC_HDR
	smvtkInteractorEvent.h
	smvtkInteractorEventWL.h
	smvtkInteractorEventSliceMove.h
)

QT45_WRAP_CPP(${LOCAL_PROJECT_NAME}_MOC_SRCS ${${LOCAL_PROJECT_NAME}_MOC_HDR} )
 
SOURCE_GROUP("Generated" FILES
	${${LOCAL_PROJECT_NAME}_MOC_SRCS}
)

ADD_EXECUTABLE(${LOCAL_PROJECT_NAME} 
	${${LOCAL_PROJECT_NAME}_SRC}
	${${LOCAL_PROJECT_NAME}_HDR}
	${${LOCAL_PROJECT_NAME}_MOC_HDR}
	${${LOCAL_PROJECT_NAME}_MOC_SRCS}
)

TARGET_LINK_LIBRARIES(${LOCAL_PROJECT_NAME} 
  ${VTK_LIBRARIES}
  ${QT_LIBRARIES}
  ${TARGET_NAME}
)

set_target_properties(${LOCAL_PROJECT_NAME} PROPERTIES FOLDER Development)

# In Qt6 we need to run windeployqt to setup the platform plugin and other plugins since this 
# doesn't happen automatically yet. With Qt5 this is not needed so just run for Qt6.
if (QT_VERSION_MAJOR EQUAL 6)
	if (MSVC) 
		add_custom_command(TARGET ${LOCAL_PROJECT_NAME} POST_BUILD
			COMMAND ${QT}::windeployqt --no-compiler-runtime \"$<TARGET_FILE:${LOCAL_PROJECT_NAME}>\"
			COMMENT "Running windeployqt for $<CONFIG>"
		)
	endif()
endif()