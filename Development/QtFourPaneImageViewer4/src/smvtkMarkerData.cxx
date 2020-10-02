#include "smGUIPCH.h"

#include "smvtkMarkerData.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "smMarkerInfo.h"
#include "smImageSeriesInfo.h"
#include "smCorrespondingMarkerInfo.h"

#if VTK_MAJOR_VERSION < 6
vtkCxxRevisionMacro(smvtkMarkerData, "$Revision: 4351 $");
#endif

vtkStandardNewMacro(smvtkMarkerData);

/////////////////////////////////////////////////////////////////////////////////////////

class smvtkMarkerData::smPrivate
{
public:
	smMarkerInfo				m_markerInfo;
	smImageSeriesInfo			m_imageSeriesInfo;
	smCorrespondingMarkerInfo	m_correspodingMarkerInfo;
	bool						m_bReadOnly;
	QString						m_strColor;
};

/////////////////////////////////////////////////////////////////////////////////////////

smvtkMarkerData::smvtkMarkerData()
{
	m_pPrivate = new smPrivate();
	m_pPrivate->m_bReadOnly = false;
}

/////////////////////////////////////////////////////////////////////////////////////////

smvtkMarkerData::~smvtkMarkerData()
{
	delete m_pPrivate;
}

/////////////////////////////////////////////////////////////////////////////////////////

qbuPropertyMap* smvtkMarkerData::getMarkerProps()
{
	return (m_pPrivate != nullptr) ? &m_pPrivate->m_markerInfo : nullptr;
}

/////////////////////////////////////////////////////////////////////////////////////////

bool smvtkMarkerData::setMarkerProps( qbuPropertyMap* pProps )
{
	bool retVal = ((m_pPrivate != nullptr) && (pProps != nullptr));
	if (retVal) {
		m_pPrivate->m_markerInfo.addProperties(pProps);
	}
	return retVal;
}

/////////////////////////////////////////////////////////////////////////////////////////

smImageSeriesInfo* smvtkMarkerData::getImageSeriesInfo()
{
	return (m_pPrivate != nullptr) ? &m_pPrivate->m_imageSeriesInfo : nullptr;
}

/////////////////////////////////////////////////////////////////////////////////////////

bool smvtkMarkerData::setImageSeriesInfo(  qbuPropertyMap* pProps )
{
	bool retVal = ((m_pPrivate != nullptr) && (pProps != nullptr));
	if (retVal) {
		m_pPrivate->m_imageSeriesInfo.addProperties(pProps);
	}
	return retVal;
}

/////////////////////////////////////////////////////////////////////////////////////////

bool smvtkMarkerData::isReadOnly()
{
	bool retVal = (m_pPrivate == nullptr);
	if (!retVal) {
		retVal = m_pPrivate->m_bReadOnly;
	}
	return retVal;
}

/////////////////////////////////////////////////////////////////////////////////////////

void smvtkMarkerData::PrintSelf( ostream& os, vtkIndent indent )
{
	//Superclass typedef defined in vtkTypeMacro() found in vtkSetGet.h
	this->Superclass::PrintSelf(os,indent);

	if (m_pPrivate != nullptr) {
		m_pPrivate->m_markerInfo.Print(os);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////

void smvtkMarkerData::setReadOnly( bool bReadOnly )
{
	if (m_pPrivate != nullptr) {
		m_pPrivate->m_bReadOnly = bReadOnly;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////

smCorrespondingMarkerInfo* smvtkMarkerData::getCorrespondingMarkerInfo()
{	
	smCorrespondingMarkerInfo* retVal = nullptr;
	if (m_pPrivate != nullptr) {
		if (!m_pPrivate->m_correspodingMarkerInfo.hasField(smMarkerInfo::g_strMarkerID)) {
			m_pPrivate->m_correspodingMarkerInfo.addProperties(&m_pPrivate->m_markerInfo);
		}
		retVal = &m_pPrivate->m_correspodingMarkerInfo;
	}
	return retVal;
}

/////////////////////////////////////////////////////////////////////////////////////////

QString smvtkMarkerData::getColor()
{
	return (m_pPrivate != nullptr) ? m_pPrivate->m_strColor : QString();
}

/////////////////////////////////////////////////////////////////////////////////////////

void smvtkMarkerData::setColor( QString strColor )
{
	if (m_pPrivate != nullptr) {
		m_pPrivate->m_strColor = strColor;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////

bool smvtkMarkerData::removeCorrespondingMarker()
{
	m_pPrivate->m_correspodingMarkerInfo.RemoveProperty(smCorrespondingMarkerInfo::g_strCorrespondingMarkerID);
	m_pPrivate->m_correspodingMarkerInfo.RemoveProperty(smCorrespondingMarkerInfo::g_strCorrespondingSeriesID);
	return true;
}


