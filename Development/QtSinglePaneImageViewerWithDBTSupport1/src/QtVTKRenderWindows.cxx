#include "ui_QtVTKRenderWindow.h"
#include "QtVTKRenderWindows.h"
#include "vtkResliceCursorCallback.h"

#include "vtkBoundedPlanePointPlacer.h"
#include "vtkCellPicker.h"
#include "vtkCommand.h"
#include "vtkDICOMImageReader.h"
#include "vtkDistanceRepresentation.h"
#include "vtkDistanceRepresentation2D.h"
#include "vtkDistanceWidget.h"
#include <vtkGenericOpenGLRenderWindow.h>
#include "vtkHandleRepresentation.h"
#include "vtkImageData.h"
#include "vtkImageMapToWindowLevelColors.h"
#include "vtkImageSlabReslice.h"
#include "vtkInteractorStyleImage.h"
#include "vtkLookupTable.h"
#include "vtkPlane.h"
#include "vtkPlaneSource.h"
#include "vtkPointHandleRepresentation2D.h"
#include "vtkPointHandleRepresentation3D.h"
#include "vtkProperty.h"
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include "vtkRenderWindowInteractor.h"
//#include "vtkResliceImageViewer.h"
#include "vtkResliceCursorLineRepresentation.h"
#include "vtkResliceCursorThickLineRepresentation.h"
#include "vtkResliceCursorWidget.h"
#include "vtkResliceCursorActor.h"
#include "vtkResliceCursorPolyDataAlgorithm.h"
#include "vtkResliceCursor.h"
//#include "vtkResliceImageViewerMeasurements.h"
#include <vtkImageView2D.h>
#include <vtkImageView2DExtended.h>
#include <vtkPointHandleRepresentation2D.h>
#include <vtkSeedRepresentation.h>
#include <vtkSeedWidget.h>
#include <vtkProperty2D.h>

#include <vtkCamera.h>
#include <vtkNew.h>

#ifndef VTK_DICOM_MODULE
#include "dicom/vtkDICOMReader.h"
#else
#include <X:\x64.20\VC.142\Install\Libraries\VTK-7.1.1\include\vtk-7.1\vtkDICOMReader.h>
#endif

#include <vtkDebugLeaks.h>

//#define TEST_SEED_WIDGET

/////////////////////////////////////////////////////////////////////////////////////////

QtVTKRenderWindows::QtVTKRenderWindows(int vtkNotUsed(argc), char* argv[])
{
	this->ui = new Ui_QtVTKRenderWindow;
	this->ui->setupUi(this);

	ui->spinBoxCamera->setMaximum(VTKView::MAX_VIEW_CONVENTIONS - 1);

	vtkSmartPointer< vtkDICOMReader > reader = vtkSmartPointer< vtkDICOMReader >::New();
	reader->SetFileName(argv[1]);
	reader->SetMemoryRowOrderToFileNative();
	reader->Update();

	reader->Print(std::cout);

	int imageDims[3];
	reader->GetOutput()->GetDimensions(imageDims);

	riw = vtkSmartPointer< VTKView >::New();
	vtkSmartPointer<vtkRenderWindow> renderWindow = vtkSmartPointer<vtkGenericOpenGLRenderWindow>::New();

	riw->SetViewConvention(VTKView::VIEW_CONVENTION_LUNG_HFS_AXIAL_VIEW_AXIAL);

	vtkSmartPointer<vtkRenderer> pren = vtkSmartPointer<vtkRenderer>::New();

	renderWindow->AddRenderer(pren);
	riw->SetRenderWindow(renderWindow);
	riw->SetRenderer(pren);

	riw->ShowRulerWidgetOff();
	riw->ShowScalarBarOff();
	riw->SetAnnotationStyle(1);
		
#if VTK_MAJOR_VERSION >= 9
	this->ui->view->setRenderWindow(riw->GetRenderWindow());
	riw->SetupInteractor(this->ui->view->renderWindow()->GetInteractor());
#else
	this->ui->view->SetRenderWindow(riw->GetRenderWindow());
	riw->SetupInteractor(this->ui->view->GetRenderWindow()->GetInteractor());
#endif

	riw->SetInput(reader->GetOutputPort());

	riw->SetSliceOrientation(vtkImageView2DExtended::SLICE_ORIENTATION_XY); // enum { SLICE_ORIENTATION_YZ = 0, SLICE_ORIENTATION_XZ = 1, SLICE_ORIENTATION_XY = 2 }

	//riw->SetOrientationMatrix(reader->GetPatientMatrix());

	riw->setImageAlignment(VTKView::IA_Right | VTKView::IA_VCenter);

	riw->SetColorLevel(512.0);
	riw->SetColorWindow(512.0);

	this->ui->view->show();

	auto pCamera = riw->GetRenderer()->GetActiveCamera();

	if (pCamera) {
		pCamera->Print(std::cout);
	}

#ifdef TEST_SEED_WIDGET
	// Create the representation for the seed widget and for its handles
	vtkNew<vtkPointHandleRepresentation2D> handleRep;
	handleRep->GetProperty()->SetColor(1, 1, 1); // Make the handles red

	vtkNew< vtkSeedRepresentation> widgetRep;

#if VTK_MAJOR_VERSION >= 9
	widgetRep->SetHandleRepresentation(handleRep);
#else 
	widgetRep->SetHandleRepresentation(handleRep.Get());
#endif 

	// Create the seed widget
	vtkNew<vtkSeedWidget> seedWidget;

	auto pInteractor = riw->GetInteractor();
	if (pInteractor) {
		seedWidget->SetInteractor(pInteractor);
#if VTK_MAJOR_VERSION >= 9
		seedWidget->SetRepresentation(widgetRep);
#else	
		seedWidget->SetRepresentation(widgetRep.Get());
#endif 
	}

	seedWidget->On();

#endif

	renderWindow->Print(std::cout);
	//renderWindow->Delete(); // We don't need to free this if we use
	// vtkSmartPointer<vtkRenderWindow> renderWindow = vtkSmartPointer<vtkGenericOpenGLRenderWindow>::New(); 
	// instead of  vtkSmartPointer<vtkRenderWindow> renderWindow = vtkGenericOpenGLRenderWindow::New();

	pren->Print(std::cout);
	//pren->Delete();

	// Set up action signals and slots
	connect(this->ui->actionExit, SIGNAL(triggered()), this, SLOT(slotExit()));
	connect(this->ui->resliceModeCheckBox, SIGNAL(stateChanged(int)), this, SLOT(resliceMode(int)));
	connect(this->ui->thickModeCheckBox, SIGNAL(stateChanged(int)), this, SLOT(thickMode(int)));
	this->ui->thickModeCheckBox->setEnabled(0);

	connect(this->ui->radioButton_Max, SIGNAL(pressed()), this, SLOT(SetBlendModeToMaxIP()));
	connect(this->ui->radioButton_Min, SIGNAL(pressed()), this, SLOT(SetBlendModeToMinIP()));
	connect(this->ui->radioButton_Mean, SIGNAL(pressed()), this, SLOT(SetBlendModeToMeanIP()));
	this->ui->blendModeGroupBox->setEnabled(0);

	connect(this->ui->resetButton, SIGNAL(pressed()), this, SLOT(ResetViews()));
	connect(this->ui->AddDistance1Button, SIGNAL(pressed()), this, SLOT(AddDistanceMeasurementToView1()));
};


QtVTKRenderWindows::~QtVTKRenderWindows()
{
	std::cout << "Got Here!" << std::endl;
	

	riw->Print(std::cout);

	//riw->Delete();

//	vtkDebugLeaks::PrintCurrentLeaks();
}

void QtVTKRenderWindows::slotExit()
{
  //qApp->exit();

	//riw->SetInput(nullptr,nullptr,0);
}

void QtVTKRenderWindows::resliceMode(int mode)
{
  this->ui->thickModeCheckBox->setEnabled(mode ? 1 : 0);
  this->ui->blendModeGroupBox->setEnabled(mode ? 1 : 0);

//   for (int i = 0; i < 3; i++)
//   {
//     //riw[i]->SetResliceMode(mode ? 1 : 0);
//     riw[i]->GetRenderer()->ResetCamera();
//     riw[i]->Render();
//   }
}

void QtVTKRenderWindows::thickMode(int mode)
{
//   for (int i = 0; i < 3; i++)
//   {
//     //riw[i]->SetThickMode(mode ? 1 : 0);
//     riw[i]->Render();
//   }
}

void QtVTKRenderWindows::SetBlendMode(int m)
{
//   for (int i = 0; i < 3; i++)
//   {
//     vtkImageSlabReslice *thickSlabReslice = vtkImageSlabReslice::SafeDownCast(
//         vtkResliceCursorThickLineRepresentation::SafeDownCast(
//           riw[i]->GetResliceCursorWidget()->GetRepresentation())->GetReslice());
//     thickSlabReslice->SetBlendMode(m);
//     riw[i]->Render();
//   }
}

void QtVTKRenderWindows::SetBlendModeToMaxIP()
{
  this->SetBlendMode(VTK_IMAGE_SLAB_MAX);
}

void QtVTKRenderWindows::SetBlendModeToMinIP()
{
  this->SetBlendMode(VTK_IMAGE_SLAB_MIN);
}

void QtVTKRenderWindows::SetBlendModeToMeanIP()
{
  this->SetBlendMode(VTK_IMAGE_SLAB_MEAN);
}

void QtVTKRenderWindows::ResetViews()
{
  // Reset the reslice image views

  riw->Reset();

  // Also sync the Image plane widget on the 3D top right view with any
  // changes to the reslice cursor.
//   for (int i = 0; i < 3; i++)
//   {
//     vtkPlaneSource *ps = static_cast< vtkPlaneSource * >(
//         planeWidget[i]->GetPolyDataAlgorithm());
//     ps->SetNormal(riw[0]->GetResliceCursor()->GetPlane(i)->GetNormal());
//     ps->SetCenter(riw[0]->GetResliceCursor()->GetPlane(i)->GetOrigin());
// 
//     // If the reslice plane has modified, update it on the 3D widget
//     this->planeWidget[i]->UpdatePlacement();
//   }

  // Render in response to changes.
  this->Render();
}

void QtVTKRenderWindows::Render()
{
	riw->Render();

#if VTK_MAJOR_VERSION >= 9
	this->ui->view->renderWindow()->Render();
#else
	this->ui->view->GetRenderWindow()->Render();
#endif 

}

void QtVTKRenderWindows::AddDistanceMeasurementToView1()
{
 // this->AddDistanceMeasurementToView(1);
}


void QtVTKRenderWindows::on_spinBoxCamera_valueChanged(int nValue)
{
	std::cout << nValue << std::endl;

// 	if (nValue >= VTKView::MAX_VIEW_CONVENTIONS) {
// 		nValue = 0;
// 		spinBox
// 	}

	riw->SetViewConvention(nValue);
}

void QtVTKRenderWindows::on_pushButtonHorizontal_clicked(bool)
{
	riw->flipHorizontal();
}

void QtVTKRenderWindows::on_pushButtonHorizontalAndVertical_clicked(bool)
{
	riw->flipVerticalAndHorizontal();
}

void QtVTKRenderWindows::on_pushButtonVertical_clicked(bool)
{
	riw->flipVertical();
}

// void QtVTKRenderWindows::AddDistanceMeasurementToView(int i)
// {
//   // remove existing widgets.
//   if (this->DistanceWidget[i])
//   {
//     this->DistanceWidget[i]->SetEnabled(0);
//     this->DistanceWidget[i] = nullptr;
//   }
// 
//   // add new widget
//   this->DistanceWidget[i] = vtkSmartPointer< vtkDistanceWidget >::New();
//   this->DistanceWidget[i]->SetInteractor(
//     this->riw[i]->GetResliceCursorWidget()->GetInteractor());
// 
//   // Set a priority higher than our reslice cursor widget
//   this->DistanceWidget[i]->SetPriority(
//     this->riw[i]->GetResliceCursorWidget()->GetPriority() + 0.01);
// 
//   vtkSmartPointer< vtkPointHandleRepresentation2D > handleRep = vtkSmartPointer< vtkPointHandleRepresentation2D >::New();
//   vtkSmartPointer< vtkDistanceRepresentation2D > distanceRep =  vtkSmartPointer< vtkDistanceRepresentation2D >::New();
// 
//   distanceRep->SetHandleRepresentation(handleRep);
//   this->DistanceWidget[i]->SetRepresentation(distanceRep);
//   distanceRep->InstantiateHandleRepresentation();
//   distanceRep->GetPoint1Representation()->SetPointPlacer(riw[i]->GetPointPlacer());
//   distanceRep->GetPoint2Representation()->SetPointPlacer(riw[i]->GetPointPlacer());
// 
//   // Add the distance to the list of widgets whose visibility is managed based
//   // on the reslice plane by the ResliceImageViewerMeasurements class
//   this->riw[i]->GetMeasurements()->AddItem(this->DistanceWidget[i]);
// 
//   this->DistanceWidget[i]->CreateDefaultRepresentation();
//   this->DistanceWidget[i]->EnabledOn();
// }
