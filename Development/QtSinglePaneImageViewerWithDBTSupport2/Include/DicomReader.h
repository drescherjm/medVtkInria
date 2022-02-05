#pragma once

#ifndef DICOMREADER_H
#define DICOMREADER_H

#include <string>
#include <memory>
#include <utility>
#include <boost/optional.hpp>

class vtkImageData;
class vtkAlgorithmOutput;

/////////////////////////////////////////////////////////////////////////////////////////

class DicomReader
{
public:
	DicomReader(std::string strFileName);
	virtual ~DicomReader();

public:
	vtkImageData* GetOutput();
	vtkAlgorithmOutput* GetOutputPort();

public:
	bool CanReadFile() const;
	bool Read();
	bool isMultiframeDicom() const;

	boost::optional<std::pair<double, double>> getDefaultWindowLevel() const;

	std::string GetViewCodeSequence() const;
	std::string GetMammographyACR_MQCM_CodeFromViewCodeSequence(std::string strViewCode) const;
	std::string GetImageLaterality() const;
	std::string GetAnatomicRegion() const;
	std::string GetImageOrientationPatient() const;
	bool		isAnatomicRegionBreast() const;

private:
	class Private;
	std::unique_ptr<Private> m_pPrivate;
};

/////////////////////////////////////////////////////////////////////////////////////////

#endif // DICOMREADER_H
