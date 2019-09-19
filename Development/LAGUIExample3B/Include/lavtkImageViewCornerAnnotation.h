#pragma once

#ifndef LAVTKIMAGEVIEWCORNERANNOTATION_H
#define LAVTKIMAGEVIEWCORNERANNOTATION_H

#include "vtkImageViewCornerAnnotationEx.h"

/////////////////////////////////////////////////////////////////////////////////////////

class  lavtkImageViewCornerAnnotation : public vtkImageViewCornerAnnotationEx
{

 public:
  vtkTypeMacro(lavtkImageViewCornerAnnotation, vtkImageViewCornerAnnotationEx);
  static lavtkImageViewCornerAnnotation *New();

 protected:
  lavtkImageViewCornerAnnotation();
  ~lavtkImageViewCornerAnnotation();

   virtual std::string replaceStrings(vtkImageActor *ia,
	  vtkImageMapToWindowLevelColors *wl,
	  std::string str);
  
 private:
  lavtkImageViewCornerAnnotation(const lavtkImageViewCornerAnnotation&);  // Not implemented.
  void operator=(const lavtkImageViewCornerAnnotation&);  // Not implemented.
};

/////////////////////////////////////////////////////////////////////////////////////////

#endif // LAVTKIMAGEVIEWCORNERANNOTATION_H
