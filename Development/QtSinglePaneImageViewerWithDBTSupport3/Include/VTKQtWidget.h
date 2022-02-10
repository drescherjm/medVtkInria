#include <vtkVersion.h>

#if VTK_MAJOR_VERSION >= 9
#include <QVTKRenderWidget.h>
using VTKQtWidget = QVTKRenderWidget;
#else
#include <QVTKWidget.h>
using VTKQtWidget = QVTKWidget;
#endif
