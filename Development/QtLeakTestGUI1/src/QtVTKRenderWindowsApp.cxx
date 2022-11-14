#include <QApplication>
#include <QSurfaceFormat>
#include <vtkVersion.h>

#if VTK_MAJOR_VERSION >= 9 
#include "QVTKOpenGLWidget.h"
#else 
//#include "QVTKOpenGLWidget.h"
#endif 

#include "QtVTKRenderWindows.h"

#include <vtkDebugLeaks.h>
#include "myVtkQtDebugLeaksView.h"

/////////////////////////////////////////////////////////////////////////////////////////

constexpr int LOOP_COUNT = 1;

/////////////////////////////////////////////////////////////////////////////////////////

int main( int argc, char** argv )
{
#if VTK_MAJOR_VERSION >= 9 
  // needed to ensure appropriate OpenGL context is created for VTK rendering.
  QSurfaceFormat::setDefaultFormat(QVTKOpenGLWidget::defaultFormat());
#else
//	QSurfaceFormat::setDefaultFormat(QVTKOpenGLWidget::defaultFormat();
#endif

  // QT Stuff
  QApplication app( argc, argv );

  myVtkQtDebugLeaksView view;

  bool retVal;
  
  {

	  for (int i = 0; i < LOOP_COUNT; ++i) {
		  QtVTKRenderWindows* pWidget = new QtVTKRenderWindows(argc, argv);
		  pWidget->setAttribute(Qt::WA_DeleteOnClose);
		  pWidget->show();
	  }

	  view.setAttribute(Qt::WA_QuitOnClose, true);
	  view.show();

	  retVal = app.exec();
  }

  return retVal;
}

/////////////////////////////////////////////////////////////////////////////////////////
