#ifndef _RENDERVECTOR_DOCK_TAB_H_
#define _RENDERVECTOR_DOCK_TAB_H_

#include "ui_renderVector.h"
#include "Utils/vbo.h"

namespace CGoGN
{

namespace SCHNApps
{

class Window;
class RenderVectorPlugin;
struct ParameterSet;

class RenderVectorDockTab : public QWidget, public Ui::RenderVectorDockWidget
{
	Q_OBJECT

public:
	RenderVectorDockTab(Window* w, RenderVectorPlugin* p);

private:
	Window* m_window;
	RenderVectorPlugin* m_plugin;
	ParameterSet* m_currentParams;
	bool b_refreshingUI;

public slots:
	void refreshUI(ParameterSet* params);

	void selectedMapChanged();

	void positionVBOChanged(int index);
	void selectedVectorsVBOChanged();
	void vectorsScaleFactorChanged(int i);

	void addVBOToList(Utils::VBO* vbo);
};

} // namespace SCHNApps

} // namespace CGoGN

#endif