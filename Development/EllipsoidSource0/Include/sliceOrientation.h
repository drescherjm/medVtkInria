#pragma once

#ifndef SLICEORIENTATION_H
#define SLICEORIENTATION_H


//////////////////////////////////////////////////////////////////////////

struct laSliceOrientation 
{
	typedef enum
	{ 
		Sagittal, 
		Coronal, 
		Axial,
		View3D,
		SO_UNKNOWN = -1,
	} OrientationType;

	static std::string		getOrientationName(OrientationType ty);
	static OrientationType	convertTo(int code);

};

inline std::string laSliceOrientation::getOrientationName(OrientationType ty)
{
	std::string retVal;
	switch(ty) {
	case Axial:
		retVal = "Axial";
		break;
	case Coronal:
		retVal = "Coronal";
		break;
	case Sagittal:
		retVal = "Sagittal";
		break;
	case View3D:
		retVal = "View3D";
		break;
	}
	return retVal;
}

/////////////////////////////////////////////////////////////////////////////////////////

inline laSliceOrientation::OrientationType laSliceOrientation::convertTo(int code )
{
	OrientationType retVal;
	switch(code) {
	case Sagittal:
	case Coronal:
	case Axial:
	case View3D:
		retVal = static_cast<OrientationType>(code);
		break;
	default:
		 retVal = SO_UNKNOWN;
	}
	return retVal;
}

//////////////////////////////////////////////////////////////////////////

#endif // SLICEORIENTATION_H
