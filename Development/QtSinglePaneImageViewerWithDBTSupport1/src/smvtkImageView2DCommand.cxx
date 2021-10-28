#include "smvtkImageView2DCommand.h"
#include "vtkObjectFactory.h"
#include "vtkSetGet.h"
#include "vtkRenderer.h"
#include "vtkImageView2DCommand.h"
#include "smvtkImageView2D.h"
#include "vtkRenderWindow.h"
#include "vtkImageActor.h"
#include "vtkCamera.h"
#include <vtkImageView2DExtended.h>

#ifdef USE_MED_VTK_INRIA_EX
#include <vtkImage2DDisplay.h>
#endif

/////////////////////////////////////////////////////////////////////////////////////////

#if VTK_MAJOR_VERSION < 6
vtkCxxRevisionMacro (smvtkImageView2DCommand, "$Revision: 4375 $");
#endif

/////////////////////////////////////////////////////////////////////////////////////////

class smvtkImageView2DCommand::smPrivate
{
public:
	void	alignLeft(smvtkImageView2DCommand* pCmd);
	void	alignRight(smvtkImageView2DCommand* pCmd);
};

/////////////////////////////////////////////////////////////////////////////////////////

void smvtkImageView2DCommand::smPrivate::alignLeft( smvtkImageView2DCommand* pCmd )
{
	if (pCmd != nullptr) {

#ifdef USE_MED_VTK_INRIA_EX
		vtkImageView2D* pViewer = pCmd->GetViewer();
		if (pViewer != nullptr) {
			
			double pan[2];

			if (pViewer-> GetPan()[0] == 0) {

				auto p2DDisplay = pViewer->GetImage2DDisplayForLayer(vtkImageView2DExtended::IMAGE_ACTOR);
				auto pRenderer = pViewer->GetRendererForLayer(vtkImageView2DExtended::IMAGE_ACTOR);

				if (p2DDisplay && pRenderer) {
					
					vtkRenderWindow* pRenderWindow = pRenderer->GetRenderWindow();
					vtkImageActor* pImageActor = p2DDisplay->GetImageActor();

					double bounds[6];
					pImageActor->GetDisplayBounds(bounds);

					double visBounds[6];
					pRenderer->ComputeVisiblePropBounds(visBounds);

					int extent[6];
					pImageActor->GetDisplayExtent(extent);


					vtkCamera* pCam = (pRenderer != nullptr) ? pRenderer->GetActiveCamera() : nullptr;
					if (pCam == nullptr) {
						return;
					}

					double scale = pCam->GetParallelScale();

					int nWidth = pRenderWindow->GetSize()[0];
					int nHeight = pRenderWindow->GetSize()[1];

					pRenderer->SetWorldPoint(bounds[0], bounds[2], bounds[4], 0);
					pRenderer->WorldToDisplay();

					double displayBL[3];
					pRenderer->GetDisplayPoint(displayBL);

					pRenderer->SetWorldPoint(bounds[1], bounds[3], bounds[5], 0);
					pRenderer->WorldToDisplay();

					double displayTR[3];
					pRenderer->GetDisplayPoint(displayTR);

					pan[0] = displayBL[0] - 5;
					pan[1] = 0;
					pViewer-> SetPan(pan);
				}
			}
		}
#else
		auto pViewer = pCmd->Viewer;
		
		if (pViewer != nullptr) {
			
			int pan[2];

			if (pViewer-> GetPan()[0] == 0) {
				vtkRenderer* pRenderer = pViewer-> GetRenderer();
				vtkRenderWindow* pRenderWindow = pViewer-> GetRenderWindow();
				vtkImageActor* pImageActor = pViewer-> GetImageActor();

				double bounds[6];
				pImageActor->GetDisplayBounds(bounds);

				double visBounds[6];
				pRenderer->ComputeVisiblePropBounds(visBounds);

				int extent[6];
				pImageActor->GetDisplayExtent(extent);


				vtkCamera* pCam = (pRenderer != nullptr) ? pRenderer->GetActiveCamera() : nullptr;
				if (pCam == nullptr) {
					return;
				}

				double scale = pCam->GetParallelScale();

				int nWidth = pRenderWindow->GetSize()[0];
				int nHeight = pRenderWindow->GetSize()[1];

				pRenderer->SetWorldPoint(bounds[0], bounds[2], bounds[4], 0);
				pRenderer->WorldToDisplay();

				double displayBL[3];
				pRenderer->GetDisplayPoint(displayBL);

				pRenderer->SetWorldPoint(bounds[1], bounds[3], bounds[5], 0);
				pRenderer->WorldToDisplay();

				double displayTR[3];
				pRenderer->GetDisplayPoint(displayTR);

				pan[0] = static_cast<int>(displayBL[0]) - 5;
				pan[1] = 0;
				pViewer-> SetPan(pan);
			}
		}
#endif

	}
}

/////////////////////////////////////////////////////////////////////////////////////////

void smvtkImageView2DCommand::smPrivate::alignRight( smvtkImageView2DCommand* pCmd )
{
	if (pCmd != nullptr) {

#ifdef USE_MED_VTK_INRIA_EX
		auto pViewer = pCmd->GetViewer();

		if (pViewer != nullptr) {
			vtkRenderer* pRenderer = pViewer->GetRenderer();
			vtkRenderWindow* pRenderWindow = pViewer->GetRenderWindow();
			vtkImageActor* pImageActor = pViewer->GetImageActor();

			double bounds[6];
			pImageActor->GetDisplayBounds(bounds);

			double visBounds[6];
			pRenderer->ComputeVisiblePropBounds(visBounds);

			int extent[6];
			pImageActor->GetDisplayExtent(extent);


			vtkCamera* pCam = (pRenderer != nullptr) ? pRenderer->GetActiveCamera() : nullptr;
			if (pCam == nullptr) {
				return;
			}

			double scale = pCam->GetParallelScale();

			int nWidth = pRenderWindow->GetSize()[0];
			int nHeight = pRenderWindow->GetSize()[1];

			pRenderer->SetWorldPoint(bounds[0], bounds[2], bounds[4], 0);
			pRenderer->WorldToDisplay();

			double displayBL[3];
			pRenderer->GetDisplayPoint(displayBL);

			pRenderer->SetWorldPoint(bounds[1], bounds[3], bounds[5], 0);
			pRenderer->WorldToDisplay();

			double displayTR[3];
			pRenderer->GetDisplayPoint(displayTR);

			double pan[2];
			pan[0] = 5 - displayBL[0];
			pan[1] = 0;
			pViewer->SetPan(pan);
		}
#else
		if (pCmd->Viewer != nullptr) {
			vtkRenderer* pRenderer = pCmd->Viewer->GetRenderer();
			vtkRenderWindow* pRenderWindow = pCmd->Viewer->GetRenderWindow();
			vtkImageActor* pImageActor = pCmd->Viewer->GetImageActor();

			double bounds[6];
			pImageActor->GetDisplayBounds(bounds);

			double visBounds[6];
			pRenderer->ComputeVisiblePropBounds(visBounds);

			int extent[6];
			pImageActor->GetDisplayExtent(extent);


			vtkCamera* pCam = (pRenderer != nullptr) ? pRenderer->GetActiveCamera() : nullptr;
			if ( pCam == nullptr) {
				return;
			}

			double scale = pCam->GetParallelScale(); 

			int nWidth = pRenderWindow->GetSize()[0];
			int nHeight = pRenderWindow->GetSize()[1];

			pRenderer->SetWorldPoint(bounds[0],bounds[2],bounds[4],0);
			pRenderer->WorldToDisplay();

			double displayBL[3];
			pRenderer->GetDisplayPoint(displayBL);

			pRenderer->SetWorldPoint(bounds[1],bounds[3],bounds[5],0);
			pRenderer->WorldToDisplay();

			double displayTR[3];
			pRenderer->GetDisplayPoint(displayTR);

			int pan[2];
			pan[0] = 5 -static_cast<int>(displayBL[0]);
			pan[1] = 0;
			pCmd->Viewer->SetPan(pan);
		}
#endif
	}
}

/////////////////////////////////////////////////////////////////////////////////////////

smvtkImageView2DCommand * smvtkImageView2DCommand::New()
{
	return new smvtkImageView2DCommand;
}

/////////////////////////////////////////////////////////////////////////////////////////

smvtkImageView2DCommand::smvtkImageView2DCommand()
{
	m_pPrivate = new smPrivate();
}

/////////////////////////////////////////////////////////////////////////////////////////

smvtkImageView2DCommand::~smvtkImageView2DCommand()
{
	delete m_pPrivate;
}

/////////////////////////////////////////////////////////////////////////////////////////

void smvtkImageView2DCommand::Execute( vtkObject *caller,unsigned long event, void* callData)
{
#ifdef USE_MED_VTK_INRIA_EX
	auto pViewer = GetViewer();
#else
	auto pViewer = Viewer;
#endif
	switch (event) {
		case smvtkImageView2DCommand::AlignLeft:
			//m_pPrivate->alignLeft(this);
			{
				smvtkImageView2D* psmViewer = smvtkImageView2D::SafeDownCast(pViewer);
				if (psmViewer != nullptr) {
					psmViewer->setImageAlignment(smvtkImageView2D::IA_Left);
				}
			}
			break;
		case smvtkImageView2DCommand::AlignRight:
			{
				smvtkImageView2D* psmViewer = smvtkImageView2D::SafeDownCast(pViewer);
				if (psmViewer != nullptr) {
					psmViewer->setImageAlignment(smvtkImageView2D::IA_Right);
				}
			}
			//m_pPrivate->alignRight(this);
			break;
		case smvtkImageView2DCommand::WindowSizeChanged:
			{
				smvtkImageView2D* psmViewer = smvtkImageView2D::SafeDownCast(pViewer);
				if (psmViewer != nullptr) {
					psmViewer->UpdateAlignment();
				}
			}
			break;
		default:
			Superclass::Execute(caller,event,callData);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
