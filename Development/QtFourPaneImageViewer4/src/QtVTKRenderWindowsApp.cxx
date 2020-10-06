#include <QApplication>
#include <QSurfaceFormat>
#include <QMessageBox>
#include <QObject>

#include "QVTKOpenGLWidget.h"
#include "QtVTKRenderWindows.h"
#include "vtkSystemInformation.h"
#include "smvtkMarkerShape.h"

/////////////////////////////////////////////////////////////////////////////////////////

bool checkRenderingCapabilities()
{
	vtkNew<vtkSystemInformation> systemInfo;
	systemInfo->RunRenderingCheck();

	if (systemInfo->GetRenderingCapabilities() & vtkSystemInformation::OPENGL)
	{
		if (systemInfo->usesMesa()) {
			smvtkMarkerShape::g_bUsePolygonsForThickLines = true;
		}

		return true;
	}

	qWarning("Graphics capability of this computer is not sufficient to run this application");

	QString message = QObject::tr("Graphics capability of this computer is not sufficient to "
		"run this application.");

	QString details = systemInfo->GetRenderingCapabilitiesDetails().c_str();

	QMessageBox* messageBox = new QMessageBox(0);
	messageBox->setAttribute(Qt::WA_DeleteOnClose, true);
	messageBox->setIcon(QMessageBox::Warning);
	messageBox->setWindowTitle(QObject::tr("Insufficient graphics capability"));
	messageBox->setText(message);
	messageBox->setDetailedText(details);
#if defined(_WIN32)
	// Older versions of Windows Remote Desktop protocol (RDP) makes the system report lower
	// OpenGL capability than the actual capability is (when the system is used locally).
	// On these systems, Slicer cannot be started while an RDP connection is active,
	// but an already started Slicer can be operated without problems.
	// Retry option allows delayed restart of Slicer through remote connection.
	// There is no need to offer "retry" option on other operating systems.
	messageBox->setStandardButtons(QMessageBox::Close | QMessageBox::Ignore | QMessageBox::Retry);
#else
	messageBox->setStandardButtons(QMessageBox::Close | QMessageBox::Ignore);
#endif
	messageBox->setDefaultButton(QMessageBox::Close);
	int result = messageBox->exec();

	// 	if (result == QMessageBox::Retry)
	// 	{
	// 		QDialog* messagePopup = new QDialog();
	// 		QVBoxLayout* layout = new QVBoxLayout();
	// 		messagePopup->setLayout(layout);
	// 		double restartDelaySec = 5.0;
	// 		QLabel* label = new QLabel(tr("Application will restart in %1 seconds. "
	// 			"If you are trying to connect through remote desktop, disconnect now "
	// 			"and reconnect in %1 seconds.").arg(int(restartDelaySec)), messagePopup);
	// 		layout->addWidget(label);
	// 		QTimer::singleShot(restartDelaySec * 1000, messagePopup, SLOT(close()));
	// 		messagePopup->exec();
	// 
	// 		restart();
	// 	}

	return (result == QMessageBox::Ignore);
}


/////////////////////////////////////////////////////////////////////////////////////////
// This example aims to pull in the advanced seed widget from StudyManager to test
// its functionality especially its use when the software renderer is enabled.


int main( int argc, char** argv )
{
	if (argc > 1) {

		// needed to ensure appropriate OpenGL context is created for VTK rendering.
		QSurfaceFormat::setDefaultFormat(QVTKOpenGLWidget::defaultFormat());

		// QT Stuff
		QApplication app(argc, argv);

		if (checkRenderingCapabilities()) {

			QtVTKRenderWindows myQtVTKRenderWindows(argc, argv);
			myQtVTKRenderWindows.show();

			return app.exec();
		}
		else {
			
		}
	}
	else {
		QApplication app(argc, argv);

		QMessageBox::warning(nullptr, "The application will close.", 
			"This program requires 1 command line argument which is the path of a folder containing a DICOM series.");

	}

	return -1;
}
