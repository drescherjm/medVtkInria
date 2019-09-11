#ifndef __SMVTKINTERACTORSTYLE2D_H__
#define __SMVTKINTERACTORSTYLE2D_H__

#include "smvtkInteractorStyleImageView2D.h"

class smvtkInteractorStyle2D : public smvtkInteractorStyleImageView2D
{
public:
	static smvtkInteractorStyle2D *New();
	vtkTypeRevisionMacro (smvtkInteractorStyle2D, smvtkInteractorStyleImageView2D);

protected:
	smvtkInteractorStyle2D();
	~smvtkInteractorStyle2D();

private:
	class smPrivate;
	smPrivate* m_pPrivate;

private: 
	smvtkInteractorStyle2D(const smvtkInteractorStyle2D&);  // Intentionally not implemented.
	void operator=(const smvtkInteractorStyle2D&);					// Intentionally not implemented.

};

#endif //__SMVTKINTERACTORSTYLE2D_H__