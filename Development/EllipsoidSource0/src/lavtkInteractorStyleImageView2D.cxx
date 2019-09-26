/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: lavtkInteractorStyleImageView2D.cxx,v 1.6 2009-08-14 23:04:23 jdrescher Exp $
Language:  C++
Author:    $Author: jdrescher $
Date:      $Date: 2009-08-14 23:04:23 $
Version:   $Revision: 1.6 $

Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.
See Copyright.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
// version vtkRenderingAddOn

#include "lavtkInteractorStyleImageView2D.h"

#include <vtkObjectFactory.h>
#include <vtkCallbackCommand.h>
#include <vtkImageView2DCommand.h>
#include <vtkRenderWindowInteractor.h>
#include "lavtkViewImage2D.h"
#include <vtkRenderWindow.h>
#include <vtkAssemblyPath.h>

/////////////////////////////////////////////////////////////////////////////////////////

vtkStandardNewMacro (lavtkInteractorStyleImageView2D);

lavtkKeyEventHandlerMap  lavtkInteractorStyleImageView2D::g_mapKeyEventHandlers;

/////////////////////////////////////////////////////////////////////////////////////////

lavtkInteractorStyleImageView2D::lavtkInteractorStyleImageView2D()
: vtkInteractorStyleImageView2D()
{
	this->EventCallbackCommand->SetCallback(lavtkInteractorStyleImageView2D::ProcessEvents);

	this->View       = 0;

	this->SliceStep = 0;
	this->RequestedPosition[0] = this->RequestedPosition[1] = 0;

	this->LeftButtonInteraction   = InteractionTypeZoom;
	this->RightButtonInteraction  = InteractionTypeWindowLevel;
	this->MiddleButtonInteraction = InteractionTypeSlice;
	this->WheelButtonInteraction  = InteractionTypeSlice;
	m_nShowState = 0;

}

/////////////////////////////////////////////////////////////////////////////////////////

lavtkInteractorStyleImageView2D::~lavtkInteractorStyleImageView2D() 
{
}

/////////////////////////////////////////////////////////////////////////////////////////

void lavtkInteractorStyleImageView2D::OnMouseMove() 
{
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
		if (this->Interactor->GetShiftKey()) {
			this->EndDolly();
			OnPanInteractionDown();
		}
		else
		{
			this->Superclass::OnMouseMove();
			this->InvokeEvent(vtkImageView2DCommand::CameraZoomEvent, this);
			this->InvokeEvent(vtkImageView2DCommand::CameraMoveEvent, this);
			View->InvokeEvent(vtkImageView::CameraChangedEvent);
		}
		break;
	case VTKIS_ZOOM:
			this->Superclass::OnMouseMove();
			this->InvokeEvent(vtkImageView2DCommand::CameraZoomEvent, this);
			this->InvokeEvent(vtkImageView2DCommand::CameraMoveEvent, this);
		break;
	case VTKIS_PAN:
		this->Superclass::OnMouseMove();
		this->InvokeEvent(vtkImageView2DCommand::CameraPanEvent, this);
		this->InvokeEvent(vtkImageView2DCommand::CameraMoveEvent, this);
		View->InvokeEvent(vtkImageView::CameraChangedEvent);
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
}

//////////////////////////////////////////////////////////////////////////

void lavtkInteractorStyleImageView2D::OnLeftButtonDown() 
{
	if (this->Interactor->GetControlKey()) {
		return;
	}

	int x = this->Interactor->GetEventPosition()[0];
	int y = this->Interactor->GetEventPosition()[1];  
	this->FindPokedRenderer(x, y);

	if (this->Interactor->GetRepeatCount())
	{
		this->RequestedPosition[0] = x;
		this->RequestedPosition[1] = y;
		this->InvokeEvent (vtkImageView2DCommand::RequestedPositionEvent, this);
		return;
	}  

	this->GrabFocus(this->EventCallbackCommand);
	
	switch(this->GetLeftButtonInteraction())
	{
	case InteractionTypeSlice:
		this->RequestedPosition[0] = x;
		this->RequestedPosition[1] = y;
		this->InvokeEvent (vtkImageView2DCommand::RequestedPositionEvent, this);
		this->StartSliceMove();
		break;
	case InteractionTypeWindowLevel:
		this->OnWindowLevelInteractionDown();
		break;
	case InteractionTypeZoom:
		if (!this->Interactor->GetShiftKey()) 
		{
			this->OnZoomInteractionDown();
		}
		else
		{
			this->OnPanInteractionDown();
		}
		break;
	case InteractionTypePan:
		this->OnPanInteractionDown();
		break;
	default:
		break;
	}

}

//////////////////////////////////////////////////////////////////////////

void lavtkInteractorStyleImageView2D::OnMiddleButtonDown()
{
	if (this->Interactor->GetControlKey()) {
		return;
	}
	
	int x = this->Interactor->GetEventPosition()[0];
	int y = this->Interactor->GetEventPosition()[1];  
	this->FindPokedRenderer(x, y);

	if (this->Interactor->GetRepeatCount())
	{
		this->RequestedPosition[0] = x;
		this->RequestedPosition[1] = y;
		this->InvokeEvent (vtkImageView2DCommand::RequestedPositionEvent, this);
		return;
	}  

	this->GrabFocus(this->EventCallbackCommand);

	switch(this->GetMiddleButtonInteraction())
	{
	case InteractionTypeSlice:
		this->RequestedPosition[0] = x;
		this->RequestedPosition[1] = y;
		this->InvokeEvent (vtkImageView2DCommand::RequestedPositionEvent, this);
		this->StartSliceMove();
		break;
	case InteractionTypeWindowLevel:
		this->OnWindowLevelInteractionDown();
		break;
	case InteractionTypeZoom:
		this->OnZoomInteractionDown();
		break;
	case InteractionTypePan:
		this->OnPanInteractionDown();
		break;
	default:
		break;
	}
	
}

//////////////////////////////////////////////////////////////////////////

void lavtkInteractorStyleImageView2D::OnMiddleButtonUp()
{


	switch (this->State)
	{
	case VTKIS_SLICE_MOVE:
		this->EndSliceMove();
		if ( this->Interactor )
		{
			this->ReleaseFocus();
		}
		break;    
	default:
		break;
	}	
	
	if (this->Interactor->GetControlKey()) {
		return;
	}

	switch (this->MiddleButtonInteraction)
	{
	case InteractionTypeSlice :
		break;
	case InteractionTypeZoom :
		this->OnZoomInteractionUp();
		break;
	case InteractionTypeWindowLevel :
		this->OnWindowLevelInteractionUp();
		break;
	case InteractionTypePan :
	default:
		this->OnPanInteractionUp();
		break;
	}	
}

//////////////////////////////////////////////////////////////////////////

void lavtkInteractorStyleImageView2D::OnLeftButtonUp()
{
	
	switch (this->State) 
	{
	case VTKIS_SLICE_MOVE:
		this->EndSliceMove();
		if ( this->Interactor )
		{
			this->ReleaseFocus();
		}
		break;
	
	case VTKIS_DOLLY:
		this->OnZoomInteractionUp();
		break;

	case VTKIS_PAN:
		this->OnPanInteractionUp();
		break;

	default:
		break;
	}

	if (this->Interactor->GetControlKey()) {
		return;
	}

	switch (this->LeftButtonInteraction)
	{
	case InteractionTypeSlice :
		break;
	case InteractionTypeZoom :
		//this->OnZoomInteractionUp();
		break;
	case InteractionTypePan :
		//this->OnPanInteractionUp();
		break;
	case InteractionTypeWindowLevel :
	default:
		this->OnWindowLevelInteractionUp();
		break;
	}  
}

//////////////////////////////////////////////////////////////////////////

void lavtkInteractorStyleImageView2D::OnRightButtonDown() 
{
	int x = this->Interactor->GetEventPosition()[0];
	int y = this->Interactor->GetEventPosition()[1];

	this->FindPokedRenderer(x, y);
	if ( !this->CurrentRenderer )
		return;

	this->GrabFocus(this->EventCallbackCommand);

	switch(this->GetRightButtonInteraction())
	{
	case InteractionTypeSlice:
		this->StartSliceMove();
		break;
	case InteractionTypeWindowLevel:
		this->OnWindowLevelInteractionDown();
		break;
	case InteractionTypeZoom:
		this->OnZoomInteractionDown();
		break;

	case InteractionTypePan:
		this->OnPanInteractionDown();
		break;
	default:
		break;
	}
}

//////////////////////////////////////////////////////////////////////////

void lavtkInteractorStyleImageView2D::OnRightButtonUp() 
{
	switch (this->State) 
	{
	case VTKIS_SLICE_MOVE:
		this->EndSliceMove();
		if ( this->Interactor )
		{
			this->ReleaseFocus();
		}
		break;
	default:
		break;
	}

	switch (this->RightButtonInteraction)
	{
	case InteractionTypeSlice :
		break;
	case InteractionTypePan :
		this->OnPanInteractionUp();
		break;
	case InteractionTypeWindowLevel :
		this->OnWindowLevelInteractionUp();
		break;
	case InteractionTypeZoom :
	default:
		this->OnZoomInteractionUp();
		break;
	}
}

//////////////////////////////////////////////////////////////////////////

void lavtkInteractorStyleImageView2D::OnChar() 
{
	if ( !this->View )
	{
		return;
	}

	vtkRenderWindowInteractor *rwi = this->Interactor;

	double factor = 0.0;
	int *size = this->View->GetRenderWindow()->GetSize();

	std::string key_sym (rwi->GetKeySym());

	if ((key_sym.compare("h")==0) || (key_sym.compare("H")==0)) {
		m_nShowState++;

		switch( m_nShowState % 6) {
		case 0:
			this->View->SetVisibility(VTK2DViewImage::IMAGE_ACTOR,1);
			this->View->SetVisibility(VTK2DViewImage::CROSSHAIR_ACTOR,1);
			this->View->SetVisibility(VTK2DViewImage::FG_IMAGE_ACTOR,1);
			break;
		case 1:
			this->View->SetVisibility(VTK2DViewImage::IMAGE_ACTOR,1);
			this->View->SetVisibility(VTK2DViewImage::CROSSHAIR_ACTOR,0);
			this->View->SetVisibility(VTK2DViewImage::FG_IMAGE_ACTOR,1);
			break;
		case 2:
			this->View->SetVisibility(VTK2DViewImage::IMAGE_ACTOR,1);
			this->View->SetVisibility(VTK2DViewImage::CROSSHAIR_ACTOR,0);
			this->View->SetVisibility(VTK2DViewImage::FG_IMAGE_ACTOR,0);
			break;
		case 3:
			this->View->SetVisibility(VTK2DViewImage::IMAGE_ACTOR,1);
			this->View->SetVisibility(VTK2DViewImage::CROSSHAIR_ACTOR,1);
			this->View->SetVisibility(VTK2DViewImage::FG_IMAGE_ACTOR,0);
			break;
		case 4:
			this->View->SetVisibility(VTK2DViewImage::IMAGE_ACTOR,0);
			this->View->SetVisibility(VTK2DViewImage::CROSSHAIR_ACTOR,1);
			this->View->SetVisibility(VTK2DViewImage::FG_IMAGE_ACTOR,1);
			break;
		case 5:
			this->View->SetVisibility(VTK2DViewImage::IMAGE_ACTOR,0);
			this->View->SetVisibility(VTK2DViewImage::CROSSHAIR_ACTOR,0);
			this->View->SetVisibility(VTK2DViewImage::FG_IMAGE_ACTOR,1);
			break;
		}


		//this->View->SetVisibility(VTK2DViewImage::CROSSHAIR_ACTOR,m_nShowState % 3);
		//this->View->SetVisibility(VTK2DViewImage::CROSSHAIR_ACTOR,(this->View->GetVisibility(VTK2DViewImage::CROSSHAIR_ACTOR) + 1) % 2);
		//this->View->SetVisibility(VTK2DViewImage::FG_IMAGE_ACTOR,(this->View->GetVisibility(VTK2DViewImage::FG_IMAGE_ACTOR) + 1) % 2);
		this->View->Modified();
		this->View->Render();
	}

	vtkAssemblyPath       *path   = 0;
	vtkAbstractPropPicker *picker = 0;
}

//////////////////////////////////////////////////////////////////////////

void lavtkInteractorStyleImageView2D::OnMouseWheelForward()
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

//////////////////////////////////////////////////////////////////////////

void lavtkInteractorStyleImageView2D::OnMouseWheelBackward()
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

/////////////////////////////////////////////////////////////////////////////////////////

void lavtkInteractorStyleImageView2D::OnKeyDown()
{

	if ( !this->View )
	{
		return;
	}

	vtkRenderWindowInteractor *rwi = this->Interactor;

	double factor = 0.0;
	int *size = this->View->GetRenderWindow()->GetSize();

	std::string key_sym (rwi->GetKeySym());
}

/////////////////////////////////////////////////////////////////////////////////////////

void lavtkInteractorStyleImageView2D::OnKeyUp()
{
	if ( !this->View )
	{
		return;
	}

	Superclass::OnKeyUp();
	handleDynamicKeybindings();

}

/////////////////////////////////////////////////////////////////////////////////////////

void lavtkInteractorStyleImageView2D::handleDynamicKeybindings()
{
	vtkRenderWindowInteractor *rwi = this->Interactor;
	std::string key_sym (rwi->GetKeySym());
	if (key_sym.compare("None")!=0) {
		auto res = g_mapKeyEventHandlers.equal_range(key_sym);

		for(auto it = res.first; it != res.second; ++it) {
			if (it->second->getAlt()) {
				if (rwi->GetAltKey() == 0) {
					continue;;
				}
			}
			if (it->second->getCtrl()) {
				if (rwi->GetControlKey() == 0) {
					continue;;
				}
			}
			if (it->second->getShift()) {
				if (rwi->GetShiftKey() == 0) {
					continue;;
				}
			}
			it->second->Execute(this->View);
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////

void lavtkInteractorStyleImageView2D::addKeyHandler( lavtkKeyEventHandler* pHandler )
{
	if (pHandler != NULL) {
		g_mapKeyEventHandlers.insert(lavtkKeyEventHandlerMap::value_type(pHandler->getKey(),pHandler));
	}
}

/////////////////////////////////////////////////////////////////////////////////////////

void lavtkInteractorStyleImageView2D::StartSliceMove()
{
	if ((this->State != VTKIS_NONE) && (this->State != VTKIS_PICK)) {
		return;
	}
	this->StartState(VTKIS_SLICE_MOVE);
	this->InvokeEvent(vtkImageView2DCommand::StartSliceMoveEvent, this);
}

/////////////////////////////////////////////////////////////////////////////////////////

void lavtkInteractorStyleImageView2D::EndSliceMove()
{
	if (this->State != VTKIS_SLICE_MOVE) {
		return;
	}
	this->StopState();
	this->InvokeEvent(vtkImageView2DCommand::EndSliceMoveEvent, this);

}

/////////////////////////////////////////////////////////////////////////////////////////

void lavtkInteractorStyleImageView2D::SliceMove()
{
	vtkRenderWindowInteractor *rwi = this->Interactor;
	int dy = rwi->GetEventPosition()[1] - rwi->GetLastEventPosition()[1];
	this->SliceStep = dy;
}

/////////////////////////////////////////////////////////////////////////////////////////

void lavtkInteractorStyleImageView2D::DefaultMoveAction()
{
	this->InvokeEvent (vtkImageView2DCommand::DefaultMoveEvent ,this);
}

/////////////////////////////////////////////////////////////////////////////////////////

bool lavtkInteractorStyleImageView2D::OnWindowLevelInteractionDown()
{
	bool retVal = (this->CurrentRenderer != NULL);
	if (retVal) {

		if (this->Interactor->GetShiftKey() || this->Interactor->GetControlKey()) 
		{
			if (this->GetLeftButtonInteraction() == InteractionTypeWindowLevel)
				this->StartSliceMove();
		}
		else {
			int x = this->Interactor->GetEventPosition()[0];
			int y = this->Interactor->GetEventPosition()[1];

			this->FindPokedRenderer(x, y);

			// Redefine this button to handle window/level
			this->GrabFocus(this->EventCallbackCommand);
			this->WindowLevelStartPosition[0] = x;
			this->WindowLevelStartPosition[1] = y;      
			this->StartWindowLevel();
		}


	}
	return retVal;
}

/////////////////////////////////////////////////////////////////////////////////////////

bool lavtkInteractorStyleImageView2D::OnWindowLevelInteractionUp()
{
	switch (this->State) 
	{
	case VTKIS_WINDOW_LEVEL:
		this->EndWindowLevel();
		if ( this->Interactor )
		{
			this->ReleaseFocus();
		}
		break;
	}
	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////

void lavtkInteractorStyleImageView2D::OnZoomInteractionDown()
{
	int x = this->Interactor->GetEventPosition()[0];
	int y = this->Interactor->GetEventPosition()[1];

	this->FindPokedRenderer(x, y);
	if (this->CurrentRenderer == NULL)
	{
		return;
	}

	this->GrabFocus(this->EventCallbackCommand);
	View->InvokeEvent(vtkImageView::CameraChangedEvent);
	this->StartDolly();
}

/////////////////////////////////////////////////////////////////////////////////////////

void lavtkInteractorStyleImageView2D::OnZoomInteractionUp()
{
	switch (this->State) 
	{
	case VTKIS_DOLLY:
		View->InvokeEvent(vtkImageView::CameraChangedEvent);
		this->EndDolly();

		if ( this->Interactor )
		{
			this->ReleaseFocus();
		}
		break;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////

void lavtkInteractorStyleImageView2D::OnPanInteractionDown()
{
	this->FindPokedRenderer(this->Interactor->GetEventPosition()[0], 
		this->Interactor->GetEventPosition()[1]);
	if (this->CurrentRenderer == NULL)
	{
		return;
	}

	View->InvokeEvent(vtkImageView::CameraChangedEvent);
	this->GrabFocus(this->EventCallbackCommand);
	this->StartPan();
}

/////////////////////////////////////////////////////////////////////////////////////////

void lavtkInteractorStyleImageView2D::OnPanInteractionUp()
{
	switch (this->State) 
	{
	case VTKIS_PAN:
		View->InvokeEvent(vtkImageView::CameraChangedEvent);
		this->EndPan();
		if ( this->Interactor )
		{
			this->ReleaseFocus();
		}
		break;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////

/**
*	\brief
*	Here we override the default behavior for vtkInteractorStyle::ProcessEvents
*  so that OnChar() will be called even if the CharEvent has an observer.
*
*	\details
*	The default behavoir of vtkInteractorStyle::ProcessEvents skips calling the 
*	virtual functions for events if the event has an observer.
*/

void lavtkInteractorStyleImageView2D::ProcessEvents(vtkObject* object, 
													unsigned long event,
													void* clientdata, 
													void* calldata)
{
	lavtkInteractorStyleImageView2D* self 
		= reinterpret_cast<lavtkInteractorStyleImageView2D *>( clientdata );
	
	switch(event)
	{

	case vtkCommand::CharEvent:  
		if (self->HandleObservers && 
			self->HasObserver(vtkCommand::CharEvent)) 
		{
			self->InvokeEvent(vtkCommand::CharEvent,NULL);
		}
		self->OnChar();
		break;
	default:
		Superclass::ProcessEvents(object,event,clientdata,calldata);
		break;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////

void lavtkInteractorStyleImageView2D::SetView( lavtkViewImage2D* view )
{
	this->View = view;

	this->View->SetVisibility(VTK2DViewImage::IMAGE_ACTOR,1);
	this->View->SetVisibility(VTK2DViewImage::CROSSHAIR_ACTOR,1);
	this->View->SetVisibility(VTK2DViewImage::FG_IMAGE_ACTOR,1);
}

/////////////////////////////////////////////////////////////////////////////////////////
