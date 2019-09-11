#include "smvtkInteractorEventWL.h"
#include "smvtkInteractorStyleImageView2D.h"
#include "smvtkInteractorStyleImageView2D.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkImageView2DCommand.h"

/////////////////////////////////////////////////////////////////////////////////////////

smvtkInteractorEventWL::smvtkInteractorEventWL() 
{
	setObjectName("Window Level Tool");
	m_flags |= FLG_USES_MOUSE_MOVE;
}

/////////////////////////////////////////////////////////////////////////////////////////

bool smvtkInteractorEventWL::OnActivationAccepted( smvtkInteractorStyleImageView2D* pInteractorStyle )
{
	return  ( (pInteractorStyle != NULL) && (pInteractorStyle->OnWindowLevelInteractionDown()));
}

/////////////////////////////////////////////////////////////////////////////////////////

bool smvtkInteractorEventWL::OnDeactivationAccepted( smvtkInteractorStyleImageView2D* pInteractorStyle )
{
	return  ( (pInteractorStyle != NULL) && (pInteractorStyle->OnWindowLevelInteractionUp()));
}

/////////////////////////////////////////////////////////////////////////////////////////

bool smvtkInteractorEventWL::OnMouseMoveAccepted(smvtkInteractorStyleImageView2D* pInteractorStyle)
{
	return  ( (pInteractorStyle != NULL) && (pInteractorStyle->OnWindowLevelInteractionMove()));
}
