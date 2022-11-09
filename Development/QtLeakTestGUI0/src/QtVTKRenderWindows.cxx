#include "ui_QtVTKRenderWindow.h"
#include "QtVTKRenderWindows.h"
#include "vtkResliceCursorCallback.h"
#include "imageHelpers.h"

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
#include <vtkMatrix4x4.h>

#include <vtkImageFlip.h>

#include <vtkDebugLeaks.h>
#include "QtVTKRenderWindows.h"
#include "FunctionProfiler.h"
#include <vtkImageData.h>

#include <QTimer>

/////////////////////////////////////////////////////////////////////////////////////////

static const vtkSmartPointer<vtkMatrix4x4> vtkIdentityMatrix4x4{ vtkSmartPointer<vtkMatrix4x4>::New()};

//#define TEST_SEED_WIDGET

//#define FLIPZ_USING_FILTER

//#define DEBUG_CAMERA

//#define DEBUG_RENDERER
//#define DEBUG_RENDER_WINDOW

/////////////////////////////////////////////////////////////////////////////////////////

int GetCodeForDBT_RMLO_RCC(vtkMatrix4x4* pPatientMatrix);

/////////////////////////////////////////////////////////////////////////////////////////

QtVTKRenderWindows::QtVTKRenderWindows(int argc, char* argv[])
{
	this->ui = new Ui_QtVTKRenderWindow;
	this->ui->setupUi(this);

	ui->spinBoxCamera->setMaximum(VTKView::MAX_VIEW_CONVENTIONS - 1);

	setupImage();

};

/////////////////////////////////////////////////////////////////////////////////////////

void QtVTKRenderWindows::setupImage()
{
	PROFILE_THIS_FUNCTION;

	m_pImage = createTestImage(640, 640, 64);

	vtkNew<vtkImageReslice> reslice;

	reslice->SetInputData(m_pImage);
	reslice->Update();

	std::string strLaterality{ "R" };

	riw = vtkSmartPointer< VTKView >::New();
	vtkSmartPointer<vtkRenderWindow> renderWindow = vtkSmartPointer<vtkGenericOpenGLRenderWindow>::New();

	auto nConv = GetCodeForDBT_RMLO_RCC(nullptr);
	riw->SetViewConvention(nConv);

	ui->spinBoxCamera->blockSignals(true);
	ui->spinBoxCamera->setValue(nConv);
	ui->spinBoxCamera->blockSignals(false);

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

#ifdef	FLIPZ_USING_FILTER
	vtkNew<vtkImageFlip> flipZFilter;
	flipZFilter->SetFilteredAxis(2); // flip z axis
	flipZFilter->SetInputConnection(m_pReader->GetOutputPort());
	flipZFilter->Update();

	double cosines[9]{};
	flipZFilter->GetResliceAxesDirectionCosines(cosines);

	riw->SetInput(flipZFilter->GetOutputPort());
#else
	riw->SetInput(reslice->GetOutputPort());
#endif //def FLIPZ_USING_FILTER

	riw->SetSliceOrientation(vtkImageView2DExtended::SLICE_ORIENTATION_XY); // enum { SLICE_ORIENTATION_YZ = 0, SLICE_ORIENTATION_XZ = 1, SLICE_ORIENTATION_XY = 2 }


	smvtkImageView2D::ImageAlignmentFlags align = VTKView::IA_VCenter;

	if (strLaterality == "L") {
		align |= VTKView::IA_Left;
	}
	else {
		align |= VTKView::IA_Right;
	}

	riw->setImageAlignment(align);


	riw->SetColorLevel(256.0);
	riw->SetColorWindow(512.0);

	// 		auto wl = m_pReader->getDefaultWindowLevel();
	// 		if (!wl) {
	// 			riw->SetColorLevel(512.0);
	// 			riw->SetColorWindow(512.0);
	// 		}
	// 		else {
	// 			riw->SetColorLevel(wl->second);
	// 			riw->SetColorWindow(wl->first);
	// 		}

	this->ui->view->show();

#ifdef DEBUG_CAMERA
	auto pCamera = riw->GetRenderer()->GetActiveCamera();

	if (pCamera) {
		pCamera->Print(std::cout);
	}
#endif //def DEBUG_CAMERA

	//riw->SetSize(imageDims[0], imageDims[1]);

	if (!align == static_cast<smvtkImageView2D::ImageAlignmentFlags>(smvtkImageView2D::IA_HCenter | smvtkImageView2D::IA_VCenter)) {
		riw->UpdateAlignment();
		riw->Render();
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

#ifdef DEBUG_RENDER_WINDOW
	renderWindow->Print(std::cout);
#endif //def DEBUG_RENDER_WINDOW

	//renderWindow->Delete(); // We don't need to free this if we use
	// vtkSmartPointer<vtkRenderWindow> renderWindow = vtkSmartPointer<vtkGenericOpenGLRenderWindow>::New(); 
	// instead of  vtkSmartPointer<vtkRenderWindow> renderWindow = vtkGenericOpenGLRenderWindow::New();

#ifdef DEBUG_RENDERER
	pren->Print(std::cout);
	//pren->Delete();
#endif //def DEBUG_RENDERER

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

	//QTimer::singleShot(100, [this]() {m_pReader->Update();
	riw->UpdateDisplayExtent();
	riw->Render();

}

/////////////////////////////////////////////////////////////////////////////////////////

int GetCodeForDBT_LMLO_LCC(vtkMatrix4x4* pPatientMatrix)
{
	if (pPatientMatrix) {
		if (pPatientMatrix->GetElement(1, 0) < 0) {
			return 3;
		}
	}
	return 4;
}

/////////////////////////////////////////////////////////////////////////////////////////

int GetCodeForDBT_RMLO_RCC(vtkMatrix4x4* pPatientMatrix)
{
	return 2;
}

/////////////////////////////////////////////////////////////////////////////////////////

int GetCodeForDBT_LLM(vtkMatrix4x4* pPatientMatrix)
{
	if (pPatientMatrix) {
		if (pPatientMatrix->GetElement(1, 0) > 0) {
			return 8;
		}
	}
	return 2;
}

/////////////////////////////////////////////////////////////////////////////////////////

int GetCodeForDBT_RLM(vtkMatrix4x4* pPatientMatrix)
{
	return 3;
}

/////////////////////////////////////////////////////////////////////////////////////////

int GetCodeForDBT_LXCCL(vtkMatrix4x4* pPatientMatrix)
{
	return 4;
}

/////////////////////////////////////////////////////////////////////////////////////////

int GetCodeForDBT_RXCCL(vtkMatrix4x4* pPatientMatrix)
{
	return 2;
}

/////////////////////////////////////////////////////////////////////////////////////////

QtVTKRenderWindows::~QtVTKRenderWindows()
{
	std::cout << __FUNCTION__ << std::endl;
	
#ifdef DEBUG_IMAGE_VIEWER
	riw->Print(std::cout);
#endif //def DEBUG_IMAGE_VIEWER

	//riw->Delete();

//	vtkDebugLeaks::PrintCurrentLeaks();
}

/////////////////////////////////////////////////////////////////////////////////////////

void QtVTKRenderWindows::slotExit()
{
  //qApp->exit();

	//riw->SetInput(nullptr,nullptr,0);
}

/////////////////////////////////////////////////////////////////////////////////////////

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

/////////////////////////////////////////////////////////////////////////////////////////

void QtVTKRenderWindows::thickMode(int mode)
{
//   for (int i = 0; i < 3; i++)
//   {
//     //riw[i]->SetThickMode(mode ? 1 : 0);
//     riw[i]->Render();
//   }
}

/////////////////////////////////////////////////////////////////////////////////////////

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

/////////////////////////////////////////////////////////////////////////////////////////

void QtVTKRenderWindows::SetBlendModeToMaxIP()
{
  this->SetBlendMode(VTK_IMAGE_SLAB_MAX);
}

/////////////////////////////////////////////////////////////////////////////////////////

void QtVTKRenderWindows::SetBlendModeToMinIP()
{
  this->SetBlendMode(VTK_IMAGE_SLAB_MIN);
}

/////////////////////////////////////////////////////////////////////////////////////////

void QtVTKRenderWindows::SetBlendModeToMeanIP()
{
  this->SetBlendMode(VTK_IMAGE_SLAB_MEAN);
}

/////////////////////////////////////////////////////////////////////////////////////////

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

/////////////////////////////////////////////////////////////////////////////////////////

void QtVTKRenderWindows::Render()
{
	riw->Render();

#if VTK_MAJOR_VERSION >= 9
	this->ui->view->renderWindow()->Render();
#else
	this->ui->view->GetRenderWindow()->Render();
#endif 

}

/////////////////////////////////////////////////////////////////////////////////////////

void QtVTKRenderWindows::AddDistanceMeasurementToView1()
{
 // this->AddDistanceMeasurementToView(1);
}

/////////////////////////////////////////////////////////////////////////////////////////

void QtVTKRenderWindows::showEvent(QShowEvent* event)
{
	QMainWindow::showEvent(event);

	riw->UpdateAlignment();
	riw->Render();

//	emit initConventionSpinBoxes();

	std::cout << "Render Time: " << riw->GetRenderer()->GetLastRenderTimeInSeconds() << std::endl;
}

void QtVTKRenderWindows::resizeEvent(QResizeEvent* event)
{
	QMainWindow::resizeEvent(event);

	//riw->UpdateDisplayExtent();
}

/////////////////////////////////////////////////////////////////////////////////////////

void QtVTKRenderWindows::on_spinBoxCamera_valueChanged(int nValue)
{
	std::cout << nValue << std::endl;

// 	if (nValue >= VTKView::MAX_VIEW_CONVENTIONS) {
// 		nValue = 0;
// 		spinBox
// 	}

	riw->SetViewConvention(nValue);
	riw->Render();

//	emit initConventionSpinBoxes();
}

/////////////////////////////////////////////////////////////////////////////////////////

void QtVTKRenderWindows::on_pushButtonHorizontal_clicked(bool)
{
	riw->flipHorizontal();
	riw->Render();

//	emit initConventionSpinBoxes();
}

/////////////////////////////////////////////////////////////////////////////////////////

void QtVTKRenderWindows::on_pushButtonHorizontalAndVertical_clicked(bool)
{
	riw->flipVerticalAndHorizontal();
	riw->Render();

//	emit initConventionSpinBoxes();
}

/////////////////////////////////////////////////////////////////////////////////////////

void QtVTKRenderWindows::on_pushButtonVertical_clicked(bool)
{
	riw->flipVertical();
	riw->Render();

//	emit initConventionSpinBoxes();
}

/////////////////////////////////////////////////////////////////////////////////////////

void QtVTKRenderWindows::on_pushButtonAlignLeft_clicked(bool)
{
	riw->setImageAlignment(VTKView::IA_Left | VTKView::IA_VCenter);
	riw->UpdateAlignment();
	riw->Render();
}

/////////////////////////////////////////////////////////////////////////////////////////

void QtVTKRenderWindows::on_pushButtonAlignRight_clicked(bool)
{
	riw->setImageAlignment(VTKView::IA_Right | VTKView::IA_VCenter);
	riw->UpdateAlignment();
	riw->Render();
}

/////////////////////////////////////////////////////////////////////////////////////////

void QtVTKRenderWindows::on_pushButtonComputeScale_clicked(bool)
{
	riw->getImageScale();
}

/////////////////////////////////////////////////////////////////////////////////////////
