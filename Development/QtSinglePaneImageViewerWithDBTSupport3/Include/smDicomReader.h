#pragma once

#ifndef SMDICOMREADER_H
#define SMDICOMREADER_H

/////////////////////////////////////////////////////////////////////////////////////////

#include <string>
#include <memory>
#include <utility>
#include <vector>
#include <boost/optional.hpp>

/////////////////////////////////////////////////////////////////////////////////////////

class vtkImageData;
class vtkAlgorithmOutput;
class vtkMatrix4x4;
class vtkObject;
class vtkDICOMMetaData;

/////////////////////////////////////////////////////////////////////////////////////////

class smDicomReader
{
public:
	using VecFileNames = std::vector<std::string>;
	smDicomReader(const VecFileNames& strFileNames);
	smDicomReader(const std::string& strFileName);
	virtual ~smDicomReader();

public:
	vtkImageData* GetOutput();
	vtkAlgorithmOutput* GetOutputPort();
	vtkObject* GetReader();

public:
	bool CanReadFile() const;
	void ReadDicomMetaData();
	bool Read();
	bool isMultiframeDicom() const;
	void setFlipZ(bool bFlip);
	void Print(std::ostream& os);
	
	vtkDICOMMetaData* GetMetaData() const;

	boost::optional<std::pair<double, double>> getDefaultWindowLevel() const;

	std::string				GetViewCodeSequence() const;
	std::string				GetMammographyACR_MQCM_CodeFromViewCodeSequence(std::string strViewCode) const;
	std::string				GetImageLaterality() const;
	std::string				GetAnatomicRegion() const;
	std::string				GetBodyPartExamined() const;
	std::string				GetOrganExposed() const;
	std::string				GetImageOrientationPatientString() const;
	std::string				GetPatientOrientation() const;
	std::string				GetSOPClassUID() const;
	boost::optional<int>	GetImagesInAcquisition() const;

	std::vector<double>		GetImageOrientationPatientVector() const;
	bool					isAnatomicRegionBreast() const;
	bool					isImageLateralityLeft(std::string strLaterality = std::string{}) const;
	bool					isImageLateralityRight(std::string strLaterality = std::string{}) const;

	vtkMatrix4x4*			GetPatientMatrix() const;

private:
	class Private;
	std::unique_ptr<Private> m_pPrivate;
};

/////////////////////////////////////////////////////////////////////////////////////////

#endif // SMDICOMREADER_H
