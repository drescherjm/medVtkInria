
#include "lavtkImageViewCornerAnnotation.h"
#include <vtkObjectFactory.h>

vtkStandardNewMacro(lavtkImageViewCornerAnnotation);

/////////////////////////////////////////////////////////////////////////////////////////

lavtkImageViewCornerAnnotation::lavtkImageViewCornerAnnotation()
{
	std::cout << __FUNCTION__;
}

/////////////////////////////////////////////////////////////////////////////////////////

lavtkImageViewCornerAnnotation::~lavtkImageViewCornerAnnotation()
{
}

/////////////////////////////////////////////////////////////////////////////////////////

std::string lavtkImageViewCornerAnnotation::replaceStrings( vtkImageActor *ia,
	vtkImageMapToWindowLevelColors *wl,
	std::string str )
{

	std::string retVal = str;

	//boost::replace_all(retVal,"<slice_and_max>","<image_slice_and_max>");
	
	retVal = Superclass::replaceStrings(ia,wl,retVal);

	return retVal;
}

