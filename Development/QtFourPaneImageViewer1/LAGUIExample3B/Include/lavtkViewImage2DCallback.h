#pragma once

#ifndef LAVTKVIEWIMAGE2DCALLBACK_H
#define LAVTKVIEWIMAGE2DCALLBACK_H

#include "vtkCommand.h"
#include "vtkObject.h"

class lavtkViewImage2D;

/////////////////////////////////////////////////////////////////////////////////////////

class lavtkViewImage2DCallback : public vtkCommand
{

 public:
  
  static lavtkViewImage2DCallback *New()
  { return new lavtkViewImage2DCallback; }

  
  virtual void Execute (vtkObject* caller, unsigned long, void*);
  
  void SetView ( lavtkViewImage2D* view);
  
  lavtkViewImage2D* GetView (void) const
  {
    return this->View;
  }

  protected:
 
  lavtkViewImage2DCallback();
  ~lavtkViewImage2DCallback();

  void OnZSliceMoveEvent(vtkObject* caller);
  void OnEndInteractionEvent(vtkObject* caller);
  void OnKeyPressEvent (vtkObject* caller, unsigned char);
 
 private:
  lavtkViewImage2DCallback (const lavtkViewImage2DCallback&);
  void operator=(const lavtkViewImage2DCallback&);
  
  lavtkViewImage2D*  View;
  
};

/////////////////////////////////////////////////////////////////////////////////////////

#endif // LAVTKVIEWIMAGE2DCALLBACK_H
