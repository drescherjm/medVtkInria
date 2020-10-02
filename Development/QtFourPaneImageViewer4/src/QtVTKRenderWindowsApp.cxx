#include <QApplication>
#include <QSurfaceFormat>
#include <QMessageBox>

#include "QVTKOpenGLWidget.h"
#include "QtVTKRenderWindows.h"

/////////////////////////////////////////////////////////////////////////////////////////
// This example aims to pull in the advanced seed widget from StudyManager to test
// its functionality espcially its use when the software reneder is enabled.


int main( int argc, char** argv )
{
	if (argc > 1) {

		// needed to ensure appropriate OpenGL context is created for VTK rendering.
		QSurfaceFormat::setDefaultFormat(QVTKOpenGLWidget::defaultFormat());

		// QT Stuff
		QApplication app(argc, argv);

		QtVTKRenderWindows myQtVTKRenderWindows(argc, argv);
		myQtVTKRenderWindows.show();

		return app.exec();
	}
	else {
		QMessageBox::warning(nullptr, "The application will close.", 
			"This program requires 1 command line argument which is the path of a folder containing a DICOM series.");

	}

	return -1;
}
