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
#include <vtkImageViewCollection.h>
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
#include "smvtkInteractorStyleImageView2D.h"
#include <QApplication>
#include "smvtkInteractorStyle2D.h"


int main(int argc, char* argv[])
{

  // QT Stuff
  QApplication app( argc, argv );

  vtkImageView2D* view1 = vtkImageView2D::New();
  vtkImageView2D* view2 = vtkImageView2D::New();
  vtkImageView2D* view3 = vtkImageView2D::New();
  vtkImageView3D* view4 = vtkImageView3D::New();

  vtkRenderWindowInteractor* iren1 = vtkRenderWindowInteractor::New();
  vtkRenderWindowInteractor* iren2 = vtkRenderWindowInteractor::New();
  vtkRenderWindowInteractor* iren3 = vtkRenderWindowInteractor::New();
  vtkRenderWindowInteractor* iren4 = vtkRenderWindowInteractor::New();

  smvtkInteractorStyleImageView2D* irenStyle1 = smvtkInteractorStyle2D::New();
  smvtkInteractorStyleImageView2D* irenStyle2 = smvtkInteractorStyle2D::New();
  smvtkInteractorStyleImageView2D* irenStyle3 = smvtkInteractorStyle2D::New();

  view1->SetInteractorStyle(irenStyle1);
  view2->SetInteractorStyle(irenStyle2);
  view3->SetInteractorStyle(irenStyle3);

  iren1->SetInteractorStyle(irenStyle1);
  iren2->SetInteractorStyle(irenStyle2);
  iren3->SetInteractorStyle(irenStyle3);
  
  vtkRenderWindow* rwin1 = vtkRenderWindow::New();
  vtkRenderWindow* rwin2 = vtkRenderWindow::New();
  vtkRenderWindow* rwin3 = vtkRenderWindow::New();
  vtkRenderWindow* rwin4 = vtkRenderWindow::New();

  vtkRenderer* renderer1 = vtkRenderer::New();
  vtkRenderer* renderer2 = vtkRenderer::New();
  vtkRenderer* renderer3 = vtkRenderer::New();
  vtkRenderer* renderer4 = vtkRenderer::New();

  rwin1->SetSize(512, 512);
  rwin2->SetSize(512, 512);
  rwin3->SetSize(512, 512);
  rwin4->SetSize(512, 512);

  iren1->SetRenderWindow (rwin1);
  iren2->SetRenderWindow (rwin2);
  iren3->SetRenderWindow (rwin3);
  iren4->SetRenderWindow (rwin4);

  rwin1->AddRenderer (renderer1);
  rwin2->AddRenderer (renderer2);
  rwin3->AddRenderer (renderer3);
  rwin4->AddRenderer (renderer4);

  view1->SetRenderWindow ( rwin1 );
  view2->SetRenderWindow ( rwin2 );
  view3->SetRenderWindow ( rwin3 );
  view4->SetRenderWindow ( rwin4 );

  view1->SetRenderer ( renderer1 );
  view2->SetRenderer ( renderer2 );
  view3->SetRenderer ( renderer3 );
  view4->SetRenderer ( renderer4 );

  view1->SetCursorFollowMouse(0);
  view2->SetCursorFollowMouse(0);
  view3->SetCursorFollowMouse(0);
  
  view1->SetSliceOrientation (vtkImageView2D::SLICE_ORIENTATION_XY);
  view2->SetSliceOrientation (vtkImageView2D::SLICE_ORIENTATION_XZ);
  view3->SetSliceOrientation (vtkImageView2D::SLICE_ORIENTATION_YZ);

  double bcolor[3] = {0.9,0.9,0.9};
  view4->SetBackground (bcolor);
  
  view4->SetRenderingModeToPlanar();
  view4->SetShowCube(1);  

  /*
  view1->SetAboutData ("Powered by vtkINRIA3D");
  view2->SetAboutData ("Powered by vtkINRIA3D");
  view3->SetAboutData ("Powered by vtkINRIA3D");
  view4->SetAboutData ("Powered by vtkINRIA3D");
  */
  
  /**
     Link the views together for synchronization.
   */
  vtkImageViewCollection* pool = vtkImageViewCollection::New();
  pool->AddItem (view1);
  pool->AddItem (view2);
  pool->AddItem (view3);
  pool->AddItem (view4);

  vtkImageEllipsoidSource* imageSource = vtkImageEllipsoidSource::New();
  imageSource->SetWholeExtent (0, 127, 0, 127, 0, 127);
  imageSource->SetCenter (64,64,64);
  imageSource->SetRadius (64,32,32);
  imageSource->Update();

  auto image = imageSource->GetOutputPort();
  
  pool->SyncSetInput (image, 0);
  
  pool->SyncReset();
  pool->SyncRender();

  pool->SyncSetShowAnnotations (0);
  view1->ShowRulerWidgetOff();
  view2->ShowRulerWidgetOff();
  view3->ShowRulerWidgetOff();

  iren1->Start();


//   view4->ShowActorXOff();
//   view4->ShowActorYOff();
//   view4->ShowActorZOff();
  
  
  view1->Delete();
  view2->Delete();
  view3->Delete();
  view4->Delete();

  iren1->Delete();
  iren2->Delete();
  iren3->Delete();
  iren4->Delete();

  rwin1->Delete();
  rwin2->Delete();
  rwin3->Delete();
  rwin4->Delete();

  renderer1->Delete();
  renderer2->Delete();
  renderer3->Delete();
  renderer4->Delete();

  pool->Delete();
  
  imageSource->Delete();


  return 0;
}
