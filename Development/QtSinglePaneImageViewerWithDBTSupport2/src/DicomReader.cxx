#include "DicomReader.h"
#include <vtkSmartPointer.h>

#include <boost/algorithm/string/predicate.hpp>

#ifndef VTK_DICOM_MODULE
#include <dicom/vtkDICOMReader.h>
#include <dicom/vtkDICOMMetaData.h>
#include <dicom/vtkDICOMItem.h>
#include <vtkMedicalImageProperties.h>
#include <vtkDICOMTagPath.h>
#else
#include <X:\x64.20\VC.142\Install\Libraries\VTK-7.1.1\include\vtk-7.1\vtkDICOMReader.h>
#endif

/////////////////////////////////////////////////////////////////////////////////////////

class DicomReader::Private
{
public:
	bool updateWindowAndLevelForMultiframe(vtkMedicalImageProperties* pProps);

public:
	vtkSmartPointer< vtkDICOMReader > reader;
	vtkSmartPointer<vtkDICOMMetaData> meta = vtkSmartPointer<vtkDICOMMetaData>::New();
};

/////////////////////////////////////////////////////////////////////////////////////////

bool DicomReader::Private::updateWindowAndLevelForMultiframe(vtkMedicalImageProperties* pProps)
{
	bool retVal{ false };
	int fileIndex = 0;
	auto value = meta->Get(fileIndex, DC::SharedFunctionalGroupsSequence);
	retVal = value.IsValid();
	
	if (retVal) {
		auto seqShared = value.GetSequenceData();
		auto valueWL = seqShared->Get(DC::FrameVOILUTSequence);
		retVal = valueWL.IsValid();
		if (retVal) {
			auto seqWL = valueWL.GetSequenceData();
			auto windowWidth = seqWL->Get(DC::WindowWidth);
			auto windowCenter = seqWL->Get(DC::WindowCenter);

			// This code handles the case where there are multiple WL presets
			// The code is based on the code from the github source: 
			// https://github.com/dgobbi/vtk-dicom/blob/master/Source/vtkDICOMReader.cxx#L2514
			
			retVal = (windowWidth.IsValid() && windowCenter.IsValid());
			if (retVal) {
				int n = static_cast<int>(windowCenter.GetNumberOfValues());
				int m = static_cast<int>(windowWidth.GetNumberOfValues());
				n = (m < n ? m : n);
				pProps->RemoveAllWindowLevelPresets();
				for (int i = 0; i < n; i++)
				{
					pProps->AddWindowLevelPreset(
						windowCenter.GetDouble(i), windowWidth.GetDouble(i));
				}

				const vtkDICOMValue& comment = seqWL->Get(DC::WindowCenterWidthExplanation);
				m = static_cast<int>(comment.GetNumberOfValues());
				m = (n < m ? n : m);
				for (int j = 0; j < m; j++)
				{
					pProps->SetNthWindowLevelPresetComment(
						j, comment.GetString(j).c_str());
				}
			}
		}
	}

	return retVal;
}

/////////////////////////////////////////////////////////////////////////////////////////

DicomReader::DicomReader(std::string strFileName) : m_pPrivate{std::make_unique<Private>()}
{
	m_pPrivate->reader = vtkSmartPointer< vtkDICOMReader >::New();
	m_pPrivate->reader->SetFileName(strFileName.c_str());
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

bool DicomReader::CanReadFile()
{
	return m_pPrivate->reader->CanReadFile(m_pPrivate->reader->GetFileName());
}

/////////////////////////////////////////////////////////////////////////////////////////

bool DicomReader::Read()
{
	m_pPrivate->reader->Update();
	auto pProps = m_pPrivate->reader->GetMedicalImageProperties();

	m_pPrivate->meta = m_pPrivate->reader->GetMetaData();

	if (isMultiframeDicom()) {
		m_pPrivate->updateWindowAndLevelForMultiframe(pProps);
	}

	pProps->Print(std::cout);
	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////

bool DicomReader::isMultiframeDicom()
{
	return m_pPrivate->meta->Has(DC::SharedFunctionalGroupsSequence);
}

/////////////////////////////////////////////////////////////////////////////////////////

boost::optional<std::pair<double, double>> DicomReader::getDefaultWindowLevel()
{
	boost::optional<std::pair<double, double>> retVal;

	if (isMultiframeDicom()) {
		auto value = m_pPrivate->meta->Get(DC::SharedFunctionalGroupsSequence);
		if (value.IsValid()) {
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
	}
	else { //Look for the Window and Level in the standard locations
		auto windowWidth = m_pPrivate->meta->Get(DC::WindowWidth);
		auto windowCenter = m_pPrivate->meta->Get(DC::WindowCenter);
		if (windowWidth.IsValid() && windowCenter.IsValid()) {
			retVal = std::make_pair(windowWidth.AsDouble(), windowCenter.AsDouble());
		}
	}

	return retVal;
}

/////////////////////////////////////////////////////////////////////////////////////////

std::string DicomReader::GetViewCodeSequence()
{
	/*
	 *	A Sequence looks like the following
	 (0054,0220) SQ "ViewCodeSequence" : (1 item)
		---- SQ Item 0001 at offset 14106 ----
		(0008,0100) SH "CodeValue" : [R-10228] (8 bytes)
		(0008,0102) SH "CodingSchemeDesignator" : [SNM3] (4 bytes)
		(0008,0104) LO "CodeMeaning" : [latero-medial] (14 bytes)
		(0054,0222) SQ "ViewModifierCodeSequence" : (0 items)
	 */

	std::string retVal;
	auto value = m_pPrivate->meta->Get(DC::ViewCodeSequence);
	if (value.IsValid()) {
		auto seqData = value.GetSequenceData();
		auto viewCode = seqData->Get(DC::CodeValue);
		if (viewCode.IsValid()) {
			retVal = viewCode.AsString();
		}
	}
	return retVal;
}

/////////////////////////////////////////////////////////////////////////////////////////

std::string DicomReader::GetImageLaterality()
{
	std::string retVal;
	if (isMultiframeDicom()) {

		/*
		(0020,9071) SQ "FrameAnatomySequence" : (1 item)
			---- SQ Item 0001 at offset 14538 ----
			(0008,2218) SQ "AnatomicRegionSequence" : (1 item)
				---- SQ Item 0001 at offset 14558 ----
				(0008,0100) SH "CodeValue" : [T-04000] (8 bytes)
				(0008,0102) SH "CodingSchemeDesignator" : [SRT] (4 bytes)
				(0008,0104) LO "CodeMeaning" : [Breast] (6 bytes)
			(0020,9072) CS "FrameLaterality" : [L] (2 bytes)
		*/

		auto value = m_pPrivate->meta->Get(DC::SharedFunctionalGroupsSequence);
		if (value.IsValid()) {
			auto seqShared = value.GetSequenceData();
			auto valueFAS = seqShared->Get(DC::FrameAnatomySequence);
			if (valueFAS.IsValid()) {
				auto valueLaterality = valueFAS.GetSequenceData()->Get(DC::FrameLaterality);
				retVal = valueLaterality.AsString();
			}
		}
	}
	else {
		auto valueImageLaterality = m_pPrivate->meta->Get(DC::ImageLaterality);
		if (valueImageLaterality.IsValid()) {
			retVal = valueImageLaterality.AsString();
		}
	}
	return retVal;
}

/////////////////////////////////////////////////////////////////////////////////////////

bool DicomReader::isAnatomicRegionBreast()
{
	bool retVal = false;

	if (isMultiframeDicom()) {
		/*
		(0020,9071) SQ "FrameAnatomySequence" : (1 item)
			---- SQ Item 0001 at offset 14538 ----
			(0008,2218) SQ "AnatomicRegionSequence" : (1 item)
				---- SQ Item 0001 at offset 14558 ----
				(0008,0100) SH "CodeValue" : [T-04000] (8 bytes)
				(0008,0102) SH "CodingSchemeDesignator" : [SRT] (4 bytes)
				(0008,0104) LO "CodeMeaning" : [Breast] (6 bytes)
			(0020,9072) CS "FrameLaterality" : [L] (2 bytes)
		*/

		// 		auto value = m_pPrivate->meta->Get(DC::SharedFunctionalGroupsSequence);
		// 		if (value.IsValid()) {
		// 			auto seqShared = value.GetSequenceData();
		// 			auto valueFAS = seqShared->Get(DC::FrameAnatomySequence);
		// 			if (valueFAS.IsValid()) {
		// 				auto valueARS = valueFAS.GetSequenceData()->Get()
		// 			}
		// 		}

		auto valueSFGS = m_pPrivate->meta->Get(DC::SharedFunctionalGroupsSequence);
		if (valueSFGS.IsValid()) {
			auto value = valueSFGS.GetSequenceData()->Get(
				vtkDICOMTagPath(DC::FrameAnatomySequence, 0,
					DC::AnatomicRegionSequence, 0,
					DC::CodeMeaning)
			);

			if (value.IsValid()) {
				retVal = (boost::iequals(value.AsString(), "Breast"));
			}
		}
	}
	else {
		auto value = m_pPrivate->meta->Get(
			vtkDICOMTagPath(DC::AnatomicRegionSequence, 0,
				DC::CodeMeaning)
		);

		if (value.IsValid()) {
			retVal = (boost::iequals(value.AsString(), "Breast"));
		}
	}

	return retVal;
}

/////////////////////////////////////////////////////////////////////////////////////////

