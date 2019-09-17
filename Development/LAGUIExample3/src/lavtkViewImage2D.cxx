#include "lavtkViewImage2D.h"
#include <vtkSetGet.h>
#include <vtkObjectFactory.h>
#include <QString>
#include "lavtkImageViewCornerAnnotation.h"
#include "SliceChangedCommand.h"
#include <QSize>
#include "lavtkViewImage2DCallback.h"
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkInteractorStyleImageView2D.h>
#include "lavtkInteractorStyleImageView2D.h"
#include <vtkCallbackCommand.h>
#include "vtkImageView2DCommand.h"
#include <vtkImageSliceMapper.h>
#include "vtkImageSlice.h"
#include <QImage>
#include <vtkImageData.h>
#include <vtkImageImport.h>

/////////////////////////////////////////////////////////////////////////////////////////

vtkStandardNewMacro(lavtkViewImage2D);

using VTK2DViewImage = lavtkViewImage2D; 
using VTKInteractorStyleImageView2DSupport = lavtkInteractorStyleImageView2D;

/////////////////////////////////////////////////////////////////////////////////////////

class lavtkViewImage2D::laPrivate
{
public:
	laPrivate(lavtkViewImage2D* pPublic);

#ifdef USE_CONTOUR_WIDGET
	typedef std::map<std::string, vtkMultiContourWidget*> MultiContourMap;
#endif // def USE_CONTOUR_WIDGET
public:
	void	updateContours();
	void	addContour();

#ifdef USE_CONTOUR_WIDGET
	vtkMultiContourWidget* NewMultiContourWidget(lavtkViewImage2D* pPublic, const std::string& Name);
#endif
	
	void	addTrace(lavtkViewImage2D* pPublic, const std::string& Name);
	void	updateCameraScaleBasedOnWindowSize(bool bForce = false);

	void	setupMainCallBack();

	QString replaceStrings(QString str);

#ifdef USE_PAINTBRUSH_EDITING
	void	setupPaintBrushWidget();
#endif //def USE_PAINTBRUSH_EDITING

public:
	static  void WindowModifiedCallback(vtkObject* caller, long unsigned int eventId, void* clientData, void* callData);
public:
	SliceChangedCommand* m_SliceChangedCommand;
	int											m_CurrentSlice;

#ifdef USE_CONTOUR_WIDGET
	vtkSmartPointer<laMultiContourCollection>	m_pContourCollection;
#endif /def USE_CONTOUR_WIDGET

	QString										m_strViewName;
	lavtkViewImage2D* m_pPublic;
	QSize										m_szLast;
	//laDocument* m_pDoc;

	vtkSmartPointer<lavtkViewImage2DCallback>	m_pCallBack;

#ifdef USE_PAINTBRUSH_EDITING
	vtkSmartPointer<lavtkPaintbrushWidget>		m_pPaintBrushWidget;
#endif //def USE_PAINTBRUSH_EDITING

	QImage										m_Image;
};

/////////////////////////////////////////////////////////////////////////////////////////

lavtkViewImage2D::laPrivate::laPrivate(lavtkViewImage2D* pPublic) : m_pPublic(pPublic),
m_SliceChangedCommand(NULL), m_CurrentSlice(-1)
{
#ifdef USE_CONTOUR_WIDGET
	m_pContourCollection = vtkSmartPointer<laMultiContourCollection>::New();
	m_pContourCollection->setImageViewer(pPublic);
#endif /def USE_CONTOUR_WIDGET
}


QImage vtkImageDataToQImage(vtkSmartPointer<vtkImageData> imageData)
{
	if (!imageData) { return QImage(); }

	/// \todo retrieve just the UpdateExtent
	int width = imageData->GetDimensions()[0];
	int height = imageData->GetDimensions()[1];
	QImage image(width, height, QImage::Format_RGB32);
	QRgb* rgbPtr =
		reinterpret_cast<QRgb*>(image.bits()) + width * (height - 1);
	unsigned char* colorsPtr =
		reinterpret_cast<unsigned char*>(imageData->GetScalarPointer());

	// Loop over the vtkImageData contents.
	for (int row = 0; row < height; row++)
	{
		for (int col = 0; col < width; col++)
		{
			// Swap the vtkImageData RGB values with an equivalent QColor
			*(rgbPtr++) = QColor(colorsPtr[0], colorsPtr[1], colorsPtr[2]).rgb();
			colorsPtr += imageData->GetNumberOfScalarComponents();
		}

		rgbPtr -= width * 2;
	}

	return image;
}

/////////////////////////////////////////////////////////////////////////////////////////

void lavtkViewImage2D::SetImage(vtkImageData* image)
{
// 	auto pSM = vtkImageSliceMapper::New();
// 	pSM->SetInputData(image);
// 	pSM->Update();
// 
// // 	auto pSlice = vtkImageSlice::New();
// // 	pSlice->SetMapper(pSM);


	//m_pPrivate->m_Image = vtkImageDataToQImage(image);

	auto pSM = vtkImageImport::New();
	pSM->SetInformation(image->GetInformation());
	pSM->SetImportVoidPointer(image->GetScalarPointer());

	int extent[6];

	image->GetExtent(extent);
	pSM->SetWholeExtent(extent);
	pSM->SetDataExtent(extent);
	

	pSM->Update();

	SuperClass::SetInput(pSM->GetOutputPort());
}

/////////////////////////////////////////////////////////////////////////////////////////

lavtkViewImage2D::lavtkViewImage2D()
{
	this->CornerAnnotation = lavtkImageViewCornerAnnotation::New();

	this->CornerAnnotation->SetNonlinearFontScaleFactor(0.3);
	this->CornerAnnotation->SetTextProperty(this->TextProperty);
	this->CornerAnnotation->SetMaximumFontSize(46);
	this->CornerAnnotation->SetImageView(this);
	this->CornerAnnotation->PickableOff();
	this->CornerAnnotation->SetText(3, "<patient>\n<study>\n<series>");

	this->initializeVTKObjects();

	m_pPrivate = new laPrivate(this);

	this->m_pPrivate->m_SliceChangedCommand = SliceChangedCommand::New();
	this->m_pPrivate->m_SliceChangedCommand->SetViewer(this);
	this->AddObserver(VTK2DViewImage::ViewImagePositionChangeEvent,
		this->m_pPrivate->m_SliceChangedCommand);

	m_pPrivate->setupMainCallBack();

	connect(this, SIGNAL(sliceChanged(int)), this, SLOT(on_slice_changed(int)));
}


/////////////////////////////////////////////////////////////////////////////////////////

void lavtkViewImage2D::laPrivate::setupMainCallBack()
{
	this->m_pCallBack = vtkSmartPointer<lavtkViewImage2DCallback>::New();
	this->m_pCallBack->SetView(m_pPublic);

	if (m_pPublic->GetRenderWindowInteractor())
	{
		vtkInteractorObserver* pInteractorObserver = m_pPublic->GetRenderWindowInteractor()->GetInteractorStyle();
		if (pInteractorObserver) {
			pInteractorObserver->AddObserver(vtkCommand::KeyPressEvent, this->m_pCallBack, -10.0);
			pInteractorObserver->AddObserver(vtkImageView2DCommand::ZSliceMoveEvent, this->m_pCallBack, -10.0);
			pInteractorObserver->AddObserver(vtkImageView2DCommand::ZoomEvent, this->m_pCallBack, -10.0);
		}
	}
	if (m_pPublic->GetRenderWindow() != nullptr) {

		vtkCallbackCommand* m_pModifiedCallback = vtkCallbackCommand::New();
		m_pModifiedCallback->SetCallback(WindowModifiedCallback);
		m_pModifiedCallback->SetClientData(this);

		vtkRenderWindow* pRenderWindow = m_pPublic->GetRenderWindow();
		pRenderWindow->AddObserver(vtkCommand::ModifiedEvent, m_pModifiedCallback);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////

void lavtkViewImage2D::laPrivate::WindowModifiedCallback(vtkObject* caller, long unsigned int eventId, void* clientData, void* callData)
{
	lavtkViewImage2D::laPrivate* pSelf = static_cast<lavtkViewImage2D::laPrivate*>(clientData);
	pSelf->updateCameraScaleBasedOnWindowSize();
}

/////////////////////////////////////////////////////////////////////////////////////////

void lavtkViewImage2D::laPrivate::updateCameraScaleBasedOnWindowSize(bool bForce/*=false*/)
{
	if (m_pPublic != NULL) {
		vtkRenderWindow* pRen = m_pPublic->GetRenderWindow();
		if (pRen != NULL) {
			int nWidth = pRen->GetSize()[0];
			int nHeight = pRen->GetSize()[1];

			QSize sz = QSize(nWidth, nHeight);

			if (nHeight > 0 && ((sz != m_szLast) || (bForce))) {

				int nMin = std::min(nHeight, nWidth);

				m_pPublic->SetZoom(nMin / (1.1 * nHeight));

				if (!m_szLast.isValid()) {
					//if (m_pPublic->isVisible()) {
					m_szLast = sz;
					//m_pPublic->initFirstView();
				//}

				}
				else
					m_szLast = sz;

				//m_pViewer->InvokeEvent(lavtkViewImage2DCommand::WindowSizeChanged);
				//m_pPublic->UpdateAlignment();

				double pan[2] = { 0,0 };
				m_pPublic->SetPan(pan);

			}
		}

	}
}

/////////////////////////////////////////////////////////////////////////////////////////

void lavtkViewImage2D::initializeVTKObjects()
{
	vtkRenderWindowInteractor* iren = vtkRenderWindowInteractor::New();
	vtkRenderWindow* rwin = vtkRenderWindow::New();
	if (rwin) {
		vtkRenderer* pren = vtkRenderer::New();
		rwin->AddRenderer(pren);

		this->SetRenderWindow(rwin);
		this->SetRenderer(pren);

#ifdef USING_VTKINRIA3D

		vtkSmartPointer<VTKInteractorStyleImageView2DSupport> irenStyle1 = vtkSmartPointer<VTKInteractorStyleImageView2DSupport>::New();
		irenStyle1->SetView(this);

		SetupInteractor(iren);
		iren->SetInteractorStyle(irenStyle1);
		SetInteractorStyle(irenStyle1);

		CursorFollowMouseOff();
		ShowRulerWidgetOff();
		ShowScalarBarOff();


#endif //def USING_VTKINRIA3D

		iren->SetRenderWindow(rwin);
		this->SetRenderWindow(rwin);
		this->SetRenderer(pren);

		this->SetBackground(0, 0, 0);

		// Note: These enums do not match the interactors..

		this->SetLeftButtonInteractionStyle(VTK2DViewImage::ZOOM_INTERACTION);
		this->SetMiddleButtonInteractionStyle(VTK2DViewImage::SELECT_INTERACTION);
		this->SetWheelInteractionStyle(VTK2DViewImage::SELECT_INTERACTION);
		this->SetRightButtonInteractionStyle(VTK2DViewImage::WINDOW_LEVEL_INTERACTION);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////

void lavtkViewImage2D::emitQTSliceChanged()
{
	if (m_pPrivate != NULL) {
		VTK2DViewImage* v(VTK2DViewImage::SafeDownCast(this));
		int curslice = v->GetSlice(v->GetOrientation());
		if (curslice == this->m_pPrivate->m_CurrentSlice)
		{
			return;
		}
		this->m_pPrivate->m_CurrentSlice = curslice;
		emit sliceChanged(curslice);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
