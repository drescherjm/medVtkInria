#include "smvtkImageView2D.h"
#include "smvtkImageView2DCommand.h"

#include <vtkImageReslice.h>
#include <vtkCamera.h>
#include <vtkRenderer.h>
#include <vtkMatrix4x4.h>
#include <vtkRenderWindow.h>
#include <vtkAxes2DWidget.h>

#ifdef USE_MED_VTK_INRIA_EX
#include <vtkImage2DDisplay.h>
#endif

/////////////////////////////////////////////////////////////////////////////////////////

class smvtkImageView2D::smPrivate 
{
public:
	smPrivate();
public:
	QSize	m_szLast;
	smvtkImageView2D::ImageAlignmentFlags m_imageAlign;
	uint8_t	m_nXOffs;
	uint8_t	m_nYOffs;

#ifdef USE_MED_VTK_INRIA_EX
	medVtkImageInfo* getImageInfo(smvtkImageView2D* pPublic);
#endif

	vtkImageData*							m_pFGImage;
	vtkSmartPointer<vtkImageActor>			m_pFGImageActor;
	vtkSmartPointer<vtkImageMapToColors>	m_pFGWindowLevel;
	vtkSmartPointer<vtkImageReslice>        m_pFGImageReslice;
};

/////////////////////////////////////////////////////////////////////////////////////////

smvtkImageView2D::smPrivate::smPrivate() : m_pFGImage(nullptr)
{

}

/////////////////////////////////////////////////////////////////////////////////////////

#ifdef USE_MED_VTK_INRIA_EX
medVtkImageInfo* smvtkImageView2D::smPrivate::getImageInfo(smvtkImageView2D* pPublic)
{
	medVtkImageInfo* retVal{};
	if (pPublic) {
		auto pImage2DDisplay = pPublic->GetImage2DDisplayForLayer(smvtkImageView2D::IMAGE_ACTOR);
		if (pImage2DDisplay) {
			retVal = pImage2DDisplay->GetMedVtkImageInfo();
		}
	}
	return retVal;
}
#endif

/////////////////////////////////////////////////////////////////////////////////////////

#if VTK_MAJOR_VERSION < 6
vtkCxxRevisionMacro (smvtkImageView2D, "$Revision: 4675 $");
#endif
vtkStandardNewMacro (smvtkImageView2D);

/////////////////////////////////////////////////////////////////////////////////////////

smvtkImageView2D::smvtkImageView2D()
: Superclass()
{
	m_pPrivate = new smPrivate();
	SetViewConvention(VIEW_CONVENTION_RADIOLOGICAL_BREAST);
	setImageAlignmentOffset();
	setImageAlignment(IA_HCenter|IA_VCenter);
	SetAnnotationStyle(3);
	
	// Replace command handler from base with ours..
	this->Command->Delete();
	this->Command = smvtkImageView2DCommand::New();
	this->Command->SetViewer(this);

}

/////////////////////////////////////////////////////////////////////////////////////////

smvtkImageView2D::~smvtkImageView2D() 
{
	UnInstallPipeline();
	delete m_pPrivate;
}

/////////////////////////////////////////////////////////////////////////////////////////

bool smvtkImageView2D::GetSafeDisplayCoordinatesFromImageCoordinates( int indices[3], double* dispaypos )
{
	bool retVal;

	double worldPos[4];

	retVal = GetSafeWorldCoordinatesFromImageCoordinates(indices,worldPos);
	if (retVal) {
		vtkCamera *cam = this->Renderer ? this->Renderer->GetActiveCamera() : nullptr;
			
		retVal = (cam != nullptr);
		if (retVal) {
			
			worldPos[3] = 1.0; // 

			this->GetRenderer()->SetWorldPoint(worldPos);
			this->GetRenderer()->WorldToDisplay();
			
			double* pDisplay = this->GetRenderer()->GetDisplayPoint();
			dispaypos[0] = pDisplay[0];
			dispaypos[1] = pDisplay[1];
			dispaypos[2] = 0.0;
		}

	}
	return retVal;
}

/////////////////////////////////////////////////////////////////////////////////////////

bool smvtkImageView2D::GetSafeWorldCoordinatesFromImageCoordinates(int indices[3], double* position)
{

#ifndef USE_MED_VTK_INRIA_EX
	if (!this->GetInput())
	{ 
		return false;
	}

	// Get information
	double* spacing = this->GetInput()->GetSpacing();
	double* origin = this->GetInput()->GetOrigin();
#else
	medVtkImageInfo* pImageInfo = m_pPrivate->getImageInfo(this);
	if (!pImageInfo) {
		return false;
	}

	double* spacing = pImageInfo->spacing;
	int*	extent =  pImageInfo->extent;
	double* origin =  pImageInfo->origin;
	
#endif

	double orientedposition[4];
	for (unsigned int i=0; i<3; i++)
		orientedposition[i] = origin[i] + spacing[i]*indices[i];
	orientedposition[3] = 1;

	this->GetOrientationMatrix()->MultiplyPoint (orientedposition, orientedposition);
	for( unsigned int i=0; i<3; i++)
		position[i] = orientedposition[i];

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////

#ifndef USING_OLD_VTK_PIPELINE
void smvtkImageView2D::SetInputConnection(vtkAlgorithmOutput* pAlgorithmOutput)
{
	Superclass::SetInput(pAlgorithmOutput);
// 	if (pAlgorithmOutput) {
// 		this->ResetCurrentPoint();
// 	}
}
#endif

/////////////////////////////////////////////////////////////////////////////////////////

#ifdef USING_OLD_VTK_PIPELINE
void smvtkImageView2D::SetInput( vtkImageData *image )
{
	Superclass::SetInput(image);
	if (image != nullptr) {
		this->ResetCurrentPoint();
	}
}
#endif

/////////////////////////////////////////////////////////////////////////////////////////

void smvtkImageView2D::UpdateCenter( void )
{
#ifndef USE_MED_VTK_INRIA_EX
	if (!this->GetInput())
		return;
	int* dimensions = this->GetInput()->GetDimensions();
#else
	medVtkImageInfo* pImageInfo = m_pPrivate->getImageInfo(this);
	if (!pImageInfo) {
		return;
	}
	
	int	dimensions[3];
	pImageInfo->GetDimensions(dimensions);

#endif

	int indices[3] = {0,0,0};
	for (unsigned int i=0; i<3; i++)
	{
		indices[i] = (int)((double)dimensions[i] / 2.0);
	}
	
	//if (this->CurrentPoint[0] != 
	indices[this->SliceOrientation] = this->GetSlice();

	this->GetWorldCoordinatesFromImageCoordinates (indices, this->ViewCenter);
}

/////////////////////////////////////////////////////////////////////////////////////////

void smvtkImageView2D::UpdateSlicePlane( void )
{
	Superclass::UpdateSlicePlane();
}

/////////////////////////////////////////////////////////////////////////////////////////

void smvtkImageView2D::Reset( void )
{
	//resetCameraOnWindowSizeChange(true);
	
	Superclass::Reset();

	UpdateAlignment();
}

/////////////////////////////////////////////////////////////////////////////////////////

QSize smvtkImageView2D::getLastWindowSize()
{
	QSize retVal;
	if (m_pPrivate != nullptr) {
		retVal = m_pPrivate->m_szLast;
	}
	return retVal;
}

/////////////////////////////////////////////////////////////////////////////////////////

void smvtkImageView2D::resetCameraOnWindowSizeChange( bool bForce/*=false*/ )
{

	/**
	 * \note: This code is disabled because it was not being called when I expected it
	 * to be called. Instead the smImageView class will handle this.
	 */

	/*vtkRenderWindow* pRenderWindow = GetRenderWindow();
	if (pRenderWindow != nullptr) {
		int nWidth = pRenderWindow->GetSize()[0];
		int nHeight = pRenderWindow->GetSize()[1];
		if (nHeight > 0) {
			int nMin = std::min(nHeight,nWidth);

			SetZoom(nMin / (1.1 * nHeight) );

			if (pRenderWindow->is)
		}
	}*/
}

/////////////////////////////////////////////////////////////////////////////////////////

void smvtkImageView2D::initFirstView()
{
	//ShowImageAxisOn();
	UpdateOrientation();
	SetCameraFromOrientation();
	ResetCamera();
}

/////////////////////////////////////////////////////////////////////////////////////////

void smvtkImageView2D::flipVertical()
{
	switch(this->SliceOrientation) {
		case vtkImageView2D::SLICE_ORIENTATION_XY:
			this->ConventionMatrix->SetElement(1,2,this->ConventionMatrix->GetElement(1,2) * -1.0);
			this->ConventionMatrix->SetElement(2,3,this->ConventionMatrix->GetElement(2,3) * -1.0);
		break;
	}
	this->ConventionMatrix->Print(std::cout);
	this->UpdateOrientation();
}


/////////////////////////////////////////////////////////////////////////////////////////

void smvtkImageView2D::flipHorizontal()
{
	switch (this->SliceOrientation) {
	case vtkImageView2D::SLICE_ORIENTATION_XY:
		this->ConventionMatrix->SetElement(1, 2, this->ConventionMatrix->GetElement(1, 2) * 1.0);
		this->ConventionMatrix->SetElement(2, 3, this->ConventionMatrix->GetElement(2, 3) * -1.0);

		this->ConventionMatrix->SetElement(3, 0, -1.0);
		break;
	}
	this->ConventionMatrix->Print(std::cout);
	this->UpdateOrientation();
}

/////////////////////////////////////////////////////////////////////////////////////////

void smvtkImageView2D::flipVerticalAndHorizontal()
{
	switch(this->SliceOrientation) {
	case vtkImageView2D::SLICE_ORIENTATION_XY:
		this->ConventionMatrix->SetElement(1,2,this->ConventionMatrix->GetElement(1,2) * -1.0);
		this->ConventionMatrix->SetElement(2,3,this->ConventionMatrix->GetElement(2,3) * 1.0);

		this->ConventionMatrix->SetElement(3,0,-1.0);
		break;
	}
	this->ConventionMatrix->Print(std::cout);
	this->UpdateOrientation();
}

/////////////////////////////////////////////////////////////////////////////////////////

void smvtkImageView2D::SetAnnotationsFromOrientation( void )
{
	Superclass::SetAnnotationsFromOrientation();
}

/////////////////////////////////////////////////////////////////////////////////////////

bool smvtkImageView2D::disableAxesCrosshair( bool bDisable )
{
	if (bDisable) 
		Axes2DWidget->EnabledOff();
	else
		Axes2DWidget->EnabledOn();

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////

void smvtkImageView2D::ResetCamera( void )
{
	Superclass::ResetCamera();
}

/////////////////////////////////////////////////////////////////////////////////////////

void smvtkImageView2D::InstallPipeline( void )
{
	this->Superclass::InstallPipeline();
	if( this->InteractorStyle )
	{
		if ( !this->InteractorStyle->HasObserver(smvtkImageView2DCommand::WindowSizeChanged, this->Command)) {
			this->InteractorStyle->AddObserver (smvtkImageView2DCommand::WindowSizeChanged, this->Command, 10);
		}
		if ( !this->InteractorStyle->HasObserver (smvtkImageView2DCommand::AlignLeft, this->Command) )
			this->InteractorStyle->AddObserver (smvtkImageView2DCommand::AlignLeft, this->Command, 10);
		if ( !this->InteractorStyle->HasObserver (smvtkImageView2DCommand::AlignRight, this->Command) )
			this->InteractorStyle->AddObserver (smvtkImageView2DCommand::AlignRight, this->Command, 10);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////

void smvtkImageView2D::UnInstallPipeline( void )
{
	this->Superclass::UnInstallPipeline();
}

/////////////////////////////////////////////////////////////////////////////////////////

/**
 *	\brief 
 *	This member updates the camera shift needed to support image alignment.
 */

void smvtkImageView2D::UpdateAlignment()
{

#ifndef USE_MED_VTK_INRIA_EX
	// First reset the pan.
	int pan[2] = {0,0};
	SetPan(pan);

	if (!m_pPrivate->m_imageAlign == (IA_HCenter | IA_VCenter)) {
		vtkRenderer* pRenderer = GetRenderer();
		vtkImageActor* pImageActor = GetImageActor();

		double bounds[6];
		pImageActor->GetDisplayBounds(bounds);

		pRenderer->SetWorldPoint(bounds[0],bounds[2],bounds[4],0);
		pRenderer->WorldToDisplay();

		double displayBL[3];
		pRenderer->GetDisplayPoint(displayBL);

		// Calculate the horizontal pan
		if (m_pPrivate->m_imageAlign.testFlag(IA_Left)) {
			pan[0] = static_cast<int>(displayBL[0]) -m_pPrivate->m_nXOffs;
		}
		else
			if (m_pPrivate->m_imageAlign.testFlag(IA_Right)) {
				pan[0] = m_pPrivate->m_nXOffs - static_cast<int>(displayBL[0]);
			}
		
		// Calculate the vertical pan
		if (m_pPrivate->m_imageAlign.testFlag(IA_Top)) {
			pan[1] = static_cast<int>(displayBL[2]) -m_pPrivate->m_nYOffs;
		}
		else
			if (m_pPrivate->m_imageAlign.testFlag(IA_Bottom)) {
				pan[1] = m_pPrivate->m_nYOffs - static_cast<int>(displayBL[2]);
			}

		SetPan(pan);
	}
#else
	// First reset the pan.
	double pan[2] = {0,0};
	SetPan(pan);

	if (!m_pPrivate->m_imageAlign == static_cast<ImageAlignmentFlags>(IA_HCenter | IA_VCenter)) {
		vtkRenderer* pRenderer = GetRenderer();
		vtkImageActor* pImageActor = GetImageActor();

		double bounds[6];
		pImageActor->GetDisplayBounds(bounds);

		pRenderer->SetWorldPoint(bounds[0],bounds[2],bounds[4],0);
		pRenderer->WorldToDisplay();

		double displayBL[3];
		pRenderer->GetDisplayPoint(displayBL);

		// Calculate the horizontal pan
		if (m_pPrivate->m_imageAlign.testFlag(IA_Left)) {
			pan[0] = displayBL[0] -m_pPrivate->m_nXOffs;
		}
		else
			if (m_pPrivate->m_imageAlign.testFlag(IA_Right)) {
				pan[0] = m_pPrivate->m_nXOffs - displayBL[0];
			}
		
		// Calculate the vertical pan
		if (m_pPrivate->m_imageAlign.testFlag(IA_Top)) {
			pan[1] = displayBL[2] -m_pPrivate->m_nYOffs;
		}
		else
			if (m_pPrivate->m_imageAlign.testFlag(IA_Bottom)) {
				pan[1] = m_pPrivate->m_nYOffs - displayBL[2];
			}

		SetPan(pan);
	}
#endif
}

/////////////////////////////////////////////////////////////////////////////////////////

/**
 *	\brief
 *	Change the image alignment from the default centered horizontally and vertically.
 */
void smvtkImageView2D::setImageAlignment( ImageAlignmentFlags align /*= IA_HCenter | IA_VCenter*/ )
{
	if (m_pPrivate != nullptr) {
		m_pPrivate->m_imageAlign = align;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////

void smvtkImageView2D::setImageAlignmentOffset( uint8_t nXOffs, uint8_t nYOffs )
{
	if (m_pPrivate != nullptr) {
		m_pPrivate->m_nXOffs = nXOffs;
		m_pPrivate->m_nYOffs = nYOffs;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////

#ifndef USE_MED_VTK_INRIA_EX

void smvtkImageView2D::SetFGImage( vtkImageData* image, vtkLookupTable* lut, int xoffs/*=0*/,int yoffs/*=0*/,int zoffs/*=0*/ )
{
	if (m_pPrivate != nullptr) {
		if (m_pPrivate->m_pFGImageActor.GetPointer() != nullptr) {
			this->GetRenderer()->RemoveViewProp(m_pPrivate->m_pFGImageActor);
		}

		vtkSmartPointer<vtkImageMapToColors>	windowLevel = vtkSmartPointer<vtkImageMapToColors>::New();
		windowLevel->SetLookupTable(lut);
		windowLevel->SetInput(image);
		windowLevel->PassAlphaToOutputOn();

		// set up the vtk pipeline 

		vtkSmartPointer<vtkImageReslice>		relsice = vtkSmartPointer<vtkImageReslice>::New();
		relsice->SetOutputDimensionality(2);
		relsice->InterpolateOff();
		relsice->SetInput(windowLevel->GetOutput());

		vtkSmartPointer<vtkImageActor>			actor = vtkSmartPointer<vtkImageActor>::New();
		actor->SetInput(relsice->GetOutput());

		m_pPrivate->m_pFGImageActor = actor;
		m_pPrivate->m_pFGWindowLevel = windowLevel;
		m_pPrivate->m_pFGImageReslice = relsice;

		m_pPrivate->m_pFGImageActor->SetOpacity(0.95);

		//Update();
		//SetupAnnotations();

		m_pPrivate->m_pFGImage = image;
		this->GetRenderer()->AddViewProp(m_pPrivate->m_pFGImageActor);	
		this->GetRenderer()->Render();
	}
}

#endif

/////////////////////////////////////////////////////////////////////////////////////////

void smvtkImageView2D::SetVisibility (int state)
{

#ifndef USE_MED_VTK_INRIA_EX
	this->ImageActor->SetVisibility (state);
#else
	this->GetImageActor()->SetVisibility(state);
#endif

}

/////////////////////////////////////////////////////////////////////////////////////////

void smvtkImageView2D::SetVisibility( Actors actor, int state )
{
	switch(actor) {
		case IMAGE_ACTOR:
			SetVisibility (state);
			break;
		case FG_IMAGE_ACTOR:
			if (( m_pPrivate != nullptr) && (m_pPrivate->m_pFGImageActor != nullptr)) {
				m_pPrivate->m_pFGImageActor->SetVisibility(state);
			}
			break;
		case CROSSHAIR_ACTOR:
			/*
			if (HorizontalLineActor != nullptr) {
				HorizontalLineActor->SetVisibility(state);
			}
			if (VerticalLineActor != nullptr) {
				VerticalLineActor->SetVisibility(state);
			}
			*/
			break;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////

bool smvtkImageView2D::GetSafeScreenCoordinatesFromDisplayCoordinates( double* pDisplay, int screen[2] )
{
	bool retVal = (pDisplay != nullptr);
	if (retVal) {
		vtkRenderWindow* pRenderWindow = GetRenderWindow();
		retVal = (pRenderWindow != nullptr);
		if (retVal) {
			int* pPos = pRenderWindow->GetPosition();
			retVal = (pPos != nullptr);
			if (retVal) {
				int* pSize = pRenderWindow->GetSize();
				retVal = (pSize != nullptr);
				if (retVal) {
					screen[0] = static_cast<int>(pDisplay[0]) + pPos[0];
					screen[1] = (pSize[1]-static_cast<int>(pDisplay[1])) + pPos[1];
				}
				
			}
		}
	}
	return retVal;
}

/////////////////////////////////////////////////////////////////////////////////////////
