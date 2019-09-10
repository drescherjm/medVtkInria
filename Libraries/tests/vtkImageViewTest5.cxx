/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: SynchronizedViewsTest.cxx 1211 2009-07-29 14:57:22Z filus $
Language:  C++
Author:    $Author: filus $
Date:      $Date: 2009-07-29 15:57:22 +0100 (Wed, 29 Jul 2009) $
Version:   $Revision: 1211 $

Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
See Copyright.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include <vtkImageView2D.h>
#include <vtkImageView3D.h>
//#include <vtkImageViewCollection.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkImageEllipsoidSource.h>
#include "vtkInteractorStyleImageView2D.h"
#include "vtkImageViewCornerAnnotation.h"
#include "vtkMath.h"

#include <time.h>
#include <cstdlib>
#include <vector>


int vtkImageViewTest5(int argc, char* argv[])
{

  vtkImageView2D* view1 = vtkImageView2D::New();
  vtkRenderWindowInteractor* iren1 = vtkRenderWindowInteractor::New();
  vtkRenderWindow* rwin1 = vtkRenderWindow::New();
  vtkRenderer* renderer1 = vtkRenderer::New();


  iren1->SetRenderWindow (rwin1);


  rwin1->AddRenderer (renderer1);
  
  view1->SetRenderWindow ( rwin1 );
  

  view1->SetRenderer ( renderer1 );
 
  
  view1->SetSliceOrientation (vtkImageView2D::SLICE_ORIENTATION_XY);

  vtkImageEllipsoidSource* imageSource = vtkImageEllipsoidSource::New();
  imageSource->SetWholeExtent (0, 127, 0, 127, 0, 127);
  imageSource->SetCenter (64,64,64);
  imageSource->SetRadius (64,32,32);
  imageSource->Update();

  //view1->SetInputConnection(imageSource->GetOutputPort());

  view1->SetInput(imageSource->GetOutputPort());
  
  view1->ShowRulerWidgetOff();

  rwin1->Render();

  iren1->Start(); 

  
  view1->Delete();
 

  iren1->Delete();
 

  rwin1->Delete();
 
  renderer1->Delete();
  
  imageSource->Delete();


  return 0;
}
