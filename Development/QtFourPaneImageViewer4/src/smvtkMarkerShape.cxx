/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: smvtkMarker.cxx,v $

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

//#include "smGUIPCH.h"

#include "smvtkMarkerShape.h"
#include "vtkCellArray.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkPolyData.h"
#include <math.h>

#if VTK_MAJOR_VERSION < 6
vtkCxxRevisionMacro(smvtkMarkerShape, "$Revision: 4388 $");
#endif

vtkStandardNewMacro(smvtkMarkerShape);

//---------------------------------------------------------------------------
// Construct with model bounds = (-10,10,-10,10, 0,0), focal point = (0,0,0),
// radius=2, all parts of cursor visible, and wrapping off.
smvtkMarkerShape::smvtkMarkerShape()
{
  this->ModelBounds[0] = -10.0;
  this->ModelBounds[1] = 10.0;
  this->ModelBounds[2] = -10.0;
  this->ModelBounds[3] = 10.0;
  this->ModelBounds[4] = 0.0;
  this->ModelBounds[5] = 0.0;

  this->FocalPoint[0] = 0.0;  
  this->FocalPoint[1] = 0.0;  
  this->FocalPoint[2] = 0.0;

  this->Outline = 1;
  this->Shape = 1;
  this->Point = 1;
  this->Radius = 2;
  this->Wrap = 0;
  this->TranslationMode = 0;

  this->st = Axes;

  this->SetNumberOfInputPorts(0);
}

/////////////////////////////////////////////////////////////////////////////////////////

smvtkMarkerShape::~smvtkMarkerShape()
{
	std::cout << __FUNCTION__;
}

//---------------------------------------------------------------------------
int smvtkMarkerShape::RequestData(
  vtkInformation *vtkNotUsed(request),
  vtkInformationVector **vtkNotUsed(inputVector),
  vtkInformationVector *outputVector)
{
  // get the info object
  vtkInformation *outInfo = outputVector->GetInformationObject(0);

  // get the output
  vtkPolyData *output = vtkPolyData::SafeDownCast(
    outInfo->Get(vtkDataObject::DATA_OBJECT()));

  int i;
  int numPts=0, numVerts=0, numLines=0;
  vtkPoints *newPts;
  vtkCellArray *newLines=nullptr, *newVerts=nullptr;
  double x[3];
  vtkIdType ptIds[5];

  // Check bounding box and origin
  //
  if ( this->Wrap ) 
    {
    for (i=0; i<2; i++)
      {
      this->FocalPoint[i] = this->ModelBounds[2*i] + 
             fmod(static_cast<double>(
                    this->FocalPoint[i]-this->ModelBounds[2*i]), 
                  static_cast<double>(
                    this->ModelBounds[2*i+1]-this->ModelBounds[2*i]));
      }
    } 
  else 
    {
    for (i=0; i<2; i++)
      {
      if ( this->FocalPoint[i] < this->ModelBounds[2*i] )
        {
        this->FocalPoint[i] = this->ModelBounds[2*i];
        }
      if ( this->FocalPoint[i] > this->ModelBounds[2*i+1] )
        {
        this->FocalPoint[i] = this->ModelBounds[2*i+1];
        }
      }
    }

  // Allocate storage
  //
  if (this->Point)
    {
    numPts += 1;
    numVerts += 1;
    }
  
  if (this->Shape) 
    {
    numPts += 8;
    numLines += 3;
    }

  if (this->Outline) 
    {
    numPts += 8;
    numLines += 12;
    }

  if ( numPts ) 
    {
    newPts = vtkPoints::New();
    newPts->Allocate(numPts);
    newLines = vtkCellArray::New();
    newLines->Allocate(newLines->EstimateSize(numLines,2));
    }
  else
    {
    return 1;
    }

  if ( numVerts )
    {
    newVerts = vtkCellArray::New();
    newVerts->Allocate(newLines->EstimateSize(1,1));
    }

  // Now create the representation. First the point (if requested).
  //
  if ( this->Point ) 
    {
    x[0] = this->FocalPoint[0];
    x[1] = this->FocalPoint[1];
    x[2] = 0.0;
    ptIds[0] = newPts->InsertNextPoint(x);
    newVerts->InsertNextCell(1,ptIds);
    output->SetVerts(newVerts);
    newVerts->Delete();
    }

  // Create Shape
  //
  if ( this->Shape ) 
    {
		switch(this->st) {
			case Axes:
				drawAxes(newPts, newLines);
			break;
			case Star:
				drawStar(newPts, newLines);
			break;
			case Triangle:
				drawTriangle(newPts,newLines);
			break;
			case Rectangle:
				drawRectangle(newPts, newLines);
			break;
		}

    }

  // Create outline
  //
  if ( this->Outline ) 
    {
    x[0] = this->ModelBounds[0]; 
    x[1] = this->ModelBounds[2]; 
    x[2] = this->ModelBounds[4];
    ptIds[0] = newPts->InsertNextPoint(x);

    x[0] = this->ModelBounds[1]; 
    x[1] = this->ModelBounds[2]; 
    x[2] = this->ModelBounds[4];
    ptIds[1] = newPts->InsertNextPoint(x);

    x[0] = this->ModelBounds[1]; 
    x[1] = this->ModelBounds[3]; 
    x[2] = this->ModelBounds[4];
    ptIds[2] = newPts->InsertNextPoint(x);

    x[0] = this->ModelBounds[0]; 
    x[1] = this->ModelBounds[3]; 
    x[2] = this->ModelBounds[4];
    ptIds[3] = newPts->InsertNextPoint(x);
    ptIds[4] = ptIds[0];
    newLines->InsertNextCell(5,ptIds);
    }

  // Update ourselves and release memory
  //
  output->SetPoints(newPts);
  newPts->Delete();

  if ( newLines )
    {
    output->SetLines(newLines);
    newLines->Delete();
    }

  return 1;
}

//---------------------------------------------------------------------------
// Set the boundary of the 3D cursor.
void smvtkMarkerShape::SetModelBounds(double xmin, double xmax, double ymin, double ymax,
                                 double zmin, double zmax)
{
  if ( xmin != this->ModelBounds[0] || xmax != this->ModelBounds[1] ||
       ymin != this->ModelBounds[2] || ymax != this->ModelBounds[3] ||
       zmin != this->ModelBounds[4] || zmax != this->ModelBounds[5] )
    {
    this->Modified();

    this->ModelBounds[0] = xmin; this->ModelBounds[1] = xmax; 
    this->ModelBounds[2] = ymin; this->ModelBounds[3] = ymax; 
    this->ModelBounds[4] = zmin; this->ModelBounds[5] = zmax; 

    for (int i=0; i<3; i++)
      {
      if ( this->ModelBounds[2*i] > this->ModelBounds[2*i+1] )
        {
        this->ModelBounds[2*i] = this->ModelBounds[2*i+1];
        }
      }
    }
}

//---------------------------------------------------------------------------
void smvtkMarkerShape::SetFocalPoint(double x[3])
{
  if ( x[0] == this->FocalPoint[0] && x[1] == this->FocalPoint[1] )
    {
    return;
    }
  
  this->Modified();

  double v[3];
  for (int i=0; i<2; i++)
    {
    v[i] = x[i] - this->FocalPoint[i];
    this->FocalPoint[i] = x[i];
  
    if ( this->TranslationMode )
      {
      this->ModelBounds[2*i] += v[i];
      this->ModelBounds[2*i+1] += v[i];
      }
    else if ( this->Wrap ) //wrap
      {
      this->FocalPoint[i] = this->ModelBounds[2*i] + 
             fmod(static_cast<double>(
                    this->FocalPoint[i]-this->ModelBounds[2*i]), 
                  static_cast<double>(
                    this->ModelBounds[2*i+1]-this->ModelBounds[2*i]));
      }
    else //clamp
      {
      if ( x[i] < this->ModelBounds[2*i] ) 
        { 
        this->FocalPoint[i] = this->ModelBounds[2*i];
        }
      if ( x[i] > this->ModelBounds[2*i+1] ) 
        { 
        this->FocalPoint[i] = this->ModelBounds[2*i+1];
        }
      }
    }
}

//---------------------------------------------------------------------------
void smvtkMarkerShape::SetModelBounds(double bounds[6])
{
  this->SetModelBounds(bounds[0], bounds[1], bounds[2], bounds[3], bounds[6], bounds[5]);
}

//---------------------------------------------------------------------------
// Turn every part of the 3D cursor on.
void smvtkMarkerShape::AllOn()
{
  this->OutlineOn();
  this->ShapeOn();
  this->PointOn();
}

//---------------------------------------------------------------------------
// Turn every part of the 3D cursor off.
void smvtkMarkerShape::AllOff()
{
  this->OutlineOff();
  this->ShapeOff();
  this->PointOff();
}

//---------------------------------------------------------------------------
void smvtkMarkerShape::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

  os << indent << "ModelBounds: \n";
  os << indent << "  Xmin,Xmax: (" << this->ModelBounds[0] << ", " << this->ModelBounds[1] << ")\n";
  os << indent << "  Ymin,Ymax: (" << this->ModelBounds[2] << ", " << this->ModelBounds[3] << ")\n";
  os << indent << "  Zmin,Zmax: (" << this->ModelBounds[4] << ", " << this->ModelBounds[5] << ")\n";

  os << indent << "Focal Point: (" << this->FocalPoint[0] << ", "
               << this->FocalPoint[1] << ", "
               << this->FocalPoint[2] << ")\n";

  os << indent << "Outline: " << (this->Outline ? "On\n" : "Off\n");
  os << indent << "Shape: " << (this->Shape ? "On\n" : "Off\n");
  os << indent << "Point: " << (this->Point ? "On\n" : "Off\n");
  os << indent << "Radius: " << this->Radius << "\n";
  os << indent << "Wrap: " << (this->Wrap ? "On\n" : "Off\n");
  os << indent << "Translation Mode: " 
     << (this->TranslationMode ? "On\n" : "Off\n");
}

/////////////////////////////////////////////////////////////////////////////////////////

void smvtkMarkerShape::setShapeType( ShapeType st )
{
	this->st = st;
}

/////////////////////////////////////////////////////////////////////////////////////////

smvtkMarkerShape::ShapeType smvtkMarkerShape::getShapeType()
{
	return this->st;
}

/////////////////////////////////////////////////////////////////////////////////////////

void smvtkMarkerShape::drawAxes( vtkPoints * newPts, vtkCellArray * newLines )
{
	double x[3];
	vtkIdType ptIds[2];

	// The lines making up the x axis
	x[0] = this->ModelBounds[0]; 
	x[1] = this->FocalPoint[1]; 
	x[2] = this->ModelBounds[4];
	ptIds[0] = newPts->InsertNextPoint(x);

	x[0] = this->FocalPoint[0] - this->Radius; 
	x[1] = this->FocalPoint[1]; 
	x[2] = this->ModelBounds[4];
	ptIds[1] = newPts->InsertNextPoint(x);
	newLines->InsertNextCell(2,ptIds);

	x[0] = this->FocalPoint[0] + this->Radius; 
	x[1] = this->FocalPoint[1]; 
	x[2] = this->ModelBounds[4];
	ptIds[0] = newPts->InsertNextPoint(x);

	x[0] = this->ModelBounds[1]; 
	x[1] = this->FocalPoint[1]; 
	x[2] = this->ModelBounds[4];
	ptIds[1] = newPts->InsertNextPoint(x);
	newLines->InsertNextCell(2,ptIds);

	// The lines making up the y axis
	x[0] = this->FocalPoint[0]; 
	x[1] = this->ModelBounds[2]; 
	x[2] = this->ModelBounds[4];
	ptIds[0] = newPts->InsertNextPoint(x);

	x[0] = this->FocalPoint[0]; 
	x[1] = this->FocalPoint[1] - this->Radius;
	x[2] = this->ModelBounds[4];
	ptIds[1] = newPts->InsertNextPoint(x);
	newLines->InsertNextCell(2,ptIds);

	x[0] = this->FocalPoint[0];
	x[1] = this->FocalPoint[1] + this->Radius; 
	x[2] = this->ModelBounds[4];
	ptIds[0] = newPts->InsertNextPoint(x);

	x[0] = this->FocalPoint[0];
	x[1] = this->ModelBounds[3]; 
	x[2] = this->ModelBounds[4];
	ptIds[1] = newPts->InsertNextPoint(x);
	newLines->InsertNextCell(2,ptIds);
}

/////////////////////////////////////////////////////////////////////////////////////////

void smvtkMarkerShape::drawStar( vtkPoints * newPts, vtkCellArray * newLines )
{

	drawAxes(newPts,newLines);

	double x[3];
	vtkIdType ptIds[2];

	x[0] = this->ModelBounds[0]; 
	x[1] = this->ModelBounds[2]; 
	x[2] = this->ModelBounds[4];
	ptIds[0] = newPts->InsertNextPoint(x);

	x[0] = this->FocalPoint[0] - this->Radius; 
	x[1] = this->FocalPoint[1] - this->Radius; 
	x[2] = this->ModelBounds[4];
	ptIds[1] = newPts->InsertNextPoint(x);
	newLines->InsertNextCell(2,ptIds);

	x[0] = this->ModelBounds[0]; 
	x[1] = this->ModelBounds[3]; 
	x[2] = this->ModelBounds[4];
	ptIds[0] = newPts->InsertNextPoint(x);

	x[0] = this->FocalPoint[0] - this->Radius; 
	x[1] = this->FocalPoint[1] + this->Radius; 
	x[2] = this->ModelBounds[4];
	ptIds[1] = newPts->InsertNextPoint(x);
	newLines->InsertNextCell(2,ptIds);


	x[0] = this->ModelBounds[1]; 
	x[1] = this->ModelBounds[3]; 
	x[2] = this->ModelBounds[4];
	ptIds[0] = newPts->InsertNextPoint(x);

	x[0] = this->FocalPoint[0] + this->Radius; 
	x[1] = this->FocalPoint[1] + this->Radius; 
	x[2] = this->ModelBounds[4];
	ptIds[1] = newPts->InsertNextPoint(x);
	newLines->InsertNextCell(2,ptIds);

	x[0] = this->ModelBounds[1]; 
	x[1] = this->ModelBounds[2]; 
	x[2] = this->ModelBounds[4];
	ptIds[0] = newPts->InsertNextPoint(x);

	x[0] = this->FocalPoint[0] + this->Radius; 
	x[1] = this->FocalPoint[1] - this->Radius; 
	x[2] = this->ModelBounds[4];
	ptIds[1] = newPts->InsertNextPoint(x);
	newLines->InsertNextCell(2,ptIds);
	
}

/////////////////////////////////////////////////////////////////////////////////////////

void smvtkMarkerShape::drawTriangle( vtkPoints * newPts, vtkCellArray * newLines )
{
	double x[3];
	vtkIdType ptIds[5];

	int pt=0;

	x[0] = this->FocalPoint[0]; 
	x[1] = this->ModelBounds[3]; 
	x[2] = this->ModelBounds[4];
	ptIds[pt++] = newPts->InsertNextPoint(x);

	x[0] = this->ModelBounds[0]; 
	x[1] = this->ModelBounds[2]; 
	x[2] = this->ModelBounds[4];
	ptIds[pt++] = newPts->InsertNextPoint(x);

	x[0] = this->ModelBounds[1]; 
	x[1] = this->ModelBounds[2]; 
	x[2] = this->ModelBounds[4];
	ptIds[pt++] = newPts->InsertNextPoint(x);

	x[0] = this->FocalPoint[0]; 
	x[1] = this->ModelBounds[3]; 
	x[2] = this->ModelBounds[4];
	ptIds[pt++] = newPts->InsertNextPoint(x);

	newLines->InsertNextCell(pt,ptIds);
}

/////////////////////////////////////////////////////////////////////////////////////////

void smvtkMarkerShape::drawRectangle(vtkPoints* newPts, vtkCellArray* newLines)
{
	double voxel[3];
	vtkIdType ptIds[5];

	int index = 0;

	double x0{ this->ModelBounds[0] };
	double x1{ this->ModelBounds[1] };
	double y0{ this->ModelBounds[2] };
	double y1{ this->ModelBounds[3] };
	
	voxel[0] = x0;  // X0
	voxel[1] = y0;  // Y0
	voxel[2] = this->ModelBounds[4];
	ptIds[index++] = newPts->InsertNextPoint(voxel);

	voxel[0] = x1;   // X1
	voxel[1] = y0;   // Y0
	voxel[2] = this->ModelBounds[4];
	ptIds[index++] = newPts->InsertNextPoint(voxel);

	voxel[0] = x1;   // X1
	voxel[1] = y1;   // Y1
	voxel[2] = this->ModelBounds[4];
	ptIds[index++] = newPts->InsertNextPoint(voxel);

	voxel[0] = x0;    // X0
	voxel[1] = y1;    // Y1
	voxel[2] = this->ModelBounds[4];
	ptIds[index++] = newPts->InsertNextPoint(voxel);

	voxel[0] = x0;    // X0
	voxel[1] = y0;    // Y0
	voxel[2] = this->ModelBounds[4];
	ptIds[index++] = newPts->InsertNextPoint(voxel);

	newLines->InsertNextCell(index, ptIds);

	x0++; y0++; 
	x1--; y1--;

	index = 0;

	voxel[0] = x0;  // X0
	voxel[1] = y0;  // Y0
	voxel[2] = this->ModelBounds[4];
	ptIds[index++] = newPts->InsertNextPoint(voxel);

	voxel[0] = x1;   // X1
	voxel[1] = y0;   // Y0
	voxel[2] = this->ModelBounds[4];
	ptIds[index++] = newPts->InsertNextPoint(voxel);

	voxel[0] = x1;   // X1
	voxel[1] = y1;   // Y1
	voxel[2] = this->ModelBounds[4];
	ptIds[index++] = newPts->InsertNextPoint(voxel);

	voxel[0] = x0;    // X0
	voxel[1] = y1;    // Y1
	voxel[2] = this->ModelBounds[4];
	ptIds[index++] = newPts->InsertNextPoint(voxel);

	voxel[0] = x0;    // X0
	voxel[1] = y0;    // Y0
	voxel[2] = this->ModelBounds[4];
	ptIds[index++] = newPts->InsertNextPoint(voxel);

	newLines->InsertNextCell(index, ptIds);
}
