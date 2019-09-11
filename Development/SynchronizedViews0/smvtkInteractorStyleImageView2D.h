/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: vtkInteractorStyleImageView2D.h 1223 2009-08-06 07:21:42Z filus $
Language:  C++
Author:    $Author: filus $
Date:      $Date: 2009-08-06 09:21:42 +0200 (jeu, 06 aoû 2009) $
Version:   $Revision: 1223 $

Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
See Copyright.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#pragma once

#ifndef SMVTKINTERACTORSTYLEIMAGEVIEW2D_H
#define SMVTKINTERACTORSTYLEIMAGEVIEW2D_H

#include <vtkInteractorStyleImageView2D.h>
#include <QSharedPointer>

class smvtkInteractorEvent;

/////////////////////////////////////////////////////////////////////////////////////////

class smvtkInteractorStyleImageView2D : public vtkInteractorStyleImageView2D
{
	typedef vtkInteractorStyleImageView2D Superclass;
 public:
  static smvtkInteractorStyleImageView2D *New();
  vtkTypeMacro (smvtkInteractorStyleImageView2D, vtkInteractorStyleImageView2D);
   
  //BTX
  enum InteractionTypeIdsEx
  {
    InteractionTypeExBegin = InteractionTypePan,
	
  };
  //ETX

  vtkSetClampMacro (LeftButtonInteraction, int, InteractionTypeNull, InteractionTypePan);
  vtkGetMacro (LeftButtonInteraction, int);
  vtkSetClampMacro (RightButtonInteraction, int, InteractionTypeNull, InteractionTypePan);
  vtkGetMacro (RightButtonInteraction, int);
  vtkSetClampMacro (MiddleButtonInteraction, int, InteractionTypeNull, InteractionTypePan);
  vtkGetMacro (MiddleButtonInteraction, int);  
  vtkSetClampMacro (WheelButtonInteraction, int, InteractionTypeNull, InteractionTypePan);
  vtkGetMacro (WheelButtonInteraction, int);  


  void	GrabFocus();
  
  virtual void updateRequiestedPosition();
  virtual bool updateCurrentRenderer();
  virtual void OnMouseMove();
  virtual void OnLeftButtonDown();
  virtual void OnLeftButtonUp();
  virtual void OnMiddleButtonDown();
  virtual void OnMiddleButtonUp();
  virtual void OnRightButtonDown();
  virtual void OnRightButtonUp();
  virtual void OnMouseWheelForward();
  virtual void OnMouseWheelBackward();

  virtual bool OnWindowLevelInteractionDown();
  virtual bool OnWindowLevelInteractionUp();
  virtual bool OnWindowLevelInteractionMove();
  virtual void OnZoomInteractionDown();
  virtual void OnZoomInteractionUp();
  virtual void OnPanInteractionDown();
  virtual void OnPanInteractionUp();

  virtual void OnChar();  
  virtual void OnKeyDown();
  virtual void OnKeyUp();

  virtual bool OnSliceMoveInteractionDown();
  virtual bool OnSliceMoveInteractionUp();
  virtual void StartSliceMove();
  virtual void SliceMove();
  virtual void EndSliceMove();

  virtual void DefaultMoveAction();
  
  vtkGetMacro (SliceStep, int);
  vtkGetVector3Macro (RequestedPosition, int);

  virtual bool addEventHandler(QSharedPointer<smvtkInteractorEvent> pEvent);
  
  
 protected:
  smvtkInteractorStyleImageView2D();
  ~smvtkInteractorStyleImageView2D();

private:
	class smPrivate;
	smPrivate* m_pPrivate;

 private: 
  smvtkInteractorStyleImageView2D(const smvtkInteractorStyleImageView2D&);  // Intentionally not implemented.
  void operator=(const smvtkInteractorStyleImageView2D&);					// Intentionally not implemented.
  
};

/////////////////////////////////////////////////////////////////////////////////////////

#endif // SMVTKINTERACTORSTYLEIMAGEVIEW2D_H
