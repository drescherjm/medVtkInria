#ifndef __SMVTKINTERACTOREVENT_H__
#define __SMVTKINTERACTOREVENT_H__

#include <QObject>

class smvtkInteractorStyleImageView2D;

/////////////////////////////////////////////////////////////////////////////////////////

/**
 *	This class is the base class for key and mouse events. 
 */

class smvtkInteractorEvent  : public QObject
{
Q_OBJECT;
public:
	smvtkInteractorEvent();
public:
	enum Event_Code {
		EVT_NO_CODE					= 0x00000000,
		EVT_LEFT_MOUSE_BTN			= 0x00000001,
		EVT_MIDDLE_MOUSE_BTN		= 0x00000002,
		EVT_RIGHT_MOUSE_BTN			= 0x00000004,
		EVT_CTRL_KEY				= 0x00000100,
		EVT_ALT_KEY					= 0x00000200,
		EVT_SHIFT_KEY				= 0x00000400,
	};
	Q_DECLARE_FLAGS(Event_Codes,Event_Code);

	enum Flag {
		FLG_NO_CODE					= 0x00000000,
		FLG_DISABLED				= 0x00000001,
		FLG_ACTIVATED				= 0x00000010,
		FLG_USES_WHEEL				= 0x00000100,
		FLG_USES_MOUSE_MOVE			= 0x00000200,
		FLG_FIRST_OPTION			= 0x00010000,
		FLG_SECOND_OPTION			= 0x00020000,
		FLG_THIRD_OPTION			= 0x00040000,
	};
	Q_DECLARE_FLAGS(Flags,Flag);

	enum WheelCode {
		WHEEL_FORWARD				= 0x01,
		WHEEL_BACKWARD				= 0x02,
	};

public:
	void	setActivationCode(Event_Codes code);
	void	getActivationCode(Event_Codes & code);
	void	setDeactivationCode(Event_Codes code);
	void	getDeactivationCode(Event_Codes & code);
	bool	isActivated() const;
	bool	hasValidActivationCode() const;
	bool	hasValidDeactivationCode() const;
public:
	virtual bool	canActivate(Event_Codes code) const;
	virtual bool	canDeactivate(Event_Codes code) const;
	virtual	bool	OnActivate(Event_Codes code, smvtkInteractorStyleImageView2D* pInteractorStyle);
	virtual bool	OnDeactivate(Event_Codes code, smvtkInteractorStyleImageView2D* pInteractorStyle);
	virtual bool	OnMouseWheel(WheelCode code, smvtkInteractorStyleImageView2D* pInteractorStyle);
	virtual bool	OnMouseMove(smvtkInteractorStyleImageView2D* pInteractorStyle);
signals:
	void	activated();
	void	deactivated();
protected:
	virtual void	activate(bool bActivate);
	virtual bool	OnActivationAccepted(smvtkInteractorStyleImageView2D* pInteractorStyle)=0;
	virtual bool	OnDeactivationAccepted(smvtkInteractorStyleImageView2D* pInteractorStyle)=0;
	virtual bool	OnMouseWheelAccepted(WheelCode code, smvtkInteractorStyleImageView2D* pInteractorStyle);
	virtual bool	OnMouseMoveAccepted(smvtkInteractorStyleImageView2D* pInteractorStyle);
protected:
	Event_Codes		m_activationCode;
	Event_Codes		m_deactivationCode;
	Flags			m_flags;
};

#endif //__SMVTKINTERACTOREVENT_H__