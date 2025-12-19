#pragma once

#ifndef SMFUNCTIONPROFILER_H
#define SMFUNCTIONPROFILER_H

#include <QTime>
#include <QMetaObject>

/////////////////////////////////////////////////////////////////////////////////////////

class smFunctionProfiler
{
public:
	smFunctionProfiler(const QString& func) {
		m_function = QMetaObject::normalizedSignature(qPrintable(func));
		m_function = m_function.remove("__thiscall");
		m_function = m_function.remove("__stdcall");
		m_function = m_function.remove("__cdecl");

		m_startTime = QTime::currentTime();
	}

	~smFunctionProfiler() {
		qint32 msecs = m_startTime.msecsTo(QTime::currentTime());
		//qInfo("PROFILE: (%5d) %s", msecs, qPrintable(m_function));

		//QLOG_INFO() << QString("PROFILE: %1 msecs - %2").arg(msecs).arg(m_function);

		std::cout << qPrintable(QString("PROFILE: %1 msecs - %2").arg(msecs).arg(m_function));
	}

private:
	QString m_function;
	QTime m_startTime;
};

/////////////////////////////////////////////////////////////////////////////////////////

#define PROFILE_THIS_FUNCTION smFunctionProfiler functionProfiler(Q_FUNC_INFO);

/////////////////////////////////////////////////////////////////////////////////////////

#endif // SMFUNCTIONPROFILER_H