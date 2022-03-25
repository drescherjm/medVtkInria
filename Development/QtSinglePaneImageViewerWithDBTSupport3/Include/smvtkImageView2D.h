#pragma once

#ifndef SMVTKIMAGEVIEW2D_H
#define SMVTKIMAGEVIEW2D_H

#define USE_MED_VTK_INRIA_EX

#include <vtkImageView2D.h>
#include <QSharedPointer>
#include <QSize>
#include <QFlags>
#include <array>

class smvtkInteractorEvent;
class vtkLookupTable;

/////////////////////////////////////////////////////////////////////////////////////////

/**
*	
*	\ingroup smGUI
*/

class smvtkImageView2D : public vtkImageView2D
{
public:
	friend class smvtkImageView2DCommand;
	static smvtkImageView2D *New();

#if VTK_MAJOR_VERSION < 6
	vtkTypeRevisionMacro (smvtkImageView2D, vtkImageView2D);
#else
	vtkTypeMacro(smvtkImageView2D, vtkImageView2D);
#endif

	enum ImageAlignmentFlag {
		IA_HCenter	=0x00,
		IA_Left		=0x01,
		IA_Right	=0x02,
		IA_VCenter	=0x00,
		IA_Top		=0x10,
		IA_Bottom	=0x20,
	};

	Q_DECLARE_FLAGS(ImageAlignmentFlags, ImageAlignmentFlag);

	enum Actors {
			IMAGE_ACTOR,
			FG_IMAGE_ACTOR,
			CROSSHAIR_ACTOR,
	};

public:
	virtual bool 	GetSafeDisplayCoordinatesFromImageCoordinates (int indices[3], double* dispaypos);
	virtual bool	GetSafeWorldCoordinatesFromImageCoordinates(int indices[3], double* position);

	virtual bool	GetSafeScreenCoordinatesFromDisplayCoordinates(double* pDisplay, int screen[2]);

	virtual	void	setImageAlignment(ImageAlignmentFlags align = IA_HCenter | IA_VCenter);
	virtual void	setImageAlignmentOffset(uint8_t nXOffs=2, uint8_t nYOffs=2);

#ifndef USE_MED_VTK_INRIA_EX
	virtual void	SetFGImage( vtkImageData* image, vtkLookupTable* lut, 
		int xoffs/*=0*/,int yoffs/*=0*/,int zoffs/*=0*/ );
#endif

	virtual void	SetVisibility( Actors actor, int state );
	virtual	void	SetVisibility (int state);


#ifndef USING_OLD_VTK_PIPELINE
	virtual void SetInputConnection(vtkAlgorithmOutput* pAlgorithmOutput);
#else
	// Description:
	// Set/Get the input image to the viewer.
	virtual void SetInput (vtkImageData *image);
#endif

	/**
	The SlicePlane instance (GetSlicePlane()) is the polygonal
	square corresponding to the slice plane,
	it is color-coded according to conventions.

	UpdateSlicePlane() is thus called each time we change slice
	or change orientation.
	*/
	virtual void UpdateSlicePlane (void);

	virtual void flipVertical();
	virtual void flipHorizontal();
	virtual void flipVerticalAndHorizontal();

	virtual void UpdateCenter (void);

	virtual void Reset(void);

	virtual	void initFirstView();

	QSize  getLastWindowSize();

	virtual void resetCameraOnWindowSizeChange(bool bForce=false);
	virtual bool disableAxesCrosshair(bool bDisable);


	virtual void UpdateAlignment();

	/**
	Reset the camera in a nice way for the 2D view
	*/
	virtual void ResetCamera (void);	

	/**
	Overwrite of the Superclass InstallPipeline() method in order to set up the
	home made InteractorStyle, and make it observe all events we need
	*/
	virtual void InstallPipeline(void);
	/**
	Overwrite of the Superclass UnInstallPipeline() method in order to set up the
	home made InteractorStyle, and make it observe all events we need
	*/
	virtual void UnInstallPipeline(void);

protected:
	smvtkImageView2D();
	~smvtkImageView2D(); 

	virtual void SetAnnotationsFromOrientation(void);

	vtkImageView2DCommand*	CreateCommandHandler();

private:
	class smPrivate;
	smPrivate* m_pPrivate;

private: 
	smvtkImageView2D(const smvtkImageView2D&);			// Intentionally not implemented.
	void operator=(const smvtkImageView2D&);			// Intentionally not implemented.

};

/////////////////////////////////////////////////////////////////////////////////////////

Q_DECLARE_OPERATORS_FOR_FLAGS(smvtkImageView2D::ImageAlignmentFlags);

#endif // SMVTKIMAGEVIEW2D_H
