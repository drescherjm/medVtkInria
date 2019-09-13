#pragma once

#ifndef VTKIMAGEVIEWCORNERANNOTATIONEX_H
#define VTKIMAGEVIEWCORNERANNOTATIONEX_H

#include "vtkImageViewCornerAnnotation.h"

class vtkImageView;

/**
   Notes on Nicolas Toussaint changes

   A) RenderOpaqueGeometry()
   we actually absolutely need to re-write this method in order
   to update properly the information as the mouse move over the window.
   
*/

/**
   flags :

   
*/

/////////////////////////////////////////////////////////////////////////////////////////

class MEDVTKINRIA_EXPORT vtkImageViewCornerAnnotationEx : public vtkImageViewCornerAnnotation
{

 public:

  vtkTypeMacro(vtkImageViewCornerAnnotationEx, vtkImageViewCornerAnnotation);

  static vtkImageViewCornerAnnotationEx *New();

 protected:
  vtkImageViewCornerAnnotationEx();
  ~vtkImageViewCornerAnnotationEx();

  virtual void TextReplace( vtkImageActor *ia, vtkImageMapToWindowLevelColors *wl);

  virtual std::string replaceStrings(vtkImageActor *ia,
	  vtkImageMapToWindowLevelColors *wl,
	  std::string str);
  
 private:
  vtkImageViewCornerAnnotationEx(const vtkImageViewCornerAnnotationEx&);  // Not implemented.
  void operator=(const vtkImageViewCornerAnnotationEx&);  // Not implemented.
};

/////////////////////////////////////////////////////////////////////////////////////////

#endif // VTKIMAGEVIEWCORNERANNOTATIONEX_H
