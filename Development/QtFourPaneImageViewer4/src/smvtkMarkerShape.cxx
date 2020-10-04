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
#include <cmath>

#include <array>
#include <complex>

/////////////////////////////////////////////////////////////////////////////////////////

static auto pi = acos(-1);

/////////////////////////////////////////////////////////////////////////////////////////

#if VTK_MAJOR_VERSION < 6
vtkCxxRevisionMacro(smvtkMarkerShape, "$Revision: 4388 $");
#endif

vtkStandardNewMacro(smvtkMarkerShape);

//---------------------------------------------------------------------------
// Construct with model bounds = (-10,10,-10,10, 0,0), focal point = (0,0,0),
// radius=2, all parts of cursor visible, and wrapping off.
smvtkMarkerShape::smvtkMarkerShape()
{
	constexpr double sz = 15;

  this->ModelBounds[0] = -sz;
  this->ModelBounds[1] = sz;
  this->ModelBounds[2] = -sz;
  this->ModelBounds[3] = sz;
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
  vtkCellArray* newLines = nullptr, * newVerts = nullptr, * newPolygons = nullptr;
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

  newPolygons = vtkCellArray::New();

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
				drawAxes(newPts, newLines,newPolygons,1.5);
			break;
			case Star:
				drawStar(newPts, newLines,newPolygons,1.5);
			break;
			case Triangle:
				//drawTriangle(newPts,newLines);
				drawNSidedShapeThick(newPts, newLines, newPolygons, 3);
			break;
			case Rectangle:
				drawRectangle(newPts, newLines);
			break;
			case Diamond:
				drawNSidedShapeThick(newPts, newLines, newPolygons, 4);
			break;
			case Pentagon:
				drawNSidedShapeThick(newPts, newLines, newPolygons, 5);
			break;
			case Hexagon:
				drawNSidedShapeThick(newPts, newLines, newPolygons, 6);
			break;
			case Heptagon:
				drawNSidedShapeThick(newPts, newLines, newPolygons, 7);
			break;
			case Octagon:
				drawNSidedShapeThick(newPts, newLines, newPolygons, 8);
			break;
			case Circle:
				drawNSidedShapeThick(newPts, newLines, newPolygons, 12);
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
  
  if ( newLines )
    {
    output->SetLines(newLines);
    newLines->Delete();
    }

  output->SetPolys(newPolygons);

  newPts->Delete();

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

void  drawThickLine(vtkPoints* newPts, vtkCellArray* newLines, vtkCellArray* newPolygons, double nThickness, double x0, double x1, double y0, double y1, double z0)
{
	if (nThickness != 1.0) {
		std::array<vtkIdType, 4> ptsInThickLine;
		auto angle = atan2(y1 - y0, x1 - x0);

		double voxel[3];
		voxel[2] = z0;

		voxel[0] = x0 + nThickness * cos(angle + pi / 2);
		voxel[1] = y0 + nThickness * sin(angle + pi / 2);

		ptsInThickLine[0] = newPts->InsertNextPoint(voxel);

		voxel[0] = x0 + nThickness * cos(angle - pi / 2);
		voxel[1] = y0 + nThickness * sin(angle - pi / 2);

		ptsInThickLine[1] = newPts->InsertNextPoint(voxel);

		voxel[0] = x1 + nThickness * cos(angle - pi / 2);
		voxel[1] = y1 + nThickness * sin(angle - pi / 2);

		ptsInThickLine[2] = newPts->InsertNextPoint(voxel);

		voxel[0] = x1 + nThickness * cos(angle + pi / 2);
		voxel[1] = y1 + nThickness * sin(angle + pi / 2);

		ptsInThickLine[3] = newPts->InsertNextPoint(voxel);

		newPolygons->InsertNextCell(ptsInThickLine.size(), ptsInThickLine.data());
	}
	else {
		vtkIdType ptIds[2];
		double voxel[3];

		voxel[0] = x0;
		voxel[1] = y0;
		voxel[2] = z0;
		ptIds[0] = newPts->InsertNextPoint(voxel);

		voxel[0] = x1;
		voxel[1] = y1;
		voxel[2] = z0;
		ptIds[1] = newPts->InsertNextPoint(voxel);
		newLines->InsertNextCell(2, ptIds);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////

void smvtkMarkerShape::drawAxes( vtkPoints * newPts, vtkCellArray * newLines, vtkCellArray* newPolygons, double nThickness)
{
	double x[3];
	vtkIdType ptIds[2];

	// The lines making up the x axis
// 	x[0] = this->ModelBounds[0]; 
// 	x[1] = this->FocalPoint[1]; 
// 	x[2] = this->ModelBounds[4];
// 	ptIds[0] = newPts->InsertNextPoint(x);
// 
// 	x[0] = this->FocalPoint[0] - this->Radius; 
// 	x[1] = this->FocalPoint[1]; 
// 	x[2] = this->ModelBounds[4];
// 	ptIds[1] = newPts->InsertNextPoint(x);
// 	newLines->InsertNextCell(2,ptIds);

	drawThickLine(newPts, newLines, newPolygons, nThickness, ModelBounds[0], this->FocalPoint[0] - this->Radius, FocalPoint[1], FocalPoint[1], ModelBounds[4]);

// 	x[0] = this->FocalPoint[0] + this->Radius; 
// 	x[1] = this->FocalPoint[1]; 
// 	x[2] = this->ModelBounds[4];
// 	ptIds[0] = newPts->InsertNextPoint(x);
// 
// 	x[0] = this->ModelBounds[1]; 
// 	x[1] = this->FocalPoint[1]; 
// 	x[2] = this->ModelBounds[4];
// 	ptIds[1] = newPts->InsertNextPoint(x);
// 	newLines->InsertNextCell(2,ptIds);

	drawThickLine(newPts, newLines, newPolygons, nThickness, FocalPoint[0]+Radius, ModelBounds[1], FocalPoint[1], FocalPoint[1], ModelBounds[4]);

	// The lines making up the y axis
// 	x[0] = this->FocalPoint[0]; 
// 	x[1] = this->ModelBounds[2]; 
// 	x[2] = this->ModelBounds[4];
// 	ptIds[0] = newPts->InsertNextPoint(x);
// 
// 	x[0] = this->FocalPoint[0]; 
// 	x[1] = this->FocalPoint[1] - Radius;
// 	x[2] = this->ModelBounds[4];
// 	ptIds[1] = newPts->InsertNextPoint(x);
// 	newLines->InsertNextCell(2,ptIds);

	drawThickLine(newPts, newLines, newPolygons, nThickness, FocalPoint[0], FocalPoint[0], ModelBounds[2], FocalPoint[1] - Radius, ModelBounds[4]);


// 	x[0] = this->FocalPoint[0];
// 	x[1] = this->FocalPoint[1] + this->Radius; 
// 	x[2] = this->ModelBounds[4];
// 	ptIds[0] = newPts->InsertNextPoint(x);
// 
// 	x[0] = this->FocalPoint[0];
// 	x[1] = this->ModelBounds[3]; 
// 	x[2] = this->ModelBounds[4];
// 	ptIds[1] = newPts->InsertNextPoint(x);
// 	newLines->InsertNextCell(2,ptIds);

	drawThickLine(newPts, newLines, newPolygons, nThickness, FocalPoint[0], FocalPoint[0], FocalPoint[1] + Radius, ModelBounds[3], ModelBounds[4]);

}

/////////////////////////////////////////////////////////////////////////////////////////

void smvtkMarkerShape::drawStar( vtkPoints * newPts, vtkCellArray * newLines, vtkCellArray* newPolygons, double nThickness)
{

	drawAxes(newPts,newLines,newPolygons,nThickness);

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

void drawRectangle(vtkPoints* newPts, vtkCellArray* newLines, double x0, double x1, double y0, double y1, double z0)
{
	int index = 0;
	double voxel[3];
	std::array<vtkIdType, 5> ptIds;

	voxel[0] = x0;  // X0
	voxel[1] = y0;  // Y0
	voxel[2] = z0;
	ptIds[index++] = newPts->InsertNextPoint(voxel);

	voxel[0] = x1;   // X1
	voxel[1] = y0;   // Y0
	voxel[2] = z0;
	ptIds[index++] = newPts->InsertNextPoint(voxel);

	voxel[0] = x1;   // X1
	voxel[1] = y1;   // Y1
	voxel[2] = z0;
	ptIds[index++] = newPts->InsertNextPoint(voxel);

	voxel[0] = x0;    // X0
	voxel[1] = y1;    // Y1
	voxel[2] = z0;
	ptIds[index++] = newPts->InsertNextPoint(voxel);

	voxel[0] = x0;    // X0
	voxel[1] = y0;    // Y0
	voxel[2] = z0;
	ptIds[index++] = newPts->InsertNextPoint(voxel);

	newLines->InsertNextCell(index, ptIds.data());
}

/////////////////////////////////////////////////////////////////////////////////////////

void drawShape(vtkPoints* newPts, vtkCellArray* newLines, double nRadius, int nSides, int fpx, int fpy, int z0)
{
	

	std::vector<vtkIdType> ptIds;
	ptIds.reserve(nSides+1);

	double degrees = 90;

	auto radians = (degrees * pi) / 180;

	double angle = radians;
	double incr = 2.0 * pi / nSides;

	double newX = nRadius * cos(angle) + fpx;
	double newY = nRadius * sin(angle) + fpy;

	double voxel[3];
	voxel[2] = z0;

	voxel[0] = newX;
	voxel[1] = newY;

	auto first = newPts->InsertNextPoint(voxel);

	ptIds.push_back(first);

	for (int i = 0; i < nSides-1; i++)
	{
		//int oldX = newX;
		//int oldY = newY;
		angle += incr;
		newX = nRadius * cos(angle) + fpx;
		newY = nRadius * sin(angle) + fpy;

		voxel[0] = newX;
		voxel[1] = newY;

		ptIds.push_back(newPts->InsertNextPoint(voxel));
		
	}

	ptIds.push_back(first);

	newLines->InsertNextCell(ptIds.size(), ptIds.data());
}

/////////////////////////////////////////////////////////////////////////////////////////

void smvtkMarkerShape::drawRectangle(vtkPoints* newPts, vtkCellArray* newLines)
{
	double x0{ this->ModelBounds[0] };
	double x1{ this->ModelBounds[1] };
	double y0{ this->ModelBounds[2] };
	double y1{ this->ModelBounds[3] };
	double z0{ this->ModelBounds[4] };
	
	::drawRectangle(newPts, newLines, x0, x1, y0, y1, z0);

	x0++; y0++; 
	x1--; y1--;

	::drawRectangle(newPts, newLines, x0, x1, y0, y1, z0);

}

/////////////////////////////////////////////////////////////////////////////////////////

void smvtkMarkerShape::drawNSidedShape(vtkPoints* newPts, vtkCellArray* newLines, vtkCellArray* newPolygons, uint8_t nSides)
{
	double x0{ this->ModelBounds[0] };
	double x1{ this->ModelBounds[1] };
	double y0{ this->ModelBounds[2] };
	double y1{ this->ModelBounds[3] };
	double z0{ this->ModelBounds[4] };

	double nRadius = std::min(x1 - x0, y1 - y0) / 2;

	drawShape(newPts, newLines, nRadius, nSides, FocalPoint[0], FocalPoint[1], z0);
	drawShape(newPts, newLines, --nRadius, nSides, FocalPoint[0], FocalPoint[1], z0);
	//drawShape(newPts, newLines, nRadius+.5, nSides, FocalPoint[0], FocalPoint[1], z0);

}

/////////////////////////////////////////////////////////////////////////////////////////

std::vector < vtkIdType> drawShapeThick(vtkPoints* newPts, vtkCellArray* newLines, double nRadius, int nSides, int fpx, int fpy, int z0)
{
	static auto pi = acos(-1);

	std::vector<vtkIdType> retVal;
	retVal.reserve(nSides + 1);

	double degrees = 90;

	auto radians = (degrees * pi) / 180;

	double angle = radians;
	double incr = 2.0 * pi / nSides;

	double newX = nRadius * cos(angle) + fpx;
	double newY = nRadius * sin(angle) + fpy;

	double voxel[3];
	voxel[2] = z0;

	voxel[0] = newX;
	voxel[1] = newY;

	auto first = newPts->InsertNextPoint(voxel);

	retVal.push_back(first);

	for (int i = 0; i < nSides - 1; i++)
	{
		//int oldX = newX;
		//int oldY = newY;
		angle += incr;
		newX = nRadius * cos(angle) + fpx;
		newY = nRadius * sin(angle) + fpy;

		voxel[0] = newX;
		voxel[1] = newY;

		retVal.push_back(newPts->InsertNextPoint(voxel));

	}

	retVal.push_back(first);

	//newLines->InsertNextCell(retVal.size(), retVal.data());

	return retVal;
}

/////////////////////////////////////////////////////////////////////////////////////////

void smvtkMarkerShape::drawNSidedShapeThick(vtkPoints* newPts, vtkCellArray* newLines, vtkCellArray* newPolygons, uint8_t nSides)
{
	double x0{ this->ModelBounds[0] };
	double x1{ this->ModelBounds[1] };
	double y0{ this->ModelBounds[2] };
	double y1{ this->ModelBounds[3] };
	double z0{ this->ModelBounds[4] };

	double nRadius = std::min(x1 - x0, y1 - y0) / 2;

	auto v0 = drawShapeThick(newPts, newLines, nRadius, nSides, FocalPoint[0], FocalPoint[1], z0);
	auto v1 = drawShapeThick(newPts, newLines, nRadius - 3, nSides, FocalPoint[0], FocalPoint[1], z0);

	if (v0.size() > 1 && (v0.size() == v1.size())) {
		for (uint16_t i = 0; i < v0.size() - 1;i++) {
			std::array<vtkIdType, 4> ptsInThickLine{ v0[i],v0[i + 1],v1[i + 1],v1[i] };
			newPolygons->InsertNextCell(ptsInThickLine.size(), ptsInThickLine.data());
		}

	}
	else {
		assert(false);
	}
	
}

/////////////////////////////////////////////////////////////////////////////////////////