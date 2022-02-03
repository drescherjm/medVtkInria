#include "DicomReader.h"
#include <vtkSmartPointer.h>

#ifndef VTK_DICOM_MODULE
#include "dicom/vtkDICOMReader.h"
#else
#include <X:\x64.20\VC.142\Install\Libraries\VTK-7.1.1\include\vtk-7.1\vtkDICOMReader.h>
#endif

/////////////////////////////////////////////////////////////////////////////////////////

class DicomReader::Private
{
public:
	vtkSmartPointer< vtkDICOMReader > reader;
};

/////////////////////////////////////////////////////////////////////////////////////////

DicomReader::DicomReader(std::string strFileName) : m_pPrivate{std::make_unique<Private>()}
{
	m_pPrivate->reader = vtkSmartPointer< vtkDICOMReader >::New();
	m_pPrivate->reader->SetFileName(strFileName.c_str());
	m_pPrivate->reader->Update();
}

DicomReader::~DicomReader() = default;

/////////////////////////////////////////////////////////////////////////////////////////

vtkImageData* DicomReader::GetOutput()
{
	return m_pPrivate->reader->GetOutput();
}

/////////////////////////////////////////////////////////////////////////////////////////

vtkAlgorithmOutput* DicomReader::GetOutputPort()
{
	return m_pPrivate->reader->GetOutputPort();
}

/////////////////////////////////////////////////////////////////////////////////////////

