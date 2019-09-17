
#pragma once

#ifndef VTKIMAGEVIEW2DEXTENDED_H
#define VTKIMAGEVIEW2DEXTENDED_H

#include "vtkImageView2D.h"
#include "vtkSmartPointer.h"

class vtkImageData;
class vtkImageActor;
class vtkImageMapToColors;
class vtkImageViewCollection;
class vtkAlgorithmOutput;

/////////////////////////////////////////////////////////////////////////////////////////

class MEDVTKINRIA_EXPORT vtkImageView2DExtended : public vtkImageView2D
{
public:
	static vtkImageView2DExtended *New();
	
	vtkTypeMacro(vtkImageView2DExtended, vtkImageView2D);

	//BTX
	enum VTKObjectType {
		IMAGE_ACTOR,
		FG_IMAGE_ACTOR,
		CROSSHAIR_ACTOR,
		PAINTBRUSH_WIDGET,
	};
	//ETX

	//BTX
	/** Indices use in Vtk referentials corresponding to the standard x, y and z unitary vectors. */
	enum DirectionIds
	{
		X_ID,
		Y_ID,
		Z_ID,
		NB_DIRECTION_IDS  // The number of DirectionIds
	};
	//ETX

	//BTX
	enum InteractionStyleIds {
		NO_INTERACTION,
		SELECT_INTERACTION = vtkInteractorStyleImageView2D::InteractionTypeSlice,
		WINDOW_LEVEL_INTERACTION = vtkInteractorStyleImageView2D::InteractionTypeWindowLevel,
		ZOOM_INTERACTION = vtkInteractorStyleImageView2D::InteractionTypeZoom,
		FULL_PAGE_INTERACTION,
		MEASURE_INTERACTION,
	};
	//ETX


	//BTX
	enum CompatibilityEventIds
	{
		ViewImagePositionChangeEvent=SliceChangedEvent
	};
	//ETX

	virtual int		GetZSlice();
	virtual int		GetWholeZMin();
	virtual int		GetWholeZMax();
	virtual int		GetOrientation();
	virtual void	SetOrientation(int orientation);
	virtual void	Update();
	vtkImageData*	GetImage();
	
	void	SetSlice (unsigned int p_plan, int p_zslice);
	void	GetPositionForSlice (int p_zslice, int orientation, double pos[3]);
	int		GetSliceForPosition(vtkImageData* pImage,const double pos[3], unsigned int p_plan);
	int		GetSliceForPosition(const double pos[3], unsigned int p_plan);
	int		GetSlice(unsigned int p_plan);
	int		GetSlice();
	 
	/**
		 Get the id of the axis orthogonal to the given plan
		(x for sagittal, y for coronal and z for axial).
	*/
	static unsigned int   GetOrthogonalAxis(unsigned int p_plan);

	vtkRenderWindowInteractor * GetRenderWindowInteractor ();

	bool GetFlipZaxis();

	virtual void SetAnnotationsFromOrientation(void);

	/**
	Set/Get Method for lower left annotation.
	*/
	virtual void SetSizeData (const char* str);
	virtual const char* GetSizeData(void) const;

protected:
	//BTX
	std::string     SizeData_str;
	//ETX

public:
	/**
	*	Set an image that will appear on top of the view image. Transparency with the LUT 
	*    will control what is visible.
	*/
	void	SetFGImage(vtkAlgorithmOutput* pi_poVtkAlgoOutput, vtkLookupTable* lut, int xoffs=0,int yoffs=0,int zoffs=0);
	vtkImageData* GetFGImage();

	virtual void	SetOpacity(double opacity);
	virtual void	GetImageCoordinatesFromWorldCoordinates(vtkImageData* pImage, double position[3], int* indices);
	virtual void	GetImageCoordinatesFromWorldCoordinates(double position[3], int* indices);
	virtual double	GetValueAtPosition(vtkImageData* pImage, double worldcoordinates[3], int component );
	virtual double	GetValueAtPosition(double worldcoordinates[3], int component=0 );

public:
	virtual void	PrintSelf(ostream& os, vtkIndent indent);
	virtual void	UpdateDisplayExtent();
	virtual void	SetVisibility( VTKObjectType actor, int state );
	virtual void	SetVisibility (int state);
	virtual int		GetVisibility( VTKObjectType actor );

	vtkImageMapToColors* GetFGWindowLevel();

protected:
	/// Overlay image				
	vtkSmartPointer<vtkAlgorithmOutput>		FGImage;
	vtkSmartPointer<vtkImageActor>			FGImageActor;
	vtkSmartPointer<vtkImageMapToColors>	FGWindowLevel;

protected:
	vtkImageView2DExtended();
	~vtkImageView2DExtended();
};

/////////////////////////////////////////////////////////////////////////////////////////

#endif // VTKIMAGEVIEW2DEXTENDED_H
