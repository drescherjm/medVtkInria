#pragma once

#ifndef QTVTKRENDERWINDOWS_H
#define QTVTKRENDERWINDOWS_H

/////////////////////////////////////////////////////////////////////////////////////////

#include "vtkSmartPointer.h"
//#include "vtkResliceImageViewer.h"
#include "vtkImagePlaneWidget.h"
#include "vtkDistanceWidget.h"
//#include "vtkResliceImageViewerMeasurements.h"
#include <QMainWindow>
#include "vtkImageView2DExtended.h"
#include "smvtkImageView2D.h"

#include <memory>

class smDicomReader;

// Forward Qt class declarations
class Ui_QtVTKRenderWindow;

//using VTKView = vtkImageView2DExtended;
using VTKView = smvtkImageView2D;

class QtVTKRenderWindows : public QMainWindow
{
  Q_OBJECT
public:

  // Constructor/Destructor
  QtVTKRenderWindows(int argc, char *argv[]);
  ~QtVTKRenderWindows();

public slots:

  virtual void slotExit();
  virtual void resliceMode(int);
  virtual void thickMode(int);
  virtual void SetBlendModeToMaxIP();
  virtual void SetBlendModeToMinIP();
  virtual void SetBlendModeToMeanIP();
  virtual void SetBlendMode(int);
  virtual void ResetViews();
  virtual void Render();
  virtual void AddDistanceMeasurementToView1();
  //virtual void AddDistanceMeasurementToView( int );

protected:
  vtkSmartPointer< VTKView > riw;

  virtual void showEvent(QShowEvent* event) override;


  virtual void resizeEvent(QResizeEvent* event) override;

  // vtkSmartPointer< vtkResliceImageViewerMeasurements > ResliceMeasurements;

protected slots:
	void on_spinBoxCamera_valueChanged(int nValue);
	void on_pushButtonHorizontal_clicked(bool);
	void on_pushButtonHorizontalAndVertical_clicked(bool);
	void on_pushButtonVertical_clicked(bool);
	void on_pushButtonAlignLeft_clicked(bool);
	void on_pushButtonAlignRight_clicked(bool);
	void on_pushButtonComputeScale_clicked(bool);

	void conventionSpinChanged();

	void initConventionSpinBoxes();

private:
	void setupImage();
	void updateInformation();
	void addViewConventionMatrix();
	int	 getProperViewConventionForImage();
	QString getViewConventionSpinName(int i, int j);

	vtkSmartPointer<vtkMatrix4x4>	getOrientationMatrixForImage(std::string strLaterality, std::string strMQCMCode, vtkMatrix4x4* pPatientMatrix);
	int								getProperViewConventionForImage(std::string strLaterality, std::string strMQCMCode, vtkMatrix4x4* pPatientMatrix);
private:
	bool m_bConventionInitialized = false;
	std::shared_ptr<smDicomReader>	m_pReader;
  // Designer form
  Ui_QtVTKRenderWindow *ui;
};

/////////////////////////////////////////////////////////////////////////////////////////

#endif // QTVTKRENDERWINDOWS_H
