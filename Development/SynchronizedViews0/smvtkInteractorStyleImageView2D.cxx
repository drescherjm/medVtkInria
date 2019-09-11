/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: smvtkInteractorStyleImageView2D.cxx 1368 2009-11-30 18:58:27Z filus $
Language:  C++
Author:    $Author: filus $
Date:      $Date: 2009-11-30 19:58:27 +0100 (lun, 30 nov 2009) $
Version:   $Revision: 1368 $

Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
See Copyright.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "smvtkInteractorStyleImageView2D.h"
#include "vtkCallbackCommand.h"

#include "vtkAbstractPropPicker.h"
#include "vtkAssemblyPath.h"
#include "vtkMath.h"
#include "vtkObjectFactory.h"
#include "vtkRenderWindowInteractor.h"
#include <vtkImageData.h>
#include <vtkRenderWindow.h>
#include <vtkCamera.h>
#include <vtkRenderer.h>
#include "vtkCommand.h"
#include <QMultiMap>

#include <vtkImageView2DCommand.h>
#include "smvtkInteractorEvent.h"

/////////////////////////////////////////////////////////////////////////////////////////

class smvtkInteractorStyleImageView2D::smPrivate 
{
public:
	typedef QSharedPointer<smvtkInteractorEvent> InteractorEventPtr;
	typedef QMultiMap<smvtkInteractorEvent::Event_Codes,InteractorEventPtr> EventMap;
public:
	void	setEventCodes(smvtkInteractorEvent::Event_Codes & code, vtkRenderWindowInteractor* pInteractor);
	void	OnActivate(smvtkInteractorEvent::Event_Codes & code,smvtkInteractorStyleImageView2D* pInteractorStyle);
	void	OnDeactivate(smvtkInteractorEvent::Event_Codes & code,smvtkInteractorStyleImageView2D* pInteractorStyle);
	void	OnMouseMove(smvtkInteractorStyleImageView2D* pInteractorStyle);
public:
	EventMap m_mapActivationEvents;
	EventMap m_mapDeactivationEvents;
};

/////////////////////////////////////////////////////////////////////////////////////////

void smvtkInteractorStyleImageView2D::smPrivate::setEventCodes( smvtkInteractorEvent::Event_Codes & code, vtkRenderWindowInteractor* pInteractor )
{
	if (pInteractor != NULL) {
		if (pInteractor->GetAltKey()) {
			code |= smvtkInteractorEvent::EVT_ALT_KEY;
		}
		if (pInteractor->GetShiftKey()) {
			code |= smvtkInteractorEvent::EVT_SHIFT_KEY;
		}
		if (pInteractor->GetControlKey()) {
			code |= smvtkInteractorEvent::EVT_CTRL_KEY;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////

void smvtkInteractorStyleImageView2D::smPrivate::OnActivate( smvtkInteractorEvent::Event_Codes & code,smvtkInteractorStyleImageView2D* pInteractorStyle )
{
	QList<InteractorEventPtr> lst = m_mapActivationEvents.values(code);
	foreach(InteractorEventPtr ptr,lst) {
		ptr->OnActivate(code,pInteractorStyle);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////

void smvtkInteractorStyleImageView2D::smPrivate::OnDeactivate( smvtkInteractorEvent::Event_Codes & code,smvtkInteractorStyleImageView2D* pInteractorStyle )
{
	QList<InteractorEventPtr> lst = m_mapDeactivationEvents.values(code);
	foreach(InteractorEventPtr ptr,lst) {
		ptr->OnDeactivate(code,pInteractorStyle);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////

void smvtkInteractorStyleImageView2D::smPrivate::OnMouseMove( smvtkInteractorStyleImageView2D* pInteractorStyle )
{
	foreach(InteractorEventPtr ptr,m_mapActivationEvents) {
		ptr->OnMouseMove(pInteractorStyle);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////

vtkStandardNewMacro (smvtkInteractorStyleImageView2D);

smvtkInteractorStyleImageView2D::smvtkInteractorStyleImageView2D()
: vtkInteractorStyleImageView2D()
{
	m_pPrivate = new smPrivate();

	this->SliceStep = 0;
	this->RequestedPosition[0] = this->RequestedPosition[1] = 0;

	this->LeftButtonInteraction   = InteractionTypeNull;
	this->RightButtonInteraction  = InteractionTypeNull;
	this->MiddleButtonInteraction = InteractionTypeSlice;
	this->WheelButtonInteraction  = InteractionTypeSlice;
}


smvtkInteractorStyleImageView2D::~smvtkInteractorStyleImageView2D() 
{
	delete m_pPrivate;
}

//----------------------------------------------------------------------------
void smvtkInteractorStyleImageView2D::OnMouseMove() 
{
	/*
	switch (this->State) 
	{
	case VTKIS_SLICE_MOVE:
		this->SliceMove();
		this->InvokeEvent(vtkImageView2DCommand::SliceMoveEvent, this);

		{
			int x = this->Interactor->GetEventPosition()[0];
			int y = this->Interactor->GetEventPosition()[1];  
			this->FindPokedRenderer(x, y);
			this->RequestedPosition[0] = x;
			this->RequestedPosition[1] = y;
			this->InvokeEvent (vtkImageView2DCommand::RequestedPositionEvent, this);
		}

		break;
	case VTKIS_DOLLY:
	case VTKIS_ZOOM:
		this->Superclass::OnMouseMove();
		this->InvokeEvent(vtkImageView2DCommand::CameraZoomEvent, this);
		this->InvokeEvent(vtkImageView2DCommand::CameraMoveEvent, this);
		break;
	case VTKIS_PAN:
		this->Superclass::OnMouseMove();
		this->InvokeEvent(vtkImageView2DCommand::CameraPanEvent, this);
		this->InvokeEvent(vtkImageView2DCommand::CameraMoveEvent, this);
		break;
	case VTKIS_SPIN:
	case VTKIS_ROTATE:	
	case VTKIS_FORWARDFLY:
	case VTKIS_REVERSEFLY:
		this->Superclass::OnMouseMove();
		this->InvokeEvent(vtkImageView2DCommand::CameraMoveEvent, this);
		break;
	case VTKIS_NONE:
		this->DefaultMoveAction();
		break;
	default:
		this->Superclass::OnMouseMove();
		break;
	}
	*/

	m_pPrivate->OnMouseMove(this);
}

//----------------------------------------------------------------------------

void smvtkInteractorStyleImageView2D::OnLeftButtonDown() 
{
	smvtkInteractorEvent::Event_Codes code = smvtkInteractorEvent::EVT_LEFT_MOUSE_BTN;
	m_pPrivate->setEventCodes(code,this->Interactor);
	m_pPrivate->OnActivate(code,this);
}


//----------------------------------------------------------------------------

void smvtkInteractorStyleImageView2D::OnLeftButtonUp()
{
	smvtkInteractorEvent::Event_Codes code = smvtkInteractorEvent::EVT_LEFT_MOUSE_BTN;
	m_pPrivate->setEventCodes(code,this->Interactor);
	m_pPrivate->OnDeactivate(code,this);
}

//----------------------------------------------------------------------------

void smvtkInteractorStyleImageView2D::OnMiddleButtonDown()
{
	smvtkInteractorEvent::Event_Codes code = smvtkInteractorEvent::EVT_MIDDLE_MOUSE_BTN;
	m_pPrivate->setEventCodes(code,this->Interactor);
	m_pPrivate->OnActivate(code,this);
}

//----------------------------------------------------------------------------

void smvtkInteractorStyleImageView2D::OnMiddleButtonUp()
{
	smvtkInteractorEvent::Event_Codes code = smvtkInteractorEvent::EVT_MIDDLE_MOUSE_BTN;
	m_pPrivate->setEventCodes(code,this->Interactor);
	m_pPrivate->OnDeactivate(code,this);
}

//----------------------------------------------------------------------------

void smvtkInteractorStyleImageView2D::OnRightButtonDown() 
{
	smvtkInteractorEvent::Event_Codes code = smvtkInteractorEvent::EVT_RIGHT_MOUSE_BTN;
	m_pPrivate->setEventCodes(code,this->Interactor);
	m_pPrivate->OnActivate(code,this);
}

//----------------------------------------------------------------------------

void smvtkInteractorStyleImageView2D::OnRightButtonUp() 
{
	smvtkInteractorEvent::Event_Codes code = smvtkInteractorEvent::EVT_RIGHT_MOUSE_BTN;
	m_pPrivate->setEventCodes(code,this->Interactor);
	m_pPrivate->OnDeactivate(code,this);
}

//----------------------------------------------------------------------------

void smvtkInteractorStyleImageView2D::OnMouseWheelForward() 
{

	int x = this->Interactor->GetEventPosition()[0];
	int y = this->Interactor->GetEventPosition()[1];

	this->FindPokedRenderer(x, y);
	if ( !this->CurrentRenderer )
		return;

	switch(this->GetWheelButtonInteraction())
	{
	case InteractionTypeSlice:
		this->SliceStep = 1;
		this->InvokeEvent (vtkImageView2DCommand::SliceMoveEvent, this);
		break;
	case InteractionTypeWindowLevel:
		this->Superclass::OnMouseWheelForward();
		break;
	case InteractionTypeZoom:
		this->Superclass::OnMouseWheelForward();
		break;
	case InteractionTypePan:
		this->Superclass::OnMouseWheelForward();
		break;
	default:
		break;
	}

}

//----------------------------------------------------------------------------

void smvtkInteractorStyleImageView2D::OnMouseWheelBackward() 
{

	int x = this->Interactor->GetEventPosition()[0];
	int y = this->Interactor->GetEventPosition()[1];

	this->FindPokedRenderer(x, y);
	if ( !this->CurrentRenderer )
		return;

	switch(this->GetWheelButtonInteraction())
	{
	case InteractionTypeSlice:
		this->SliceStep = -1;
		this->InvokeEvent (vtkImageView2DCommand::SliceMoveEvent, this);
		break;
	case InteractionTypeWindowLevel:
		this->Superclass::OnMouseWheelBackward();
		break;
	case InteractionTypeZoom:
		this->Superclass::OnMouseWheelBackward();
		break;
	case InteractionTypePan:
		this->Superclass::OnMouseWheelBackward();
		break;
	default:
		break;
	}  
}



//----------------------------------------------------------------------------
void smvtkInteractorStyleImageView2D::OnChar() 
{
	vtkRenderWindowInteractor *rwi = this->Interactor;

	if (!strcmp (rwi->GetKeySym(),"Up"))
	{
		this->SliceStep = 1;
		this->InvokeEvent (vtkImageView2DCommand::SliceMoveEvent, this);
	}
	else if(!strcmp (rwi->GetKeySym(),"Down"))
	{

		this->SliceStep = -1;
		this->InvokeEvent (vtkImageView2DCommand::SliceMoveEvent, this); 
	}
	else if ((rwi->GetKeyCode() == 'r') || (rwi->GetKeyCode() == 'R'))
	{
		this->InvokeEvent (vtkImageView2DCommand::ResetViewerEvent, this);
	}
	this->Superclass::OnChar();
}

//----------------------------------------------------------------------------
void smvtkInteractorStyleImageView2D::OnKeyDown(void)
{
	// Apparently there is a problem here.
	// The event vtkCommand::CharEvent and vtkCommand::KeyPressEvent seem
	// to mix between each other.
	// tackled by calling the charevent
	// (supposed to be called at any keyboard event)
	this->OnChar();
	this->Superclass::OnKeyDown();
}

//----------------------------------------------------------------------------
void smvtkInteractorStyleImageView2D::OnKeyUp(void)
{
	this->Superclass::OnKeyUp();
}


//----------------------------------------------------------------------------
void smvtkInteractorStyleImageView2D::StartSliceMove()
{
	if ((this->State != VTKIS_NONE) && (this->State != VTKIS_PICK)) {
		return;
	}
	this->StartState(VTKIS_SLICE_MOVE);
	this->InvokeEvent(vtkImageView2DCommand::StartSliceMoveEvent, this);
}

//----------------------------------------------------------------------------
void smvtkInteractorStyleImageView2D::EndSliceMove()
{
	if (this->State != VTKIS_SLICE_MOVE) {
		return;
	}
	this->StopState();
	this->InvokeEvent(vtkImageView2DCommand::EndSliceMoveEvent, this);

}

//----------------------------------------------------------------------------
void smvtkInteractorStyleImageView2D::SliceMove()
{
	vtkRenderWindowInteractor *rwi = this->Interactor;
	int dy = rwi->GetEventPosition()[1] - rwi->GetLastEventPosition()[1];
	this->SliceStep = dy;
}

//----------------------------------------------------------------------------
void smvtkInteractorStyleImageView2D::DefaultMoveAction()
{
	this->InvokeEvent (vtkImageView2DCommand::DefaultMoveEvent ,this);
}

/////////////////////////////////////////////////////////////////////////////////////////

bool smvtkInteractorStyleImageView2D::OnWindowLevelInteractionDown()
{
	bool retVal = (updateCurrentRenderer());
	if (retVal) {

		// Redefine this button to handle window/level
		this->GrabFocus();

		int x = this->Interactor->GetEventPosition()[0];
		int y = this->Interactor->GetEventPosition()[1];  
		this->WindowLevelStartPosition[0] = x;
		this->WindowLevelStartPosition[1] = y;      
		
		this->StartState(VTKIS_WINDOW_LEVEL);
		this->InvokeEvent(vtkCommand::StartWindowLevelEvent,this);
	}
	return retVal;
}

/////////////////////////////////////////////////////////////////////////////////////////

bool smvtkInteractorStyleImageView2D::OnWindowLevelInteractionUp()
{
	bool retVal = (updateCurrentRenderer());
	if (retVal) {
		this->InvokeEvent(vtkCommand::EndWindowLevelEvent, this);
		this->StopState();
		this->ReleaseFocus();

	}
	return retVal;
}

/////////////////////////////////////////////////////////////////////////////////////////

void smvtkInteractorStyleImageView2D::OnZoomInteractionDown()
{
	if (!this->updateCurrentRenderer())
	{
		return;
	}

	this->vtkInteractorStyleTrackballCamera::OnRightButtonDown();
}

/////////////////////////////////////////////////////////////////////////////////////////

void smvtkInteractorStyleImageView2D::OnZoomInteractionUp()
{
	vtkInteractorStyleImage::OnRightButtonUp();
}

/////////////////////////////////////////////////////////////////////////////////////////

void smvtkInteractorStyleImageView2D::OnPanInteractionDown()
{
	vtkInteractorStyleImage::OnMiddleButtonDown();
}

/////////////////////////////////////////////////////////////////////////////////////////

void smvtkInteractorStyleImageView2D::OnPanInteractionUp()
{
	vtkInteractorStyleImage::OnMiddleButtonUp();
}

/////////////////////////////////////////////////////////////////////////////////////////

void smvtkInteractorStyleImageView2D::GrabFocus()
{
	this->Superclass::GrabFocus(this->EventCallbackCommand);
}

/////////////////////////////////////////////////////////////////////////////////////////

bool smvtkInteractorStyleImageView2D::addEventHandler( QSharedPointer<smvtkInteractorEvent> pEvent )
{
	bool retVal = (m_pPrivate != NULL);
	if (retVal) {
		retVal = !pEvent.isNull();
		if (retVal) {
			if (pEvent->hasValidActivationCode()) {
				smvtkInteractorEvent::Event_Codes code;
				pEvent->getActivationCode(code);
				m_pPrivate->m_mapActivationEvents.insert(code,pEvent);
			}
			if (pEvent->hasValidDeactivationCode()) {
				smvtkInteractorEvent::Event_Codes code;
				pEvent->getDeactivationCode(code);
				m_pPrivate->m_mapDeactivationEvents.insert(code,pEvent);
			}
		}
	}
	return retVal;
}

/////////////////////////////////////////////////////////////////////////////////////////

bool smvtkInteractorStyleImageView2D::OnSliceMoveInteractionDown()
{
	bool retVal = ((this->State == VTKIS_NONE) || (this->State == VTKIS_PICK));
	if (retVal) {
		this->StartState(VTKIS_SLICE_MOVE);
		this->InvokeEvent(vtkImageView2DCommand::StartSliceMoveEvent, this);
	}
	return retVal;
}

/////////////////////////////////////////////////////////////////////////////////////////

bool smvtkInteractorStyleImageView2D::OnSliceMoveInteractionUp()
{
	bool retVal = (this->State == VTKIS_SLICE_MOVE);
	if (retVal) {
		this->InvokeEvent(vtkImageView2DCommand::EndSliceMoveEvent, this);
		this->StopState();
	}
	return retVal;
}

/////////////////////////////////////////////////////////////////////////////////////////

void smvtkInteractorStyleImageView2D::updateRequiestedPosition()
{
	int x = this->Interactor->GetEventPosition()[0];
	int y = this->Interactor->GetEventPosition()[1];  
	this->FindPokedRenderer(x, y);
	this->RequestedPosition[0] = x;
	this->RequestedPosition[1] = y;
}

/////////////////////////////////////////////////////////////////////////////////////////

bool smvtkInteractorStyleImageView2D::OnWindowLevelInteractionMove()
{
	bool retVal = (this->State == VTKIS_WINDOW_LEVEL);
	if (retVal) {
		this->updateCurrentRenderer();
		this->WindowLevel();
		this->InvokeEvent(vtkCommand::InteractionEvent, NULL);
	}
	return retVal;
}

/////////////////////////////////////////////////////////////////////////////////////////

/**
 *	Updates the Current renderer to the one that is associated with the object at 
 *  the current current position.
 *
 *	\returns false if no render is at the current position.
 */

bool smvtkInteractorStyleImageView2D::updateCurrentRenderer()
{
	int x = this->Interactor->GetEventPosition()[0];
	int y = this->Interactor->GetEventPosition()[1];  
	this->FindPokedRenderer(x, y);

	return (this->CurrentRenderer != NULL);
}
