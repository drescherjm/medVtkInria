#pragma once

#ifndef LAVTKINTERACTORSTYLEIMAGEVIEW2D_H
#define LAVTKINTERACTORSTYLEIMAGEVIEW2D_H

/////////////////////////////////////////////////////////////////////////////////////////

#include "vtkInteractorStyleImageView2D.h"
#include "lavtkKeyEvent.h"
#include <vtkSetGet.h>

class lavtkViewImage2D;
class vtkImageView2D;
class lavtkKeyEventHandler;

// Motion flags (See VTKIS_PICK and so on in vtkInteractorStyleImageView2D.h)
#define VTKIS_MEASURE      5050
#define VTKIS_ZSLICE_MOVE  5051

using VTK2DViewImage = lavtkViewImage2D;

/////////////////////////////////////////////////////////////////////////////////////////

class lavtkInteractorStyleImageView2D : public vtkInteractorStyleImageView2D
{
public:
	static lavtkInteractorStyleImageView2D *New();
	vtkTypeMacro (lavtkInteractorStyleImageView2D, vtkInteractorStyleImageView2D);

	static  void addKeyHandler(lavtkKeyEventHandler* pHandler);

	virtual void OnMouseMove();
	virtual void OnLeftButtonDown();
	virtual void OnLeftButtonUp();
	virtual void OnMiddleButtonDown();
	virtual void OnMiddleButtonUp();
	virtual void OnRightButtonDown();
	virtual void OnRightButtonUp();
	virtual void OnMouseWheelForward();
	virtual void OnMouseWheelBackward();
	virtual void OnChar();
	virtual void OnKeyDown();
	virtual void OnKeyUp();

	void SetView (lavtkViewImage2D* view);

	vtkGetObjectMacro(View, VTK2DViewImage);

	virtual bool OnWindowLevelInteractionDown();
	virtual bool OnWindowLevelInteractionUp();
	virtual void OnZoomInteractionDown();
	virtual void OnZoomInteractionUp();
	virtual void OnPanInteractionDown();
	virtual void OnPanInteractionUp();
	virtual void StartSliceMove();
	virtual void EndSliceMove();
	virtual void SliceMove();
	virtual	void DefaultMoveAction();

protected:
	lavtkInteractorStyleImageView2D();
	~lavtkInteractorStyleImageView2D();

	static void ProcessEvents(vtkObject* vtkNotUsed(object), unsigned long event, void* clientdata, void* calldata);

private:

	lavtkInteractorStyleImageView2D(const lavtkInteractorStyleImageView2D&);  // Not implemented.
	void operator=(const lavtkInteractorStyleImageView2D&);  // Not implemented.
	void handleDynamicKeybindings();
	
public:
	static lavtkKeyEventHandlerMap  g_mapKeyEventHandlers;

private:
	VTK2DViewImage*		View;
	int					m_nShowState;
};

/////////////////////////////////////////////////////////////////////////////////////////

#endif // LAVTKINTERACTORSTYLEIMAGEVIEW2D_H

