#pragma once

#ifndef __LAVTKKEYEVENT_H__
#define __LAVTKKEYEVENT_H__

/////////////////////////////////////////////////////////////////////////////////////////

#include <string>
#include <map>

class vtkImageView2D;

/////////////////////////////////////////////////////////////////////////////////////////

// This class is an abstract base class for all dynamic key bindings. This will allow
// the user to set/change the keys for particular defined behaviors.

class lavtkKeyEventHandler 
{
public:
	lavtkKeyEventHandler();

public:
	std::string		getDescription() const;
	std::string		getKey() const;
	bool			getShift() const;
	bool			getAlt() const;
	bool			getCtrl() const;

public:
	void			setDescription(std::string str);
	void			setKey(std::string str);
	void			setShift( bool bShift );
	void			setAlt( bool bAlt );
	void			setCtrl( bool bCtrl );
	
public:
	virtual void	Execute(vtkImageView2D* pView)=0;
protected:
	std::string		m_strKey;
	std::string		m_strDescription;
	bool			m_bAlt;
	bool			m_bCtrl;
	bool			m_bShift;
};

/////////////////////////////////////////////////////////////////////////////////////////

typedef std::multimap<std::string,lavtkKeyEventHandler*> lavtkKeyEventHandlerMap;

/////////////////////////////////////////////////////////////////////////////////////////

#endif // __LAVTKKEYEVENT_H__
