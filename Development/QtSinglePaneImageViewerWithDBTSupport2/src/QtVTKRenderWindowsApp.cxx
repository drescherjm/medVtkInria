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

  bool retVal;
  
  {
	  QtVTKRenderWindows myQtVTKRenderWindows(argc, argv);
	  myQtVTKRenderWindows.show();

	  retVal = app.exec();
  }

  vtkDebugLeaks::PrintCurrentLeaks();

  return retVal;
}
