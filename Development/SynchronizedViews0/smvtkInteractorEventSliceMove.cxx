#include "smvtkInteractorEventSliceMove.h"
#include "smvtkInteractorStyleImageView2D.h"
#include "smvtkInteractorStyleImageView2D.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkImageView2DCommand.h"

/////////////////////////////////////////////////////////////////////////////////////////

smvtkInteractorEventSliceMove::smvtkInteractorEventSliceMove() 
{
	setObjectName("Select Slice Tool");
	m_flags |= FLG_USES_MOUSE_MOVE;
}

/////////////////////////////////////////////////////////////////////////////////////////

bool smvtkInteractorEventSliceMove::OnActivationAccepted( smvtkInteractorStyleImageView2D* pInteractorStyle )
{
	return  ( (pInteractorStyle != NULL) && (pInteractorStyle->OnSliceMoveInteractionDown()));
}

/////////////////////////////////////////////////////////////////////////////////////////

bool smvtkInteractorEventSliceMove::OnDeactivationAccepted( smvtkInteractorStyleImageView2D* pInteractorStyle )
{
	return  ( (pInteractorStyle != NULL) && (pInteractorStyle->OnSliceMoveInteractionUp()));
}

/////////////////////////////////////////////////////////////////////////////////////////

bool smvtkInteractorEventSliceMove::OnMouseMoveAccepted( smvtkInteractorStyleImageView2D* pInteractorStyle )
{
	bool retVal = (pInteractorStyle != NULL);
	if (retVal) {
		pInteractorStyle->SliceMove();
		pInteractorStyle->InvokeEvent(vtkImageView2DCommand::SliceMoveEvent, this);

		if (m_flags.testFlag(FLG_FIRST_OPTION)) {
			vtkRenderWindowInteractor* pInteractor = pInteractorStyle->GetInteractor();
			if (pInteractor != NULL) {
				pInteractorStyle->updateRequiestedPosition();
				pInteractorStyle->InvokeEvent(vtkImageView2DCommand::RequestedPositionEvent, pInteractorStyle);
			}
 
		}

	}
	return retVal;
	
}

/////////////////////////////////////////////////////////////////////////////////////////

/**
 *	When this is enabled other windows in the vtkImageCollection will be moved to the
 *  same position when this event is active (button down..).
 */

void smvtkInteractorEventSliceMove::enableSynchronizedPos( bool bEnable )
{
	if (bEnable)
	{
		m_flags |= FLG_FIRST_OPTION;
	} 
	else
	{
		m_flags &= ~FLG_FIRST_OPTION;
	}
}

