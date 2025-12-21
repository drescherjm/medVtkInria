#pragma once

#ifndef FUNCTIONPROFILER_H
#define FUNCTIONPROFILER_H

/////////////////////////////////////////////////////////////////////////////////////////\

#include <QMetaObject>
#include <QDebug>
#include <QTime>

/////////////////////////////////////////////////////////////////////////////////////////\

class FunctionProfiler
{
public:
	FunctionProfiler(const QString& func) {
		m_function = QMetaObject::normalizedSignature(qPrintable(func));
		m_function = m_function.remove("__thiscall");
		m_function = m_function.remove("__stdcall");
		m_function = m_function.remove("__cdecl");

		m_startTime = QTime::currentTime();
	}

	~FunctionProfiler() {
		qint32 msecs = m_startTime.msecsTo(QTime::currentTime());
		qDebug("PROFILE: (%5d) %s \n", msecs, qPrintable(m_function));
	}

private:
	QString m_function;
	QTime m_startTime;
};

#define PROFILE_THIS_FUNCTION FunctionProfiler functionProfiler(Q_FUNC_INFO);

/////////////////////////////////////////////////////////////////////////////////////////\

#endif // FUNCTIONPROFILER_H

