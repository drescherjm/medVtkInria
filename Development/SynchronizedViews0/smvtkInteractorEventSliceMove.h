#ifndef __SMVTKINTERACTOREVENTSLICEMOVE_H__
#define __SMVTKINTERACTOREVENTSLICEMOVE_H__

#include <QObject>
#include "smvtkInteractorEvent.h"

class smvtkInteractorStyleImageView2D;

/////////////////////////////////////////////////////////////////////////////////////////

/**
 *	This class is the base class for key and mouse events. 
 */

class smvtkInteractorEventSliceMove  : public smvtkInteractorEvent
{
Q_OBJECT;
public:
	smvtkInteractorEventSliceMove();
public:
	void	enableSynchronizedPos(bool bEnable);
protected:
	virtual bool	OnActivationAccepted(smvtkInteractorStyleImageView2D* pInteractorStyle);
	virtual bool	OnDeactivationAccepted(smvtkInteractorStyleImageView2D* pInteractorStyle);
	virtual bool	OnMouseMoveAccepted(smvtkInteractorStyleImageView2D* pInteractorStyle);
};

#endif //__SMVTKINTERACTOREVENTSLICEMOVE_H__