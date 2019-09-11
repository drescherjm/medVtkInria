#include "smvtkInteractorEvent.h"

/////////////////////////////////////////////////////////////////////////////////////////

smvtkInteractorEvent::smvtkInteractorEvent() : m_activationCode(EVT_NO_CODE), 
	m_deactivationCode(EVT_NO_CODE), m_flags(FLG_NO_CODE)
{

}

/////////////////////////////////////////////////////////////////////////////////////////

void smvtkInteractorEvent::setActivationCode( Event_Codes code )
{
	m_activationCode = code;	
}

/////////////////////////////////////////////////////////////////////////////////////////

void smvtkInteractorEvent::setDeactivationCode( Event_Codes code )
{
	m_deactivationCode = code;	
}

/////////////////////////////////////////////////////////////////////////////////////////

bool smvtkInteractorEvent::canActivate( Event_Codes code ) const
{
	bool retVal = (code == m_activationCode);
	if (retVal) {
		retVal = !isActivated();
		if (retVal) {
			retVal = hasValidActivationCode();
		}
	}
	return retVal;
}


/////////////////////////////////////////////////////////////////////////////////////////

bool smvtkInteractorEvent::canDeactivate( Event_Codes code ) const
{
	bool retVal = (code == m_deactivationCode);
	if (retVal) {
		retVal = isActivated();
		if (retVal) {
			retVal = hasValidDeactivationCode();
		}
	}
	return retVal;
}

/////////////////////////////////////////////////////////////////////////////////////////

bool smvtkInteractorEvent::hasValidActivationCode() const
{
	return (m_activationCode != EVT_NO_CODE);
}

/////////////////////////////////////////////////////////////////////////////////////////

bool smvtkInteractorEvent::hasValidDeactivationCode() const
{
	return (m_deactivationCode != EVT_NO_CODE);
}

/////////////////////////////////////////////////////////////////////////////////////////

bool smvtkInteractorEvent::isActivated() const
{
	return (m_flags & FLG_ACTIVATED);
}

/////////////////////////////////////////////////////////////////////////////////////////

bool smvtkInteractorEvent::OnMouseMove( smvtkInteractorStyleImageView2D* pInteractorStyle )
{
	bool retVal = isActivated();
	if (retVal) {
		if (m_flags.testFlag(FLG_USES_MOUSE_MOVE)) {
			retVal = OnMouseMoveAccepted(pInteractorStyle);	
		}
	}
	return retVal;
}

/////////////////////////////////////////////////////////////////////////////////////////

bool smvtkInteractorEvent::OnActivate( Event_Codes code, smvtkInteractorStyleImageView2D* pInteractorStyle )
{
	bool retVal = canActivate(code);
	if (retVal) {
		retVal = OnActivationAccepted(pInteractorStyle);
		if (retVal) {
			activate(true);
		}
	}
	return retVal;
}

/////////////////////////////////////////////////////////////////////////////////////////

bool smvtkInteractorEvent::OnDeactivate( Event_Codes code, smvtkInteractorStyleImageView2D* pInteractorStyle )
{
	bool retVal = canDeactivate(code);
	if (retVal) {
		retVal = OnDeactivationAccepted(pInteractorStyle);
		if (retVal) {
			activate(false);
		}
	}
	return retVal;
}

/////////////////////////////////////////////////////////////////////////////////////////

void smvtkInteractorEvent::activate( bool bActivate )
{
	if (bActivate) {
		if (!isActivated()) {
			m_flags |= FLG_ACTIVATED;
			emit activated();
		}
	
	}
	else
	{
		if (isActivated()) {
			m_flags &= ~FLG_ACTIVATED;
			emit deactivated();
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////

bool smvtkInteractorEvent::OnMouseMoveAccepted( smvtkInteractorStyleImageView2D* pInteractorStyle )
{
	return false;
}

/////////////////////////////////////////////////////////////////////////////////////////

void smvtkInteractorEvent::getActivationCode( Event_Codes & code )
{
	code = m_activationCode;
}

/////////////////////////////////////////////////////////////////////////////////////////

void smvtkInteractorEvent::getDeactivationCode( Event_Codes & code )
{
	code = m_deactivationCode;
}

/////////////////////////////////////////////////////////////////////////////////////////

/**
 *	The derived class should not call the base. 
 */

bool smvtkInteractorEvent::OnMouseWheelAccepted( WheelCode code, smvtkInteractorStyleImageView2D* pInteractorStyle )
{
	return false;
}

/////////////////////////////////////////////////////////////////////////////////////////

bool smvtkInteractorEvent::OnMouseWheel( WheelCode code, smvtkInteractorStyleImageView2D* pInteractorStyle )
{
	bool retVal = isActivated();
	if (retVal) {
		if (m_flags.testFlag(FLG_USES_WHEEL)) {
			retVal = OnMouseWheelAccepted(code,pInteractorStyle);
		}
	}
	return retVal;
}