// LAGUIExample3

// This example tests overlays of a smaller size than the source image. The purpose
// of this is to verify that the FGImage method will perform efficiently and also 
// not have to create a RGBA buffer to store the result of the blending of the 
// two images.

#include <QApplication>
#include <QMainWindow>
#include "lavtkViewImage2D.h"
#include "vtkImageData.h"
#include "vtkRenderWindow.h"
#include "vtkLookupTable.h"
#include "sliceOrientation.h"
#include <vtkImageSliceMapper.h>
#include <vtkDICOMImageReader.h>
#include <vtkImageImport.h>
#include <vtkImageMapToColors.h>
#include <vtkImageActor.h>
#include <vtkImageAlgorithm.h>
#include <vtkSimpleImageToImageFilter.h>
#include <vtkSimpleImageFilterExample.h>
#include <vtkImageChangeInformation.h>
#include <vtkImageConstantPad.h>

const int ROWS{ 512 };
const int COLS{ 512 };
const int SLICES{ 400 };

const int MASK_ROW_OFFS{ 20 };
const int MASK_COL_OFFS{ 20 };
const int MASK_SLICE_OFFS{10};

const int MASK_ROWS = ROWS - MASK_ROW_OFFS - MASK_ROW_OFFS;
const int MASK_COLS = COLS - MASK_COL_OFFS - MASK_COL_OFFS;
const int MASK_SLICES = SLICES - MASK_SLICE_OFFS - MASK_SLICE_OFFS;

#define LIB_SETS_OFFSET
#define USE_VTKINRIA_LAYER

vtkImageData*  InitializeInputImage()
{
	vtkImageData* pImage = vtkImageData::New();
	pImage->SetDimensions(COLS,ROWS,SLICES);
	pImage->AllocateScalars(VTK_UNSIGNED_SHORT, 1);
	pImage->SetSpacing(0.5,0.5,0.5);

	short* pDest = static_cast<short*>(pImage->GetScalarPointer());

	short val =0;
	for(quint16 k=0; k < SLICES;++k) {
		val = 0;
		for(quint16 j=0; j < ROWS;++j) {

			if ( (j % 64) == 0 ) {
				val += 16;
				if (val > 512) {
					val = 0;
				}
			}

			for(quint16 i=0; i < COLS;++i) {
				*pDest++ = val; 
			}
		}
	}

	return pImage;
}

vtkImageData*  InitializeMaskImage(vtkImageData* pImage)
{
	vtkImageData* retVal = vtkImageData::New();
	if (retVal != NULL) {
		retVal->SetDimensions(MASK_COLS,MASK_ROWS,MASK_SLICES);
		retVal->AllocateScalars(VTK_UNSIGNED_CHAR, 1);

		char* pDest = static_cast<char*>(retVal->GetScalarPointer());

		char sliceVal =0;
		for(quint16 k=0; k < MASK_SLICES;++k) {
			
			if ((k> 128) && (k < 256)) {

				int nColor = ((k - 128) / 22) + 1;
				sliceVal = nColor;
			}
			else {
				sliceVal = 0;
			}

			char rowVal = sliceVal;
			for(quint16 j=0; j < MASK_ROWS;++j) {
				if ((sliceVal != 0) && (j > 128) && (j < 256) ) {

					int nColor = ((j - 128) / 22);
					rowVal = ((sliceVal + nColor) % 6) +1;
				}
				else
				{
					rowVal = 0;
				}

				for(quint16 i=0; i < MASK_COLS;++i) {
					if ( (rowVal != 0) && (i > 128) && (i < 256) ) {
						int nColor = ((i - 128) / 22);
						*pDest++ = ((rowVal + nColor) % 6) +1;
					}
					else
					{
						*pDest++ = 0;
					}
				}
			}
		}
		if (pImage != NULL) {

			double spacing[3];
			pImage->GetSpacing(spacing);
			retVal->SetSpacing(spacing);

#ifndef LIB_SETS_OFFSET
			double spacing[3];
			pImage->GetSpacing(spacing);
			retVal->SetSpacing(spacing);

			double origin[3];
			pImage->GetOrigin(origin);
			origin[0] += spacing[0] * MASK_COL_OFFS;
			origin[1] += spacing[1] * MASK_ROW_OFFS;
			origin[2] += spacing[2] * MASK_SLICE_OFFS;
			retVal->SetOrigin(origin);

			//retVal->Update();
#endif // ndef LIB_SETS_OFFSET

		}
	}
	//retVal->Update();

	return retVal;
}

lavtkViewImage2D* InitializeView( vtkImageData* pImage, unsigned int orientation )
{
	lavtkViewImage2D* pView = lavtkViewImage2D::New();
	pView->SetImage(pImage);
	pView->SetOrientation(orientation);
	pView->Reset();

	vtkRenderWindow* pren = pView->GetRenderWindow();
	pren->Render();
	return pView;
}

lavtkViewImage2D* InitializeView(vtkAlgorithmOutput* pImage, unsigned int orientation)
{
	lavtkViewImage2D* pView = lavtkViewImage2D::New();
	pView->SetInput(pImage);
	pView->SetOrientation(orientation);
	pView->Reset();

	vtkRenderWindow* pren = pView->GetRenderWindow();
	pren->Render();
	return pView;
}

/////////////////////////////////////////////////////////////////////////////////////////

vtkAlgorithmOutput* getImageAsAlgorithm(vtkImageData* pImage)
{
	auto pSM = vtkImageChangeInformation::New();
	pSM->SetInformation(pImage->GetInformation());
	pSM->SetInputDataObject(pImage);

 	pSM->Update();

	return pSM->GetOutputPort();
}

/////////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char *argv[])
{

	QApplication app(argc, argv);

	QMainWindow mainWindow;
	mainWindow.setGeometry(100, 100, 700, 700);

	vtkImageData* pImage = InitializeInputImage();

	vtkAlgorithmOutput* pImageAlgorithm = getImageAsAlgorithm(pImage);

	lavtkViewImage2D* pViews[3];

	pViews[0] = InitializeView(pImageAlgorithm,laSliceOrientation::Sagittal);
	pViews[1] = InitializeView(pImageAlgorithm,laSliceOrientation::Coronal);
	pViews[1]->AddSiblingView(pViews[0]);
	pViews[2] = InitializeView(pImageAlgorithm,laSliceOrientation::Axial);
	pViews[2]->AddSiblingView(pViews[1]);
	pViews[0]->AddSiblingView(pViews[2]);


	for (auto pView : pViews) {
		pView->SetSize(425, 425);
	}

	pViews[0]->SetPosition(0, 0);
	pViews[1]->SetPosition(0, 450);
	pViews[2]->SetPosition(425, 0); pViews[2]->SetSize(900, 875);

	vtkImageData* pMaskImage = InitializeMaskImage(pImage);

	vtkLookupTable* lut = vtkLookupTable::New(); 
	lut->SetNumberOfTableValues(7); 
	lut->SetRange(0.0,6.0); 
	lut->SetTableValue( 0, 0.0, 0.0, 0.0, 0.0 ); //label 0 is transparent 
	lut->SetTableValue( 1, 0.0, 1.0, 0.0, 0.8 ); //label 1 is green 
	lut->SetTableValue( 2, 1.0, 0.0, 0.0, 0.3 ); //label 2 is red 
	lut->SetTableValue( 3, 0.0, 0.0, 1.0, 0.3 ); //label 3 is blue
	lut->SetTableValue( 4, 0.0, 1.0, 1.0, 0.3 ); //label 4 is cyan 
	lut->SetTableValue( 5, 1.0, 1.0, 0.0, 0.3 ); //label 5
	lut->SetTableValue( 6, 1.0, 0.0, 1.0, 0.3 ); //label 6
	lut->Build(); 


 	vtkAlgorithmOutput* pMaskAlgorithm = getImageAsAlgorithm(pMaskImage);

	double spacing[3];
	pImage->GetSpacing(spacing);

	auto pChangeInfo = vtkImageChangeInformation::New();
	pChangeInfo->SetInputConnection(pMaskAlgorithm);
	pChangeInfo->SetExtentTranslation(MASK_COL_OFFS, MASK_ROW_OFFS, MASK_SLICE_OFFS);
	pChangeInfo->SetOriginTranslation(-MASK_COL_OFFS * spacing[0], -MASK_ROW_OFFS * spacing[1], -MASK_SLICE_OFFS * spacing[2]);

	auto pPadFilter = vtkImageConstantPad::New();

	pPadFilter->SetInputConnection(pChangeInfo->GetOutputPort());
	int extent[6];
	pImage->GetExtent(extent);
	pPadFilter->SetOutputWholeExtent(extent);

	auto pChangeInfo2 = vtkImageChangeInformation::New();
	pChangeInfo2->SetInputConnection(pPadFilter->GetOutputPort());
	//pChangeInfo2->SetExtentTranslation(MASK_COL_OFFS, MASK_ROW_OFFS, MASK_SLICE_OFFS);
	pChangeInfo2->SetOriginTranslation(+MASK_COL_OFFS * spacing[0], +MASK_ROW_OFFS * spacing[1], +MASK_SLICE_OFFS * spacing[2]);
	//pPadFilter->Update();

#ifdef USE_VTKINRIA_LAYER
	vtkImageMapToColors* windowLevel = vtkImageMapToColors::New();
	windowLevel->SetLookupTable(lut);
	windowLevel->SetInputConnection(pChangeInfo2->GetOutputPort());
	windowLevel->PassAlphaToOutputOn();
	windowLevel->Update();
#endif

	for(int i=0; i < 3;++i) {

//#ifndef LIB_SETS_OFFSET
		//pViews[i]->SetFGImage(pMaskImage,lut);
//#else

#ifndef USE_VTKINRIA_LAYER
		//auto pSL = vtkImageSliceMapper::New();
		//pSL->SetInputData(pMaskImage);

		pViews[i]->SetFGImage(pMaskAlgorithm,lut,MASK_COL_OFFS,MASK_ROW_OFFS,MASK_SLICE_OFFS);
		pViews[i]->Render();
#else
		pViews[i]->SetInput(windowLevel->GetOutputPort(), pViews[i]->GetOrientationMatrix(), lavtkViewImage2D::FG_IMAGE_ACTOR);
#endif // def USE_VTKINRIA_LAYER
// 		auto pActor = pViews[i]->GetImageActor(lavtkViewImage2D::FG_IMAGE_ACTOR);
// 
// 		pActor->SetOpacity(0.95);

//#endif //ndef LIB_SETS_OFFSET

	}
	
	return app.exec();

}
