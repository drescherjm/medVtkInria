#include "vtkImageViewCornerAnnotationEx.h"

#include "vtkImageMapToWindowLevelColors.h"
#include "vtkImageData.h"
#include "vtkImageActor.h"
#include "vtkTextMapper.h"
#include "vtkObjectFactory.h"


#include "vtkAlgorithmOutput.h"
#include "vtkPropCollection.h"
#include "vtkTextMapper.h"
#include "vtkTextProperty.h"
#include "vtkViewport.h"
#include "vtkWindow.h"
#include "vtkCamera.h"
#include "vtkRenderer.h"
#include "vtkMath.h"
#include "vtkMatrix4x4.h"

#include "vtkImageView2D.h"

#include <cmath>
#include "vtkImageView2DExtended.h"

vtkStandardNewMacro(vtkImageViewCornerAnnotationEx);


vtkImageViewCornerAnnotationEx::vtkImageViewCornerAnnotationEx()
{

}


vtkImageViewCornerAnnotationEx::~vtkImageViewCornerAnnotationEx()
{
}


void vtkImageViewCornerAnnotationEx::TextReplace(vtkImageActor *ia,
	vtkImageMapToWindowLevelColors *wl)
{

	int i;

	// search for tokens, replace and then assign to TextMappers
	for (i = 0; i < 4; i++)
	{
		if (this->CornerText[i] && strlen(this->CornerText[i]))
		{

			std::string str = this->CornerText[i];

			str = replaceStrings(ia,wl,str);

			this->TextMapper[i]->SetInput(str.c_str());


		}
		else
		{
			this->TextMapper[i]->SetInput("");
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////

std::string vtkImageViewCornerAnnotationEx::replaceStrings( vtkImageActor *ia,
	vtkImageMapToWindowLevelColors *wl,
	std::string str )
{
	char *text, *text2;
	int slice = 0, slice_max = 0;
	char *rpos, *tmp;
	double window = 0, level = 0;
	long int windowi = 0, leveli = 0;
	vtkImageData *wl_input = NULL, *ia_input = NULL;
	vtkCamera *cam = NULL;

	int input_type_is_float = 0;
	int size_x = -1, size_y = -1, size_z = -1;
	double spacing_x = 0.0, spacing_y = 0.0, spacing_z = 0.0;
	double pos[3]={0.0, 0.0, 0.0};
	double pos_x = 0.0, pos_y = 0.0, pos_z = 0.0;
	int coord[3]={0, 0, 0};
	int coord_x = 0, coord_y = 0, coord_z = 0;
	double value = 0.0, zoom = 100.0, cameraScale = 1;
	std::string patient_name, study_name, series_name;


	vtkImageView* pImageView = this->GetImageView();

	vtkImageView2DExtended* view2d = vtkImageView2DExtended::SafeDownCast (pImageView);

	if (view2d)
	{

		pImageView->GetCurrentPoint (pos);
		pImageView->GetImageCoordinatesFromWorldCoordinates (pos, coord);
		value = pImageView->GetValueAtPosition(pos);
		zoom  = pImageView->GetZoom()*100.0;
		patient_name = pImageView->GetPatientName();
		study_name   = pImageView->GetStudyName();
		series_name  = pImageView->GetSeriesName();
		window = pImageView->GetColorWindow();
		window *= this->LevelScale;
		level = pImageView->GetColorLevel();
		level = level * this->LevelScale + this->LevelShift;
		windowi = (long int)window;
		leveli = (long int)level;
		cameraScale = pImageView->GetCameraParallelScale();
		
		auto pImageActor = view2d->GetImageActor(0);
		if (pImageActor) {
			wl_input = pImageActor->GetInput();
			cam = pImageView->GetRenderer() ? pImageView->GetRenderer()->GetActiveCamera() : NULL;

			if (wl_input && cam)
			{
				double* viewup = cam->GetViewUp();
				double* normal = cam->GetViewPlaneNormal();
				double rightvector[3];
				vtkMath::Cross(normal, viewup, rightvector);

				// naively the X and Y axes of the current view
				// correspond to the rightvector and the viewup respectively.
				// But in fact we have to put those vectors back in the image
				// coordinates and see to which xyz image axis they correspond.

				double Xaxis[4] = { 0,0,0,0 };
				double Yaxis[4] = { 0,0,0,0 };
				for (unsigned int i = 0; i < 3; i++)
				{
					Xaxis[i] = rightvector[i];
					Yaxis[i] = viewup[i];
				}

				pImageView->GetInvertOrientationMatrix()->MultiplyPoint(Xaxis, Xaxis);
				pImageView->GetInvertOrientationMatrix()->MultiplyPoint(Yaxis, Yaxis);

				double dotX = 0;
				double dotY = 0;
				int idX, idY, idZ;
				idX = idY = idZ = 0;

				for (unsigned int i = 0; i < 3; i++)
				{
					if (dotX <= std::fabs(Xaxis[i]))
					{
						dotX = std::fabs(Xaxis[i]);
						idX = i;
					}
					if (dotY <= std::fabs(Yaxis[i]))
					{
						dotY = std::fabs(Yaxis[i]);
						idY = i;
					}
				}
				if (view2d)
					idZ = view2d->GetSliceOrientation();

				input_type_is_float = (wl_input->GetScalarType() == VTK_FLOAT ||
					wl_input->GetScalarType() == VTK_DOUBLE);

				size_x = wl_input->GetDimensions()[idX];
				size_y = wl_input->GetDimensions()[idY];
				size_z = wl_input->GetDimensions()[idZ];

				spacing_x = wl_input->GetSpacing()[idX];
				spacing_y = wl_input->GetSpacing()[idY];
				spacing_z = wl_input->GetSpacing()[idZ];

				pos_x = pos[idX];
				pos_y = pos[idY];
				pos_z = pos[idZ];

				coord_x = coord[idX];
				coord_y = coord[idY];
				coord_z = coord[idZ];
			}
		}
	}


	if (ia)
	{
		slice = ia->GetSliceNumber() - ia->GetSliceNumberMin() + 1;
		slice_max = ia->GetSliceNumberMax() - ia->GetSliceNumberMin() + 1;
		ia_input = ia->GetInput();
		if (!wl_input && ia_input)
		{
			input_type_is_float = (ia_input->GetScalarType() == VTK_FLOAT || 
				ia_input->GetScalarType() == VTK_DOUBLE);
		}
	}

	text = new char [str.length()+1000];
	text2 = new char [str.length()+1000];
	strcpy(text,str.c_str());

	// now do the replacements

	rpos = strstr(text,"<image>");
	while (rpos)
	{
		*rpos = '\0';
		if (ia && this->ShowSliceAndImage)
		{
			sprintf(text2,"%sImage: %i%s",text,slice,rpos+7);
		}
		else
		{
			sprintf(text2,"%s%s",text,rpos+7);
		}
		tmp = text;
		text = text2;
		text2 = tmp;
		rpos = strstr(text,"<image>");
	}

	rpos = strstr(text,"<image_and_max>");
	while (rpos)
	{
		*rpos = '\0';
		if (ia && this->ShowSliceAndImage)
		{
			sprintf(text2,"%sImage: %i / %i%s",text,slice,slice_max,rpos+15);
		}
		else
		{
			sprintf(text2,"%s%s",text,rpos+15);
		}
		tmp = text;
		text = text2;
		text2 = tmp;
		rpos = strstr(text,"<image_and_max>");
	}

	rpos = strstr(text,"<slice>");
	while (rpos)
	{
		*rpos = '\0';
		if (ia && this->ShowSliceAndImage)
		{
			sprintf(text2,"%sSlice: %i%s",text,slice,rpos+7);
		}
		else
		{
			sprintf(text2,"%s%s",text,rpos+7);
		}
		tmp = text;
		text = text2;
		text2 = tmp;
		rpos = strstr(text,"<slice>");
	}

	rpos = strstr(text,"<slice_and_max>");
	while (rpos)
	{
		*rpos = '\0';
		if (ia && this->ShowSliceAndImage)
		{
			sprintf(text2,"%sSlice: %i / %i%s",text,slice,slice_max,rpos+15);
		}
		else
		{
			sprintf(text2,"%s%s",text,rpos+15);
		}
		tmp = text;
		text = text2;
		text2 = tmp;
		rpos = strstr(text,"<slice_and_max>");
	}

	rpos = strstr(text,"<image_slice_and_max>");
	while (rpos)
	{
		*rpos = '\0';
		size_t offset = strlen("<image_slice_and_max>");
		if (ia && this->ShowSliceAndImage)
		{
			sprintf(text2,"%sImage: %i / %i%s",text,slice,slice_max,rpos+offset);
		}
		else
		{
			sprintf(text2,"%s%s",text,rpos+offset);
		}
		tmp = text;
		text = text2;
		text2 = tmp;
		rpos = strstr(text,"<image_slice_and_max>");
	}

	rpos = strstr(text,"<slice_pos>");
	while (rpos)
	{
		*rpos = '\0';
		if (ia && this->ShowSliceAndImage)
		{
			double *dbounds = ia->GetDisplayBounds();
			int *dext = ia->GetDisplayExtent();
			double pos;
			if (dext[0] == dext[1])
			{
				pos = dbounds[0];
			}
			else if (dext[2] == dext[3])
			{
				pos = dbounds[2];
			}
			else
			{
				pos = dbounds[4];
			}
			sprintf(text2,"%s%g%s",text,pos,rpos+11);
		}
		else
		{
			sprintf(text2,"%s%s",text,rpos+11);
		}
		tmp = text;
		text = text2;
		text2 = tmp;
		rpos = strstr(text,"<slice_pos>");
	}

	rpos = strstr(text,"<window>");
	while (rpos)
	{
		*rpos = '\0';
		if (pImageView)
		{
			if (input_type_is_float)
			{
				sprintf(text2,"%sWindow: %g%s",text,window,rpos+8);
			}
			else
			{
				sprintf(text2,"%sWindow: %li%s",text,windowi,rpos+8);
			}
		}
		else
		{
			sprintf(text2,"%s%s",text,rpos+8);
		}
		tmp = text;
		text = text2;
		text2 = tmp;
		rpos = strstr(text,"<window>");
	}

	rpos = strstr(text,"<level>");
	while (rpos)
	{
		*rpos = '\0';
		if (pImageView)
		{
			if (input_type_is_float)
			{
				sprintf(text2,"%sLevel: %g%s",text,level,rpos+7);
			}
			else
			{
				sprintf(text2,"%sLevel: %li%s",text,leveli,rpos+7);
			}
		}
		else
		{
			sprintf(text2,"%s%s",text,rpos+7);
		}
		tmp = text;
		text = text2;
		text2 = tmp;
		rpos = strstr(text,"<level>");
	}

	rpos = strstr(text,"<window_level>");
	while (rpos)
	{
		*rpos = '\0';
		if (pImageView)
		{
			if (input_type_is_float)
			{
				sprintf(text2,"%sWW/WL: %g / %g%s",text,window,level,rpos+14);
			}
			else
			{
				sprintf(text2,"%sWW/WL: %li / %li%s",text,windowi,leveli,rpos+14);
			}
		}
		else
		{
			sprintf(text2,"%s%s",text,rpos+14);
		}
		tmp = text;
		text = text2;
		text2 = tmp;
		rpos = strstr(text,"<window_level>");
	}


	rpos = strstr(text,"<size_x>");
	while (rpos)
	{
		*rpos = '\0';
		sprintf(text2,"%s%i%s",text,size_x,rpos+8);
		tmp = text;
		text = text2;
		text2 = tmp;
		rpos = strstr(text,"<size_x>");
	}

	rpos = strstr(text,"<size_y>");
	while (rpos)
	{
		*rpos = '\0';
		sprintf(text2,"%s%i%s",text,size_y,rpos+8);
		tmp = text;
		text = text2;
		text2 = tmp;
		rpos = strstr(text,"<size_y>");
	}

	rpos = strstr(text,"<size_z>");
	while (rpos)
	{
		*rpos = '\0';
		sprintf(text2,"%s%i%s",text,size_z,rpos+8);
		tmp = text;
		text = text2;
		text2 = tmp;
		rpos = strstr(text,"<size_z>");
	}

	rpos = strstr(text,"<size>");
	while (rpos)
	{
		*rpos = '\0';
		sprintf(text2,"%simage size: %i x %i%s",text,size_x,size_y,rpos+6);
		tmp = text;
		text = text2;
		text2 = tmp;
		rpos = strstr(text,"<size>");
	}

	rpos = strstr(text,"<spacing_x>");
	while (rpos)
	{
		*rpos = '\0';
		sprintf(text2,"%s%3.3g%s",text,spacing_x,rpos+11);
		tmp = text;
		text = text2;
		text2 = tmp;
		rpos = strstr(text,"<spacing_x>");
	}

	rpos = strstr(text,"<spacing_y>");
	while (rpos)
	{
		*rpos = '\0';
		sprintf(text2,"%s%3.3g%s",text,spacing_y,rpos+11);
		tmp = text;
		text = text2;
		text2 = tmp;
		rpos = strstr(text,"<spacing_y>");
	}

	rpos = strstr(text,"<spacing_z>");
	while (rpos)
	{
		*rpos = '\0';
		sprintf(text2,"%s%3.3g%s",text,spacing_z,rpos+11);
		tmp = text;
		text = text2;
		text2 = tmp;
		rpos = strstr(text,"<spacing_z>");
	}

	rpos = strstr(text,"<spacing>");
	while (rpos)
	{
		*rpos = '\0';
		sprintf(text2,"%sspacing: %3.3g x %3.3g mm%s",text,spacing_x, spacing_y, rpos+9);
		tmp = text;
		text = text2;
		text2 = tmp;
		rpos = strstr(text,"<spacing>");
	}

	rpos = strstr(text,"<pos_x>");
	while (rpos)
	{
		*rpos = '\0';
		sprintf(text2,"%s%g%s",text,pos_x,rpos+7);
		tmp = text;
		text = text2;
		text2 = tmp;
		rpos = strstr(text,"<pos_x>");
	}

	rpos = strstr(text,"<pos_y>");
	while (rpos)
	{
		*rpos = '\0';
		sprintf(text2,"%s%g%s",text,pos_y,rpos+7);
		tmp = text;
		text = text2;
		text2 = tmp;
		rpos = strstr(text,"<pos_y>");
	}

	rpos = strstr(text,"<pos_z>");
	while (rpos)
	{
		*rpos = '\0';
		sprintf(text2,"%s%g%s",text,pos_z,rpos+7);
		tmp = text;
		text = text2;
		text2 = tmp;
		rpos = strstr(text,"<pos_z>");
	}

	rpos = strstr(text,"<coord_x>");
	while (rpos)
	{
		*rpos = '\0';
		sprintf(text2,"%s%i%s",text,coord_x,rpos+9);
		tmp = text;
		text = text2;
		text2 = tmp;
		rpos = strstr(text,"<coord_x>");
	}

	rpos = strstr(text,"<coord_y>");
	while (rpos)
	{
		*rpos = '\0';
		sprintf(text2,"%s%i%s",text,coord_y,rpos+9);
		tmp = text;
		text = text2;
		text2 = tmp;
		rpos = strstr(text,"<coord_y>");
	}

	rpos = strstr(text,"<coord_z>");
	while (rpos)
	{
		*rpos = '\0';
		sprintf(text2,"%s%i%s",text,coord_z,rpos+9);
		tmp = text;
		text = text2;
		text2 = tmp;
		rpos = strstr(text,"<coord_z>");
	}

	rpos = strstr(text,"<xyz>");
	while (rpos)
	{
		*rpos = '\0';
		sprintf(text2,"%sxyz: %4.4g, %4.4g, %4.4g mm%s",text,pos[0], pos[1], pos[2],rpos+5);
		tmp = text;
		text = text2;
		text2 = tmp;
		rpos = strstr(text,"<xyz>");
	}

	rpos = strstr(text,"<value>");
	while (rpos)
	{
		*rpos = '\0';
		sprintf(text2,"%svalue: %4.4g%s",text,value,rpos+7);
		tmp = text;
		text = text2;
		text2 = tmp;
		rpos = strstr(text,"<value>");
	}

	rpos = strstr(text,"<zoom>");
	while (rpos)
	{
		*rpos = '\0';
		sprintf(text2,"%szoom: %4.4g scale: %4.4g%s",text,zoom,cameraScale,rpos+6);
		tmp = text;
		text = text2;
		text2 = tmp;
		rpos = strstr(text,"<zoom>");
	}

	rpos = strstr(text,"<patient>");
	while (rpos)
	{
		*rpos = '\0';
		sprintf(text2,"%s%s%s",text,patient_name.c_str(),rpos+9);
		tmp = text;
		text = text2;
		text2 = tmp;
		rpos = strstr(text,"<patient>");
	}

	rpos = strstr(text,"<study>");
	while (rpos)
	{
		*rpos = '\0';
		sprintf(text2,"%s%s%s",text,study_name.c_str(),rpos+7);
		tmp = text;
		text = text2;
		text2 = tmp;
		rpos = strstr(text,"<study>");
	}

	rpos = strstr(text,"<series>");
	while (rpos)
	{
		*rpos = '\0';
		sprintf(text2,"%s%s%s",text,series_name.c_str(),rpos+8);
		tmp = text;
		text = text2;
		text2 = tmp;
		rpos = strstr(text,"<series>");
	}

	std::string retVal(text);

	delete [] text;
	delete [] text2;

	return retVal;
}

