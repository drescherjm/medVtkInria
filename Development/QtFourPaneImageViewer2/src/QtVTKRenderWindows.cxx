#include "ui_QtVTKRenderWindows.h"
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
#include "vtkResliceImageViewer.h"
#include "vtkResliceCursorLineRepresentation.h"
#include "vtkResliceCursorThickLineRepresentation.h"
#include "vtkResliceCursorWidget.h"
#include "vtkResliceCursorActor.h"
#include "vtkResliceCursorPolyDataAlgorithm.h"
#include "vtkResliceCursor.h"
#include "vtkResliceImageViewerMeasurements.h"
#include <vtkImageView2D.h>
#include <vtkImageView2DExtended.h>
#include <vtkPointHandleRepresentation2D.h>
#include <vtkSeedRepresentation.h>
#include <vtkSeedWidget.h>
#include <vtkProperty2D.h>

//#define TEST_SEED_WIDGET

/////////////////////////////////////////////////////////////////////////////////////////

QtVTKRenderWindows::QtVTKRenderWindows(int vtkNotUsed(argc), char* argv[])
{
	this->ui = new Ui_QtVTKRenderWindows;
	this->ui->setupUi(this);

	vtkSmartPointer< vtkDICOMImageReader > reader = vtkSmartPointer< vtkDICOMImageReader >::New();
	reader->SetDirectoryName(argv[1]);
	reader->Update();
	int imageDims[3];
	reader->GetOutput()->GetDimensions(imageDims);

	for (int i = 0; i < 3; i++)
	{
		riw[i] = vtkSmartPointer< vtkImageView2DExtended >::New();
		vtkRenderWindow* renderWindow = vtkGenericOpenGLRenderWindow::New();

		vtkRenderer* pren = vtkRenderer::New();
		renderWindow->AddRenderer(pren);
		riw[i]->SetRenderWindow(renderWindow);
		riw[i]->SetRenderer(pren);

		riw[i]->ShowRulerWidgetOff();
		riw[i]->ShowScalarBarOff();
		riw[i]->SetAnnotationStyle(1);
	}

	this->ui->view1->SetRenderWindow(riw[0]->GetRenderWindow());
	riw[0]->SetupInteractor(this->ui->view1->GetRenderWindow()->GetInteractor());

	this->ui->view2->SetRenderWindow(riw[1]->GetRenderWindow());
	riw[1]->SetupInteractor(this->ui->view2->GetRenderWindow()->GetInteractor());

	this->ui->view3->SetRenderWindow(riw[2]->GetRenderWindow());
	riw[2]->SetupInteractor(this->ui->view3->GetRenderWindow()->GetInteractor());

	for (int i = 0; i < 3; i++)
	{
		riw[i]->SetInput(reader->GetOutputPort());
		riw[i]->SetSliceOrientation(i); // enum { SLICE_ORIENTATION_YZ = 0, SLICE_ORIENTATION_XZ = 1, SLICE_ORIENTATION_XY = 2 

		switch (i) {
		case vtkImageView2D::SLICE_ORIENTATION_XY:
			riw[i]->SetViewConvention(vtkImageView2D::VIEW_CONVENTION_LUNG_HFS_AXIAL_VIEW_AXIAL);
			break;
		case vtkImageView2D::SLICE_ORIENTATION_XZ:
			riw[i]->SetViewConvention(vtkImageView2D::VIEW_CONVENTION_LUNG_HFS_AXIAL_VIEW_CORONAL);
			break;
		case vtkImageView2D::SLICE_ORIENTATION_YZ:
			riw[i]->SetViewConvention(vtkImageView2D::VIEW_CONVENTION_LUNG_HFS_AXIAL_VIEW_SAGITTAL);
			break;
		}

		riw[i]->SetColorLevel(-600.0);
		riw[i]->SetColorWindow(1500.0);

	}

	vtkSmartPointer<vtkCellPicker> picker = vtkSmartPointer<vtkCellPicker>::New();
	picker->SetTolerance(0.005);

	vtkSmartPointer<vtkProperty> ipwProp = vtkSmartPointer<vtkProperty>::New();

	vtkSmartPointer< vtkRenderer > ren = vtkSmartPointer< vtkRenderer >::New();

	vtkRenderWindow* renderWindow = vtkGenericOpenGLRenderWindow::New();

	this->ui->view4->SetRenderWindow(renderWindow);
	this->ui->view4->GetRenderWindow()->AddRenderer(ren);
	vtkRenderWindowInteractor* iren = this->ui->view4->GetInteractor();

	for (int i = 0; i < 3; i++)
	{
		planeWidget[i] = vtkSmartPointer<vtkImagePlaneWidget>::New();
		planeWidget[i]->SetInteractor(iren);
		planeWidget[i]->SetPicker(picker);
		planeWidget[i]->RestrictPlaneToVolumeOn();
		double color[3] = { 0, 0, 0 };
		color[i] = 1;
		planeWidget[i]->GetPlaneProperty()->SetColor(color);

		color[0] /= 4.0;
		color[1] /= 4.0;
		color[2] /= 4.0;
		// This sets the background color on the 3 slice views to match the color
		// set on the 4th 3D Image plane widget.
		riw[i]->GetRenderer()->SetBackground(color);

		planeWidget[i]->SetTexturePlaneProperty(ipwProp);
		planeWidget[i]->TextureInterpolateOff();
		planeWidget[i]->SetResliceInterpolateToLinear();
		planeWidget[i]->SetInputData(reader->GetOutput());
		planeWidget[i]->SetPlaneOrientation(i);
		planeWidget[i]->SetSliceIndex(imageDims[i] / 2);
		planeWidget[i]->DisplayTextOn();
		planeWidget[i]->SetDefaultRenderer(ren);
		planeWidget[i]->SetWindowLevel(1358, -27);
		planeWidget[i]->On();
		planeWidget[i]->InteractionOn();
	}

	vtkSmartPointer<vtkResliceCursorCallback> cbk = vtkSmartPointer<vtkResliceCursorCallback>::New();

	//   for (int i = 0; i < 3; i++)
	//   {
	//     cbk->IPW[i] = planeWidget[i];
	//     cbk->RCW[i] = riw[i]->GetResliceCursorWidget();
	//     riw[i]->GetResliceCursorWidget()->AddObserver(
	//         vtkResliceCursorWidget::ResliceAxesChangedEvent, cbk );
	//     riw[i]->GetResliceCursorWidget()->AddObserver(
	//         vtkResliceCursorWidget::WindowLevelEvent, cbk );
	//     riw[i]->GetResliceCursorWidget()->AddObserver(
	//         vtkResliceCursorWidget::ResliceThicknessChangedEvent, cbk );
	//     riw[i]->GetResliceCursorWidget()->AddObserver(
	//         vtkResliceCursorWidget::ResetCursorEvent, cbk );
	//     riw[i]->GetInteractorStyle()->AddObserver(
	//         vtkCommand::WindowLevelEvent, cbk );
	// 
	//     // Make them all share the same color map.
	//     riw[i]->SetLookupTable(riw[0]->GetLookupTable());
	//     planeWidget[i]->GetColorMap()->SetLookupTable(riw[0]->GetLookupTable());
	//     //planeWidget[i]->GetColorMap()->SetInput(riw[i]->GetResliceCursorWidget()->GetResliceCursorRepresentation()->GetColorMap()->GetInput());
	//     planeWidget[i]->SetColorMap(riw[i]->GetResliceCursorWidget()->GetResliceCursorRepresentation()->GetColorMap());
	// 
	//   }

	this->ui->view1->show();
	this->ui->view2->show();
	this->ui->view3->show();

#ifdef TEST_SEED_WIDGET
	// Create the representation for the seed widget and for its handles
	auto handleRep = vtkSmartPointer<vtkPointHandleRepresentation2D>::New();
	handleRep->GetProperty()->SetColor(1, 0, 0); // Make the handles red
	auto widgetRep = vtkSmartPointer<vtkSeedRepresentation>::New();
	widgetRep->SetHandleRepresentation(handleRep);

	// Create the seed widget
	auto seedWidget = vtkSmartPointer<vtkSeedWidget>::New();

	auto pInteractor = riw[0]->GetInteractor();
	if (pInteractor) {
		seedWidget->SetInteractor(pInteractor);
		seedWidget->SetRepresentation(widgetRep);
	}

	//seedWidget->On();

#endif

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

void QtVTKRenderWindows::slotExit()
{
  qApp->exit();
}

void QtVTKRenderWindows::resliceMode(int mode)
{
  this->ui->thickModeCheckBox->setEnabled(mode ? 1 : 0);
  this->ui->blendModeGroupBox->setEnabled(mode ? 1 : 0);

  for (int i = 0; i < 3; i++)
  {
    //riw[i]->SetResliceMode(mode ? 1 : 0);
    riw[i]->GetRenderer()->ResetCamera();
    riw[i]->Render();
  }
}

void QtVTKRenderWindows::thickMode(int mode)
{
  for (int i = 0; i < 3; i++)
  {
    //riw[i]->SetThickMode(mode ? 1 : 0);
    riw[i]->Render();
  }
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
  for (int i = 0; i < 3; i++)
  {
    riw[i]->Reset();
  }

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
  for (int i = 0; i < 3; i++)
  {
    riw[i]->Render();
  }
  this->ui->view3->GetRenderWindow()->Render();
}

void QtVTKRenderWindows::AddDistanceMeasurementToView1()
{
 // this->AddDistanceMeasurementToView(1);
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
