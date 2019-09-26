#ifndef __LUNGAPP_H__
#define __LUNGAPP_H__

#include <QApplication>

class laProgressManager;

/////////////////////////////////////////////////////////////////////////////////////////

namespace AppUi {
	class LungApp : public QApplication
	{
		typedef QApplication Superclass;
	Q_OBJECT
	public:
		LungApp(int &argc, char **argv, int = QT_VERSION);
	public:
		laProgressManager*		getProgressManager();
		bool					executingABatch();
	};
}

/////////////////////////////////////////////////////////////////////////////////////////

#endif //__LUNGAPP_H__
