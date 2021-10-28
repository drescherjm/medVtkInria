#pragma once

#ifndef SMVTKIMAGEVIEW2DCOMMAND_H
#define SMVTKIMAGEVIEW2DCOMMAND_H

#define USE_MED_VTK_INRIA_EX

#include <vtkVersion.h>
#include <vtkImageView2DCommand.h>

/////////////////////////////////////////////////////////////////////////////////////////

class smvtkImageView2DCommand : public vtkImageView2DCommand
{
public:
	static smvtkImageView2DCommand *New();

#if VTK_MAJOR_VERSION < 6
	vtkTypeRevisionMacro(smvtkImageView2DCommand,vtkImageView2DCommand);
#else
	vtkTypeMacro(smvtkImageView2DCommand, vtkImageView2DCommand);
#endif

public:
	enum EventIds {
		WindowSizeChanged = vtkImageView2DCommand::DefaultMoveEvent+1,
		AlignLeft,
		AlignRight,
	};
protected:
	smvtkImageView2DCommand();
	~smvtkImageView2DCommand();
	// Description:
	// Satisfy the superclass API for callbacks. Recall that the caller is
	// the instance invoking the event; eid is the event id (see
	// vtkCommand.h); and calldata is information sent when the callback
	// was invoked (e.g., progress value in the vtkCommand::ProgressEvent).
	virtual void Execute(vtkObject *caller,unsigned long event, void *vtkNotUsed(callData));

private: 
	smvtkImageView2DCommand(const smvtkImageView2DCommand&);  			// Intentionally not implemented.
	void operator=(const smvtkImageView2DCommand&);						// Intentionally not implemented.

	class smPrivate;
	smPrivate*			m_pPrivate;
};

/////////////////////////////////////////////////////////////////////////////////////////

#endif // SMVTKIMAGEVIEW2DCOMMAND_H
