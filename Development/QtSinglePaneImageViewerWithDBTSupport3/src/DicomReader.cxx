#include "DicomReader.h"
#include <vtkSmartPointer.h>
#include <vtkStringArray.h>
#include <map>

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
#include "FunctionProfiler.h"

/////////////////////////////////////////////////////////////////////////////////////////

class DicomReader::Private
{
public:
	bool updateWindowAndLevelForMultiframe(vtkMedicalImageProperties* pProps);

public:
	bool m_bFlipZ{ false };
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

bool DicomReader::CanReadFile() const
{
	return m_pPrivate->reader->CanReadFile(m_pPrivate->reader->GetFileName());
}

/////////////////////////////////////////////////////////////////////////////////////////

void DicomReader::UpdateInformation()
{
	PROFILE_THIS_FUNCTION;

	m_pPrivate->reader->UpdateInformation();

	m_pPrivate->meta = m_pPrivate->reader->GetMetaData();

	m_pPrivate->meta->Print(std::cout);

	m_pPrivate->reader->Update();
	auto pProps = m_pPrivate->reader->GetMedicalImageProperties();

	if (isMultiframeDicom()) {
		m_pPrivate->updateWindowAndLevelForMultiframe(pProps);
	}

	pProps->Print(std::cout);
}

/////////////////////////////////////////////////////////////////////////////////////////

bool DicomReader::Read()
{
	PROFILE_THIS_FUNCTION;

	if (m_pPrivate->m_bFlipZ) {
		m_pPrivate->reader->SetMemoryRowOrderToFileNative();
	}
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

bool DicomReader::isMultiframeDicom() const
{
	return m_pPrivate->meta->Has(DC::SharedFunctionalGroupsSequence);
}

/////////////////////////////////////////////////////////////////////////////////////////

void DicomReader::setFlipZ(bool bFlip)
{
	m_pPrivate->m_bFlipZ = bFlip;
}

/////////////////////////////////////////////////////////////////////////////////////////

boost::optional<std::pair<double, double>> DicomReader::getDefaultWindowLevel() const
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

std::string DicomReader::GetViewCodeSequence() const
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

std::string DicomReader::GetMammographyACR_MQCM_CodeFromViewCodeSequence(std::string strViewCode) const
{
	// See: https://dicom.nema.org/dicom/2013/output/chtml/part16/sect_CID_4014.html

	static std::map<std::string, std::string> mapCodes{ 
		{"R-10224","ML"},
		{"R-10226","MLO"},
		{"R-10228","LM"},
		{"R-10230","LMO"},
		{"R-10242","CC"},
		{"R-10244","FB"},
		{"R-40AAA","ISO"},
		{"R-102D0","SIO"},
		{"R-1024A","XCCL"},
		{"Y-X1770","XCCL"},
		{"R-1024B","XCCM"},
		{"Y-X1771","XCCM"},
		{"R-102C2","TAN"},
	};
	
	std::string retVal;

	auto it = mapCodes.find(strViewCode);

	if (it != mapCodes.end()) {
		retVal = it->second;
	}
	else if (!strViewCode.empty()) {
		retVal = strViewCode;
	}

	return retVal;
}

/////////////////////////////////////////////////////////////////////////////////////////

std::string DicomReader::GetImageLaterality() const
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

std::string DicomReader::GetAnatomicRegion() const
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

		auto valueSFGS = m_pPrivate->meta->Get(DC::SharedFunctionalGroupsSequence);
		if (valueSFGS.IsValid()) {
			auto value = valueSFGS.GetSequenceData()->Get(
				vtkDICOMTagPath(DC::FrameAnatomySequence, 0,
					DC::AnatomicRegionSequence, 0,
					DC::CodeMeaning)
			);

			if (value.IsValid()) {
				retVal = value.AsString();
			}
		}
	}
	else {
		auto value = m_pPrivate->meta->Get(
			vtkDICOMTagPath(DC::AnatomicRegionSequence, 0,
				DC::CodeMeaning)
		);

		if (value.IsValid()) {
			retVal = value.AsString();
		}
	}

	return retVal;
}

/////////////////////////////////////////////////////////////////////////////////////////

std::string DicomReader::GetImageOrientationPatientString() const
{
	/*
	*   
	(0020,9116) SQ "PlaneOrientationSequence" : (1 item)
		---- SQ Item 0001 at offset 14630 ----
		(0020,0037) DS "ImageOrientationPatient" : [0\-1\0\0.003\0\-1.000] (22 bytes)
	*/

	std::string retVal;

	if (isMultiframeDicom()) {
		auto value = m_pPrivate->meta->Get(
			vtkDICOMTagPath(DC::SharedFunctionalGroupsSequence,0,
				DC::PlaneOrientationSequence, 0,
				DC::ImageOrientationPatient)
		);

		if (value.IsValid()) {
			retVal = value.AsString();
		}
	}
	else {
		auto value = m_pPrivate->meta->Get(DC::ImageOrientationPatient);
	
		if (value.IsValid()) {
			retVal = value.AsString();
		}
	}
	return retVal;
}

/////////////////////////////////////////////////////////////////////////////////////////

std::vector<double> DicomReader::GetImageOrientationPatientVector() const
{
	/*
	*   
	(0020,9116) SQ "PlaneOrientationSequence" : (1 item)
		---- SQ Item 0001 at offset 14630 ----
		(0020,0037) DS "ImageOrientationPatient" : [0\-1\0\0.003\0\-1.000] (22 bytes)
	*/

	std::vector<double> retVal;

	if (isMultiframeDicom()) {
		auto value = m_pPrivate->meta->Get(
			vtkDICOMTagPath(DC::SharedFunctionalGroupsSequence,0,
				DC::PlaneOrientationSequence, 0,
				DC::ImageOrientationPatient)
		);

		if (value.IsValid()) {
			auto size = value.GetNumberOfValues();
			if (size > 0) {
				retVal.resize(size);
				value.GetValues(retVal.data(), size);
			}
		}
	}
	else {
		auto value = m_pPrivate->meta->Get(DC::ImageOrientationPatient);
	
		if (value.IsValid()) {
			auto size = value.GetNumberOfValues();
			if (size > 0) {
				retVal.resize(size);
				value.GetValues(retVal.data(), size);
			}
		}
	}
	return retVal;
}


/////////////////////////////////////////////////////////////////////////////////////////

bool DicomReader::isAnatomicRegionBreast() const
{
	auto strAnatomicRegion = GetAnatomicRegion();
	return (boost::iequals(strAnatomicRegion, "Breast"));
}

/////////////////////////////////////////////////////////////////////////////////////////

bool DicomReader::isImageLateralityLeft(std::string strLaterality) const
{
	bool retVal{ false };

	if (strLaterality.empty()) {
		strLaterality = GetImageLaterality();
	}

	if (!strLaterality.empty()) {
		retVal = ((strLaterality[0] == 'L') || (strLaterality[0] == 'l'));
	}

	return retVal;
}


/////////////////////////////////////////////////////////////////////////////////////////

bool DicomReader::isImageLateralityRight(std::string strLaterality) const
{
	bool retVal{ false };

	if (strLaterality.empty()) {
		strLaterality = GetImageLaterality();
	}

	if (!strLaterality.empty()) {
		retVal = ((strLaterality[0] == 'R') || (strLaterality[0] == 'r'));
	}

	return retVal;
}

/////////////////////////////////////////////////////////////////////////////////////////

vtkMatrix4x4* DicomReader::GetPatientMatrix() const
{
	return m_pPrivate->reader->GetPatientMatrix();
}

/////////////////////////////////////////////////////////////////////////////////////////
