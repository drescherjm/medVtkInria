#pragma once

#ifndef SMVTKINTERACTORSTYLE2D_H
#define SMVTKINTERACTORSTYLE2D_H

#include "smvtkInteractorStyleImageView2D.h"

/////////////////////////////////////////////////////////////////////////////////////////

class smvtkInteractorStyle2D : public smvtkInteractorStyleImageView2D
{
public:
	static smvtkInteractorStyle2D *New();
	vtkTypeMacro (smvtkInteractorStyle2D, smvtkInteractorStyleImageView2D);

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

/////////////////////////////////////////////////////////////////////////////////////////

#endif // SMVTKINTERACTORSTYLE2D_H
