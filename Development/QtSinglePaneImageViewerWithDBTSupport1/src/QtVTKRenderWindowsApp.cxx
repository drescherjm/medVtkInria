#include <QApplication>
#include <QSurfaceFormat>

#include "QVTKOpenGLWidget.h"
#include "QtVTKRenderWindows.h"

#include <vtkDebugLeaks.h>

int main( int argc, char** argv )
{
  // needed to ensure appropriate OpenGL context is created for VTK rendering.
  QSurfaceFormat::setDefaultFormat(QVTKOpenGLWidget::defaultFormat());

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
