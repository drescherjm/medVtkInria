#ifndef __SMVTKINTERACTOREVENTWL_H__
#define __SMVTKINTERACTOREVENTWL_H__

#include <QObject>
#include "smvtkInteractorEvent.h"

class smvtkInteractorStyleImageView2D;

/////////////////////////////////////////////////////////////////////////////////////////

/**
 *	This class is the base class for key and mouse events. 
 */

class smvtkInteractorEventWL  : public smvtkInteractorEvent
{
Q_OBJECT;
public:
	smvtkInteractorEventWL();
protected:
	virtual bool	OnActivationAccepted(smvtkInteractorStyleImageView2D* pInteractorStyle);
	virtual bool	OnDeactivationAccepted(smvtkInteractorStyleImageView2D* pInteractorStyle);
	virtual	bool	OnMouseMoveAccepted(smvtkInteractorStyleImageView2D* pInteractorStyle);
};

#endif //__SMVTKINTERACTOREVENTWL_H__