
#include "smvtkInteractorStyle2D.h"
#include "vtkCallbackCommand.h"

#include "vtkAbstractPropPicker.h"
#include "vtkAssemblyPath.h"
#include "vtkMath.h"
#include "vtkObjectFactory.h"
#include "vtkRenderWindowInteractor.h"
#include <vtkImageData.h>
#include <vtkRenderWindow.h>
#include <vtkCamera.h>
#include <vtkRenderer.h>
#include "vtkCommand.h"
#include <QMultiMap>

#include <vtkImageView2DCommand.h>
#include "smvtkInteractorEvent.h"
#include "smvtkInteractorEventWL.h"
#include "smvtkInteractorEventSliceMove.h"

/////////////////////////////////////////////////////////////////////////////////////////

class smvtkInteractorStyle2D::smPrivate 
{
public:
	void setupDefaultHandlers(smvtkInteractorStyle2D* pImageStyle);
};

/////////////////////////////////////////////////////////////////////////////////////////

void smvtkInteractorStyle2D::smPrivate::setupDefaultHandlers(smvtkInteractorStyle2D* pImageStyle)
{
	smvtkInteractorEvent::Event_Codes code;
	QSharedPointer<smvtkInteractorEvent> pWL(new smvtkInteractorEventWL());
	code = smvtkInteractorEvent::EVT_RIGHT_MOUSE_BTN;
	pWL->setActivationCode(code);
	pWL->setDeactivationCode(code);

	pImageStyle->addEventHandler(pWL);

	QSharedPointer<smvtkInteractorEventSliceMove> pSliceMove(new smvtkInteractorEventSliceMove());
	code = smvtkInteractorEvent::EVT_MIDDLE_MOUSE_BTN;
	pSliceMove->setActivationCode(code);
	pSliceMove->setDeactivationCode(code);
	pSliceMove->enableSynchronizedPos(true);

	pImageStyle->addEventHandler(pSliceMove);


}

/////////////////////////////////////////////////////////////////////////////////////////

vtkStandardNewMacro (smvtkInteractorStyle2D);

/////////////////////////////////////////////////////////////////////////////////////////

smvtkInteractorStyle2D::smvtkInteractorStyle2D()
: Superclass()
{
	m_pPrivate = new smPrivate();
	m_pPrivate->setupDefaultHandlers(this);
}

/////////////////////////////////////////////////////////////////////////////////////////

smvtkInteractorStyle2D::~smvtkInteractorStyle2D() 
{
	delete m_pPrivate;
}

