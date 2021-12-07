#include <QApplication>
#include <QSurfaceFormat>

#if VTK_MAJOR_VERSION >= 9 
#include "QVTKOpenGLWidget.h"
#else 
#include "QVTKWidget.h"
#endif 

#include "QtVTKRenderWindows.h"

int main( int argc, char** argv )
{
#if VTK_MAJOR_VERSION >= 9 
  // needed to ensure appropriate OpenGL context is created for VTK rendering.
  QSurfaceFormat::setDefaultFormat(QVTKOpenGLWidget::defaultFormat());
#endif

  // QT Stuff
  QApplication app( argc, argv );

  QtVTKRenderWindows myQtVTKRenderWindows(argc, argv);
  myQtVTKRenderWindows.show();

  return app.exec();
}
