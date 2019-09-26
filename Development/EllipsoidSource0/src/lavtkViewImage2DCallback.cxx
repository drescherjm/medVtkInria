
#include "lavtkViewImage2DCallback.h"
#include "lavtkViewImage2D.h"
#include <vtkImageView2DCommand.h>
#include <vtkRenderWindowInteractor.h>

/////////////////////////////////////////////////////////////////////////////////////////

lavtkViewImage2DCallback::lavtkViewImage2DCallback()
{
  this->View     = 0;
}

/////////////////////////////////////////////////////////////////////////////////////////

lavtkViewImage2DCallback::~lavtkViewImage2DCallback()
{

}

/////////////////////////////////////////////////////////////////////////////////////////

void lavtkViewImage2DCallback::SetView (lavtkViewImage2D* view)
{
  this->View = view;
}

/////////////////////////////////////////////////////////////////////////////////////////'

void
lavtkViewImage2DCallback::Execute (vtkObject* caller, unsigned long event, void*)
{
  if( !this->GetView() )
  {
    return;
  }

  if (event == vtkImageView2DCommand::ZSliceMoveEvent) {
	  this->OnZSliceMoveEvent(caller);
  }
  
  if(event == vtkCommand::EndInteractionEvent )
  {
    this->OnEndInteractionEvent (caller);
    return;
  }

  if( event == vtkCommand::KeyPressEvent )
  {
    this->OnKeyPressEvent (caller, this->GetView()->GetRenderWindowInteractor()->GetKeyCode() );
    return;
  }
    
}

/////////////////////////////////////////////////////////////////////////////////////////

void
lavtkViewImage2DCallback::OnEndInteractionEvent(vtkObject* caller)
{

}

/////////////////////////////////////////////////////////////////////////////////////////

void
lavtkViewImage2DCallback::OnKeyPressEvent (vtkObject* caller, unsigned char key)
{

 
  
  switch(key)
  {
	case 'e':
	case 'E':
		this->View->SetVisibility(lavtkViewImage2D::PAINTBRUSH_WIDGET,(this->View->GetVisibility(lavtkViewImage2D::PAINTBRUSH_WIDGET) + 1) % 2);
        break;
        
  }
  
  
  return;
  
}

/////////////////////////////////////////////////////////////////////////////////////////

void lavtkViewImage2DCallback::OnZSliceMoveEvent( vtkObject* caller )
{
	int current_slice = this->View->GetZSlice();
	int min_slice     = this->View->GetWholeZMin();
	int max_slice     = this->View->GetWholeZMax();

}
