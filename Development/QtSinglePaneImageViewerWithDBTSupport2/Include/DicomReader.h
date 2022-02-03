#pragma once

#ifndef DICOMREADER_H
#define DICOMREADER_H

#include <string>
#include <memory>

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

private:
	class Private;
	std::unique_ptr<Private> m_pPrivate;
};

/////////////////////////////////////////////////////////////////////////////////////////

#endif // DICOMREADER_H
