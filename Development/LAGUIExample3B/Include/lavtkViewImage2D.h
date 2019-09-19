#pragma once

#ifndef LAVTKVIEWIMAGE2D_H
#define LAVTKVIEWIMAGE2D_H

//#define PREV_VTKRENDERINGADDON_COMPATIBILITY
//#define USING_VTKINRIA3D

#include <vtkImageView2DExtended.h>
#include <QObject>

/////////////////////////////////////////////////////////////////////////////////////////

class lavtkViewImage2D : public QObject, public vtkImageView2DExtended
{
	Q_OBJECT
	typedef vtkImageView2DExtended SuperClass;
public:
	static lavtkViewImage2D* New();
	vtkTypeMacro(lavtkViewImage2D, vtkImageView2DExtended);
public:
	void SetImage(vtkImageData* image);
	void emitQTSliceChanged();

signals:
	void	windowChanged(double);
	void	levelChanged(double);
	void	sliceChanged(int);

private:
	lavtkViewImage2D();
	void initializeVTKObjects();

private:
	class		laPrivate;
	laPrivate* m_pPrivate;
};

/////////////////////////////////////////////////////////////////////////////////////////

#endif // LAVTKVIEWIMAGE2D_H
