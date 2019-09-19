/////////////////////////////////////////////////////////////////////////////////////////


class SliceChangedCommand : public vtkCommand
{
public:
	SliceChangedCommand() : m_Viewer(0)
	{
	}
	static SliceChangedCommand *New();
	void SetViewer(lavtkViewImage2D *viewer)
	{
		this->m_Viewer = viewer;
	}
	virtual void Execute(vtkObject *,
		unsigned long ,
		void *);
private:
	lavtkViewImage2D *m_Viewer;
};

/////////////////////////////////////////////////////////////////////////////////////////

SliceChangedCommand *
SliceChangedCommand::New()
{
	return new SliceChangedCommand();
}

/////////////////////////////////////////////////////////////////////////////////////////

void
SliceChangedCommand::Execute(vtkObject *,
		unsigned long ,
		void *)
{
	this->m_Viewer->emitQTSliceChanged();
}