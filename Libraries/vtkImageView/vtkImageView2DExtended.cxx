#include "vtkImageView2DExtended.h"
#include "vtkSetGet.h"
#include "vtkObjectFactory.h"
#include "vtkImageData.h"
#include "vtkImageActor.h"
#include "vtkImageMapToColors.h"
#include "vtkImageView.h"
#include "vtkRenderer.h"
#include "vtkLookupTable.h"
#include "vtkRenderWindow.h"
#include "vtkMath.h"
#include "vtkImageViewCornerAnnotationEx.h"
#include "vtkMatrix4x4.h"
#include <algorithm>
#include <vtkAlgorithmOutput.h>
#include "vtkImage2DDisplay.h"
#include <vtkImageMapper3D.h>

/////////////////////////////////////////////////////////////////////////////////////////

vtkStandardNewMacro(vtkImageView2DExtended);

/////////////////////////////////////////////////////////////////////////////////////////

vtkImageView2DExtended::vtkImageView2DExtended()
{
	// Remove the old corner annotation from the view.
	this->GetRenderer()->RemoveViewProp(this->CornerAnnotation);
	this->CornerAnnotation->Delete();

	this->CornerAnnotation = vtkImageViewCornerAnnotationEx::New();
	this->CornerAnnotation->SetNonlinearFontScaleFactor (0.3);
	this->CornerAnnotation->SetTextProperty ( this->TextProperty );
	this->CornerAnnotation->SetMaximumFontSize (46);
	this->CornerAnnotation->SetImageView (this);
	this->CornerAnnotation->PickableOff();
	this->CornerAnnotation->SetText (3, "<patient>\n<study>\n<series>");

	this->GetRenderer()->AddViewProp(this->CornerAnnotation);
}

/////////////////////////////////////////////////////////////////////////////////////////

vtkImageView2DExtended::~vtkImageView2DExtended()
{
	
}

/////////////////////////////////////////////////////////////////////////////////////////

void vtkImageView2DExtended::PrintSelf( ostream& os, vtkIndent indent )
{
	Superclass::PrintSelf(os,indent);
}

/////////////////////////////////////////////////////////////////////////////////////////

void vtkImageView2DExtended::SetFGImage(vtkAlgorithmOutput* pi_poVtkAlgoOutput, vtkLookupTable* lut, int xoffs/*=0*/,int yoffs/*=0*/,int zoffs/*=0*/ )
{
	if (FGImageActor != NULL) {
		this->GetRenderer()->RemoveViewProp(FGImageActor);
	}

	vtkImageMapToColors*	windowLevel = vtkImageMapToColors::New();
	windowLevel->SetLookupTable(lut);
	windowLevel->SetInputConnection(pi_poVtkAlgoOutput);
	windowLevel->PassAlphaToOutputOn();
	windowLevel->Update();

	// set up the vtk pipeline 

	vtkImageActor*	actor = vtkImageActor::New();
	actor->GetMapper()->SetInputConnection(windowLevel->GetOutputPort());

	FGImageActor = actor;
	FGWindowLevel = windowLevel;

	this->FGImageActor->SetOpacity(0.95);

	//Update();
	//SetupAnnotations();

	//FGImage->Update();
	FGImage = pi_poVtkAlgoOutput;
	this->GetRenderer()->AddViewProp(this->FGImageActor);	
	//this->GetRenderer()->Render();
	this->Modified();

	this->UpdateDisplayExtent();
	this->GetRenderer()->Render();
}

/////////////////////////////////////////////////////////////////////////////////////////

void vtkImageView2DExtended::UpdateDisplayExtent()
{
	Superclass::UpdateDisplayExtent();

	auto input = this->FGImage;
	if (!input)
	{
		return;
	}

	auto pAlgorithm = input->GetProducer();
	
	if (!pAlgorithm) {
		return;
	}

	pAlgorithm->UpdateInformation();
	//int *w_ext = pAlgorithm->GetOutputDataObject(0)->GetWholeExtent();

	int* w_ext = this->GetMedVtkImageInfo()->extent;

	// get and clamp the slice if necessary
	int  slice = this->GetSliceForWorldCoordinates (this->CurrentPoint);

	int *range = this->GetSliceRange();
	if (range)
	{
		slice = std::max (slice, range[0]);
		slice = std::min (slice, range[1]);
	}
	
	switch (this->SliceOrientation)
	{
	case vtkImageView2D::SLICE_ORIENTATION_XY:
		this->FGImageActor->SetDisplayExtent(w_ext[0], w_ext[1], w_ext[2], w_ext[3], slice, slice);
		break;

	case vtkImageView2D::SLICE_ORIENTATION_XZ:
		this->FGImageActor->SetDisplayExtent(w_ext[0], w_ext[1], slice, slice, w_ext[4], w_ext[5]);
		break;

	case vtkImageView2D::SLICE_ORIENTATION_YZ:
	default:
		this->FGImageActor->SetDisplayExtent(slice, slice, w_ext[2], w_ext[3], w_ext[4], w_ext[5]);
		break;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////

void vtkImageView2DExtended::SetVisibility (int state)
{
	auto pImageActor = GetImageActor(IMAGE_ACTOR);
	if (pImageActor) {
		pImageActor->SetVisibility(state);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////


void vtkImageView2DExtended::SetVisibility( VTKObjectType actor, int state )
{
	switch(actor) {
	case IMAGE_ACTOR:
		SetVisibility (state);
		break;
	case FG_IMAGE_ACTOR:
		if (FGImageActor != NULL) {
			FGImageActor->SetVisibility(state);
		}
		break;
	case CROSSHAIR_ACTOR:
		SetShowImageAxis(state);
		break;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////

int vtkImageView2DExtended::GetVisibility(VTKObjectType actor)
{
	int retVal = 0;
	switch (actor) {
	case IMAGE_ACTOR:
	case FG_IMAGE_ACTOR:
		{
			auto pImageActor = GetImageActor(actor);
			if (pImageActor) {
				retVal = pImageActor->GetVisibility();
			}
		}
		break;
	case CROSSHAIR_ACTOR:
		retVal = GetShowImageAxis();
		break;
	}
	return retVal;
}

/////////////////////////////////////////////////////////////////////////////////////////

void vtkImageView2DExtended::SetOpacity(double opacity)
{
	auto pImageActor = GetImageActor(IMAGE_ACTOR);
	if (!pImageActor)
		return;
	pImageActor->SetOpacity(opacity);
}

/////////////////////////////////////////////////////////////////////////////////////////

int vtkImageView2DExtended::GetZSlice()
{
	return Superclass::GetSlice();
}

/////////////////////////////////////////////////////////////////////////////////////////

int vtkImageView2DExtended::GetWholeZMin()
{
	return GetSliceMin();
}

/////////////////////////////////////////////////////////////////////////////////////////

int vtkImageView2DExtended::GetWholeZMax()
{
	return GetSliceMax();
}

/////////////////////////////////////////////////////////////////////////////////////////

vtkRenderWindowInteractor * vtkImageView2DExtended::GetRenderWindowInteractor()
{
	return GetRenderWindow()->GetInteractor();
}

/////////////////////////////////////////////////////////////////////////////////////////

int vtkImageView2DExtended::GetOrientation()
{
	return GetSliceOrientation();
}

/////////////////////////////////////////////////////////////////////////////////////////

unsigned int vtkImageView2DExtended::GetOrthogonalAxis( unsigned int p_plan )
{
	switch(p_plan)
	{
	case VIEW_ORIENTATION_SAGITTAL:
		return X_ID;
		break;

	case VIEW_ORIENTATION_CORONAL:
		return Y_ID;
		break;

	case VIEW_ORIENTATION_AXIAL:

		return Z_ID;
		break;      
	}
	return 0;    
}

/////////////////////////////////////////////////////////////////////////////////////////

bool vtkImageView2DExtended::GetFlipZaxis()
{
	return false;
}

/////////////////////////////////////////////////////////////////////////////////////////

vtkImageData* vtkImageView2DExtended::GetImage()
{
	vtkImageData* retVal{};

	auto pImageActor = GetImageActor(IMAGE_ACTOR);
	if (pImageActor) {
		retVal = pImageActor->GetInput();
	}

	return retVal;
}

/////////////////////////////////////////////////////////////////////////////////////////

void vtkImageView2DExtended::SetOrientation( int orientation )
{
	Superclass::SetSliceOrientation(orientation);
}

/////////////////////////////////////////////////////////////////////////////////////////

void vtkImageView2DExtended::Update()
{

}

/////////////////////////////////////////////////////////////////////////////////////////

vtkImageMapToColors* vtkImageView2DExtended::GetFGWindowLevel()
{
	return FGWindowLevel;
}

/////////////////////////////////////////////////////////////////////////////////////////

void vtkImageView2DExtended::SetSizeData( const char* str )
{
	SizeData_str = str; this->Modified();
}

/////////////////////////////////////////////////////////////////////////////////////////

const char* vtkImageView2DExtended::GetSizeData( void ) const
{
	return SizeData_str.c_str();
}

/////////////////////////////////////////////////////////////////////////////////////////

void vtkImageView2DExtended::SetSlice (unsigned int p_plan, int p_zslice)
{
	double pos[3];
	this->GetPositionForSlice (p_zslice, p_plan, pos);
	this->SetCurrentPoint(pos);
}

/////////////////////////////////////////////////////////////////////////////////////////

void vtkImageView2DExtended::GetPositionForSlice (int p_zslice, int orientation, double pos[3])
{
	auto pVtk2DDisplay = GetImage2DDisplayForLayer(IMAGE_ACTOR);

	if (pVtk2DDisplay) {

		medVtkImageInfo* pImageInfo = pVtk2DDisplay->GetMedVtkImageInfo();
		
		if (pImageInfo) {
			unsigned int axis = this->GetOrthogonalAxis(orientation);

			double* spacing = pImageInfo->spacing;
			int* extent = pImageInfo->extent;
			double* origin = pImageInfo->origin;
			int     slice = p_zslice;

			int dims[3];
			dims[0] = extent[1];
			dims[1] = extent[3];
			dims[2] = extent[5];

			if (slice >= dims[axis])  slice = dims[axis];
			if (slice < 0)           slice = 0;

			this->GetCurrentPoint(pos);
			pos[axis] = origin[axis] + slice * spacing[axis];
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////

int vtkImageView2DExtended::GetSliceForPosition(vtkImageData* pImage,const double pos[3], unsigned int p_plan)
{

	if( pImage == NULL )
	{
		return 0;
	}

	assert(p_plan < NB_DIRECTION_IDS);

	double*       spacing = pImage->GetSpacing();
	double*       origin  = pImage->GetOrigin();

	int   axis     = this->GetOrthogonalAxis(p_plan);
	double  soft_pos = pos[axis];

	return (int)vtkMath::Round((soft_pos-origin[axis])/spacing[axis]);
}

/////////////////////////////////////////////////////////////////////////////////////////

int vtkImageView2DExtended::GetSliceForPosition(const double pos[3], unsigned int p_plan)
{
	return GetSliceForPosition(this->GetImage(),pos,p_plan);
}

/////////////////////////////////////////////////////////////////////////////////////////

int vtkImageView2DExtended::GetSlice(unsigned int p_plan)
{
	const double* pos = this->GetCurrentPoint();
	return this->GetSliceForPosition (pos, p_plan);
}

/////////////////////////////////////////////////////////////////////////////////////////

int vtkImageView2DExtended::GetSlice()
{
	return Superclass::GetSlice();
}

/////////////////////////////////////////////////////////////////////////////////////////

vtkImageData* vtkImageView2DExtended::GetFGImage()
{
	vtkImageData* retVal{};

	auto pImageActor = GetImageActor(FG_IMAGE_ACTOR);

	if (pImageActor) {
		retVal = pImageActor->GetInput();
	}

	return retVal;
	//return FGImage;
}

/////////////////////////////////////////////////////////////////////////////////////////

void vtkImageView2DExtended::SetAnnotationsFromOrientation( void )
{
	Superclass::SetAnnotationsFromOrientation();
}

/////////////////////////////////////////////////////////////////////////////////////////

void vtkImageView2DExtended::GetImageCoordinatesFromWorldCoordinates(vtkImageData* pImage,
	double position[3], int* indices)
{
	if (pImage != NULL) {

		// Get information
		double unorientedposition[4] = {position[0], position[1], position[2], 1};
		double* spacing = pImage->GetSpacing();
		double* origin = pImage->GetOrigin();

		// apply inverted orientation matrix to the world-coordinate position
		this->InvertOrientationMatrix->MultiplyPoint (unorientedposition, unorientedposition);

		for (unsigned int i=0; i<3;i++)
		{
			if (fabs (spacing[i]) > 1e-15)
				indices[i] = vtkMath::Round((unorientedposition[i]-origin[i])/spacing[i]);
			else
				indices[i] = 0;
		}
	}
	else
	{
		indices[0] = 0; indices[1] = 0; indices[2] = 0;    
		return;
	}

}

/////////////////////////////////////////////////////////////////////////////////////////

void vtkImageView2DExtended::GetImageCoordinatesFromWorldCoordinates( double position[3], int* indices )
{
	Superclass::GetImageCoordinatesFromWorldCoordinates(position,indices);
}

/////////////////////////////////////////////////////////////////////////////////////////

double vtkImageView2DExtended::GetValueAtPosition(vtkImageData* pImage,
	double worldcoordinates[3], int component )
{

	double retVal = 0.0;
	if (pImage != NULL) {
		int indices[3];
		this->GetImageCoordinatesFromWorldCoordinates (pImage,worldcoordinates, indices);

		auto pImageActor = GetImageActor(IMAGE_ACTOR);
		if (pImageActor) {
			int* extent = pImageActor->GetInput()->GetExtent();
			if (!((indices[0] < extent[0]) ||
				(indices[0] > extent[1]) ||
				(indices[1] < extent[2]) ||
				(indices[1] > extent[3]) ||
				(indices[2] < extent[4]) ||
				(indices[2] > extent[5]))) {

				retVal = pImage->GetScalarComponentAsDouble(indices[0], indices[1], indices[2], component);
			}
		}
			
	}
	
	return retVal;
}

/////////////////////////////////////////////////////////////////////////////////////////

double vtkImageView2DExtended::GetValueAtPosition( double worldcoordinates[3], int component/*=0 */ )
{
	return Superclass::GetValueAtPosition(worldcoordinates,component);
}
