#include "smGUIPCH.h"

#include "smvtkMarkerHandleRepresentation.h"

#include "smMarkerInfo.h"
#include "smvtkMarkerData.h"

/////////////////////////////////////////////////////////////////////////////////////////

#if VTK_MAJOR_VERSION < 6
vtkCxxRevisionMacro(smvtkMarkerHandleRepresentation, "$Revision: 4389 $");
#endif

vtkStandardNewMacro(smvtkMarkerHandleRepresentation);

/////////////////////////////////////////////////////////////////////////////////////////

class smvtkMarkerHandleRepresentation::smPrivate 
{
public:
	vtkSmartPointer<smvtkMarkerData>	m_pParkData;
};

/////////////////////////////////////////////////////////////////////////////////////////

smvtkMarkerHandleRepresentation::smvtkMarkerHandleRepresentation() :Superclass()
{
	m_pPrivate = new smPrivate();
	m_pPrivate->m_pParkData = vtkSmartPointer<smvtkMarkerData>::New();
}

/////////////////////////////////////////////////////////////////////////////////////////

smvtkMarkerHandleRepresentation::~smvtkMarkerHandleRepresentation()
{
	delete m_pPrivate;
}

/////////////////////////////////////////////////////////////////////////////////////////

smvtkMarkerData* smvtkMarkerHandleRepresentation::getMarkerData()
{
	smvtkMarkerData* retVal = nullptr;
	if (m_pPrivate != nullptr) {
		retVal = m_pPrivate->m_pParkData;
	}
	return retVal;
}

/////////////////////////////////////////////////////////////////////////////////////////

void smvtkMarkerHandleRepresentation::setMarkerData( smvtkMarkerData* pMarkerData )
{
	if (m_pPrivate != nullptr) {
		m_pPrivate->m_pParkData = pMarkerData;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////

int smvtkMarkerHandleRepresentation::ComputeInteractionState( int X, int Y, int modify/*=0*/ )
{
	double pos[3], xyz[3];
	this->FocalPoint->GetPoint(0,pos);
	xyz[0] = static_cast<double>(X);
	xyz[1] = static_cast<double>(Y);
	xyz[2] = pos[2];

	this->VisibilityOn();
	double tol2 = this->Tolerance * this->Tolerance;
	if ( vtkMath::Distance2BetweenPoints(xyz,pos) <= tol2 )
	{
		this->InteractionState = vtkHandleRepresentation::Nearby;
	}
	else
	{
		this->InteractionState = vtkHandleRepresentation::Outside;
		if ( this->ActiveRepresentation )
		{
			this->VisibilityOff();
		}
	}

	return this->InteractionState;
}

/////////////////////////////////////////////////////////////////////////////////////////

void smvtkMarkerHandleRepresentation::PrintSelf( ostream& os, vtkIndent indent )
{
	//Superclass typedef defined in vtkTypeMacro() found in vtkSetGet.h
	this->Superclass::PrintSelf(os,indent);

	if ((m_pPrivate != nullptr) && (m_pPrivate->m_pParkData.GetPointer() != nullptr)) {
		m_pPrivate->m_pParkData->PrintSelf(os,indent);
	}
}

