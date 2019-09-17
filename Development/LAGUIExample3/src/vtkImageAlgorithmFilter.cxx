#include "vtkImageAlgorithmFilter.h"

#include <vtkImageData.h>
#include <vtkObjectFactory.h>
#include <vtkStreamingDemandDrivenPipeline.h>
#include <vtkInformationVector.h>
#include <vtkInformation.h>
#include <vtkDataObject.h>
#include <vtkSmartPointer.h>

vtkStandardNewMacro(vtkImageAlgorithmFilter);

int vtkImageAlgorithmFilter::RequestData(vtkInformation *vtkNotUsed(request),
                                             vtkInformationVector **inputVector,
                                             vtkInformationVector *outputVector)
{
  // Get the info objects
  vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
  vtkInformation *outInfo = outputVector->GetInformationObject(0);
  
  // Get the input and output
  vtkImageData *input = dynamic_cast<vtkImageData*>(
      inInfo->Get(vtkDataObject::DATA_OBJECT()));
  
  vtkImageData *output = dynamic_cast<vtkImageData*>(
      outInfo->Get(vtkDataObject::DATA_OBJECT()));
     
  output->ShallowCopy(input);

  // Without these lines, the output will appear real but will not work as the input to any other filters
  int extent[6];
  input->GetExtent(extent);
  output->SetExtent(extent);
  outInfo->Set(vtkStreamingDemandDrivenPipeline::UPDATE_EXTENT(),
               extent,
               6);
  outInfo->Set(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT(),
               extent,
               6);
  return 1;
}
