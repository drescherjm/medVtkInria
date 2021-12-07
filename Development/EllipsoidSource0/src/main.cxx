// LAGUIExample3B

/*
 *	This example shows how to use a mask image that is smaller than the original imag
 *  and has possible padding on the x, y and z planes. 
 */

#include <QApplication>
#include <QMainWindow>
#include "lavtkViewImage2D.h"
#include "vtkImageData.h"
#include "vtkRenderWindow.h"
#include "vtkLookupTable.h"
#include "sliceOrientation.h"
#include <vtkImageSliceMapper.h>
#include <vtkDICOMImageReader.h>
#include <vtkImageImport.h>
#include <vtkImageMapToColors.h>
#include <vtkImageActor.h>
#include <vtkImageAlgorithm.h>
#include <vtkSimpleImageToImageFilter.h>
#include <vtkSimpleImageFilterExample.h>
#include <vtkImageChangeInformation.h>
#include <vtkImageConstantPad.h>
#include <vtkAlgorithmOutput.h>
#include <vtkOutputWindow.h>
#include <vtkImageEllipsoidSource.h>

/////////////////////////////////////////////////////////////////////////////////////////

lavtkViewImage2D* InitializeView( vtkImageData* pImage, unsigned int orientation )
{
	lavtkViewImage2D* pView = lavtkViewImage2D::New();
	pView->SetImage(pImage);
	pView->SetOrientation(orientation);
	pView->Reset();

	vtkRenderWindow* pren = pView->GetRenderWindow();
	pren->Render();
	return pView;
}

lavtkViewImage2D* InitializeView(vtkAlgorithmOutput* pImage, unsigned int orientation)
{
	lavtkViewImage2D* pView = lavtkViewImage2D::New();
	pView->SetInput(pImage);
	pView->SetOrientation(orientation);
	pView->Reset();

	vtkRenderWindow* pren = pView->GetRenderWindow();
	pren->Render();
	return pView;
}

/////////////////////////////////////////////////////////////////////////////////////////

vtkAlgorithmOutput* getImageAsAlgorithm(vtkImageData* pImage)
{
	auto pSM = vtkImageChangeInformation::New();
	pSM->SetInformation(pImage->GetInformation());
	pSM->SetInputDataObject(pImage);

 	pSM->Update();

	return pSM->GetOutputPort();
}

/////////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char *argv[])
{

	QApplication app(argc, argv);

	QMainWindow mainWindow;
	mainWindow.setGeometry(100, 100, 700, 700);

	const int nSize = 256;

	vtkImageEllipsoidSource* pImageSource = vtkImageEllipsoidSource::New();
	pImageSource->SetWholeExtent(0, nSize - 1, 0, nSize - 1, 0, nSize - 1);
	pImageSource->SetCenter(nSize >> 1, nSize >> 1, nSize >> 1);
	pImageSource->SetRadius(nSize >> 1, nSize >> 2, nSize >> 2);
	pImageSource->Update();

	auto pImageAlgorithm = pImageSource->GetOutputPort();
	
	lavtkViewImage2D* pViews[3];

	pViews[0] = InitializeView(pImageAlgorithm,laSliceOrientation::Sagittal);
	pViews[1] = InitializeView(pImageAlgorithm,laSliceOrientation::Coronal);
	pViews[1]->AddSiblingView(pViews[0]);
	pViews[2] = InitializeView(pImageAlgorithm,laSliceOrientation::Axial);
	pViews[2]->AddSiblingView(pViews[1]);
	pViews[0]->AddSiblingView(pViews[2]);


	for (auto pView : pViews) {
		pView->SetSize(425, 425);
		pView->SetVisibility(vtkImageView2DExtended::CROSSHAIR_ACTOR, 1);
	}

	pViews[0]->SetPosition(0, 0);
	pViews[1]->SetPosition(0, 450);
	pViews[2]->SetPosition(425, 0); pViews[2]->SetSize(900, 875);

	vtkObject::SetGlobalWarningDisplay(1);

	vtkSmartPointer<vtkOutputWindow> myOutputWindow = vtkSmartPointer<vtkOutputWindow>::New();

#if VTK_MAJOR_VERSION >=9
	myOutputWindow->SetUseStdErrorForAllMessages(true);
#endif

	vtkOutputWindow::SetInstance(myOutputWindow);
	
	return app.exec();

}
