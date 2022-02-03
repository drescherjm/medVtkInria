#include "DicomReader.h"
#include <vtkSmartPointer.h>

#ifndef VTK_DICOM_MODULE
#include <dicom/vtkDICOMReader.h>
#include <dicom/vtkDICOMMetaData.h>
#include <dicom/vtkDICOMItem.h>
#else
#include <X:\x64.20\VC.142\Install\Libraries\VTK-7.1.1\include\vtk-7.1\vtkDICOMReader.h>
#endif

/////////////////////////////////////////////////////////////////////////////////////////

class DicomReader::Private
{
public:
	vtkSmartPointer< vtkDICOMReader > reader;
	vtkSmartPointer<vtkDICOMMetaData> meta = vtkSmartPointer<vtkDICOMMetaData>::New();
};

/////////////////////////////////////////////////////////////////////////////////////////

DicomReader::DicomReader(std::string strFileName) : m_pPrivate{std::make_unique<Private>()}
{
	m_pPrivate->reader = vtkSmartPointer< vtkDICOMReader >::New();
	m_pPrivate->reader->SetFileName(strFileName.c_str());
	m_pPrivate->reader->Update();

	m_pPrivate->meta = m_pPrivate->reader->GetMetaData();
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

bool DicomReader::isMultiframeDicom()
{
	return m_pPrivate->meta->Has(DC::SharedFunctionalGroupsSequence);
}

/////////////////////////////////////////////////////////////////////////////////////////

boost::optional<std::pair<double, double>> DicomReader::getDefaultWindow()
{
	boost::optional<std::pair<double, double>> retVal;

	if (isMultiframeDicom()) {
		int fileIndex = 0;
		auto value = m_pPrivate->meta->Get(fileIndex, DC::SharedFunctionalGroupsSequence);

		auto seqShared = value.GetSequenceData();
		auto valueWL = seqShared->Get(DC::FrameVOILUTSequence);
		if (valueWL.IsValid()) {
			auto seqWL = valueWL.GetSequenceData();
			auto windowWidth = seqWL->Get(DC::WindowWidth);
			auto windowCenter = seqWL->Get(DC::WindowCenter);
			if (windowWidth.IsValid() && windowCenter.IsValid()) {
				retVal = std::make_pair(windowWidth.AsDouble(), windowCenter.AsDouble());
			}
		}
	}
	else {
		auto windowWidth = m_pPrivate->meta->Get(DC::WindowWidth);
		auto windowCenter = m_pPrivate->meta->Get(DC::WindowCenter);
		if (windowWidth.IsValid() && windowCenter.IsValid()) {
			retVal = std::make_pair(windowWidth.AsDouble(), windowCenter.AsDouble());
		}
	}

	return retVal;
}

/////////////////////////////////////////////////////////////////////////////////////////

