#include "imageHelpers.h"

/////////////////////////////////////////////////////////////////////////////////////////

vtkSmartPointer<vtkImageData> createTestImage(uint16_t nX, uint16_t nY, uint16_t nZ)
{
	vtkSmartPointer<vtkImageData> retVal = vtkNew<vtkImageData>();
	retVal->SetDimensions(nX, nY, nZ);
	retVal->AllocateScalars(VTK_UNSIGNED_SHORT, 1);
	retVal->SetSpacing(1.0, 1.0, 1.0);

	uint16_t* pDest = static_cast<uint16_t*>(retVal->GetScalarPointer());
	uint16_t val = 0;

	for (decltype(nZ) k = 0; k < nZ; ++k) {
		//val = 0;
		for (decltype(nY) j = 0; j < nY; ++j) {

			if ((j % 64) == 0) {
				val += 16;
				if (val > 512) {
					val = 0;
				}
			}

			for (decltype(nX) i = 0; i < nX; ++i) {
				*pDest++ = val;
			}
		}
	}

	return retVal;
}

/////////////////////////////////////////////////////////////////////////////////////////
