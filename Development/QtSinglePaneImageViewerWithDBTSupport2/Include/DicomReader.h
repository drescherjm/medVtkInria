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
	bool CanReadFile();
	bool Read();
	bool isMultiframeDicom();

	boost::optional<std::pair<double, double>> getDefaultWindowLevel();

	std::string GetViewCodeSequence();
	std::string GetImageLaterality();
	bool		isAnatomicRegionBreast();

private:
	class Private;
	std::unique_ptr<Private> m_pPrivate;
};

/////////////////////////////////////////////////////////////////////////////////////////

#endif // DICOMREADER_H
