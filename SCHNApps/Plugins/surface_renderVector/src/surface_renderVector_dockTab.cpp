#include "surface_renderVector_dockTab.h"

#include "surface_renderVector.h"
#include "schnapps.h"
#include "mapHandler.h"

#include "Utils/vbo.h"

namespace CGoGN
{

namespace SCHNApps
{

Surface_RenderVector_DockTab::Surface_RenderVector_DockTab(SCHNApps* s, Surface_RenderVector_Plugin* p) :
	m_schnapps(s),
	m_plugin(p),
	b_updatingUI(false)
{
	setupUi(this);

	list_vectorVBO->setSelectionMode(QAbstractItemView::SingleSelection);
	slider_vectorsScaleFactor->setDisabled(true);
	combo_color->setDisabled(true);

	connect(combo_positionVBO, SIGNAL(currentIndexChanged(int)), this, SLOT(positionVBOChanged(int)));
	connect(slider_vectorsScaleFactor, SIGNAL(valueChanged(int)), this, SLOT(vectorsScaleFactorChanged(int)));
	connect(list_vectorVBO, SIGNAL(itemChanged(QListWidgetItem*)), this, SLOT(vectorsVBOChecked(QListWidgetItem*)));
	connect(list_vectorVBO, SIGNAL(currentItemChanged(QListWidgetItem*, QListWidgetItem*)), this, SLOT(selectedVectorVBOChanged(QListWidgetItem*, QListWidgetItem*)));

	connect(combo_color, SIGNAL(currentIndexChanged(int)), this, SLOT(colorChanged(int)));
}

void Surface_RenderVector_DockTab::positionVBOChanged(int index)
{
	if (!b_updatingUI)
	{
		View* view = m_schnapps->getSelectedView();
		MapHandlerGen* map = m_schnapps->getSelectedMap();
		if(view && map)
		{
			m_plugin->h_viewParameterSet[view][map].positionVBO = map->getVBO(combo_positionVBO->currentText());
			view->updateGL();
			m_plugin->pythonRecording("changePositionVBO", "", view->getName(), map->getName(), combo_positionVBO->currentText());
		}
	}
}

void Surface_RenderVector_DockTab::selectedVectorVBOChanged(QListWidgetItem* item, QListWidgetItem* old)
{
	if (!b_updatingUI)
	{
		View* view = m_schnapps->getSelectedView();
		MapHandlerGen* map = m_schnapps->getSelectedMap();
		if (view && map)
		{
			if ((item->checkState() == Qt::Checked))
			{
				const MapParameters& p = m_plugin->h_viewParameterSet[view][map];
				Utils::VBO* v = map->getVBO(item->text());
				int idx = p.vectorVBOs.indexOf(v);
				slider_vectorsScaleFactor->setEnabled(true);
				slider_vectorsScaleFactor->setSliderPosition(p.scaleFactors[idx] * 50.0f);
				combo_color->setEnabled(true);
				combo_color->setColor(p.colors[idx]);
			}
			else
			{
				slider_vectorsScaleFactor->setDisabled(true);
				combo_color->setDisabled(true);
			}
		}
	}
}

void Surface_RenderVector_DockTab::vectorsVBOChecked(QListWidgetItem* item)
{
	if (!b_updatingUI)
	{
		View* view = m_schnapps->getSelectedView();
		MapHandlerGen* map = m_schnapps->getSelectedMap();
		if (view && map)
		{
			MapParameters& p = m_plugin->h_viewParameterSet[view][map];
			Utils::VBO* vbo = map->getVBO(item->text());

			if (item->checkState() == Qt::Checked)
			{
				p.vectorVBOs.append(vbo);
				p.scaleFactors.append(1.0f);
				p.colors.append(QColor("red"));
				if (list_vectorVBO->currentItem() == item)
					selectedVectorVBOChanged(item, NULL);
				else
					list_vectorVBO->setCurrentItem(item);
				m_plugin->pythonRecording("addVectorVBO", "", view->getName(), map->getName(), item->text());
			}
			else
			{
				int idx = p.vectorVBOs.indexOf(vbo);
				p.vectorVBOs.removeAt(idx);
				p.scaleFactors.removeAt(idx);
				p.colors.removeAt(idx);
//				list_vectorVBO->setCurrentItem(item);
//				list_vectorVBO->clearSelection();
				slider_vectorsScaleFactor->setDisabled(true);
				combo_color->setDisabled(true);
				m_plugin->pythonRecording("removeVectorVBO", "", view->getName(), map->getName(), QString(item->text()));
			}
			view->updateGL();
		}
	}
}

void Surface_RenderVector_DockTab::vectorsScaleFactorChanged(int i)
{
	if(!b_updatingUI)
	{
		View* view = m_schnapps->getSelectedView();
		MapHandlerGen* map = m_schnapps->getSelectedMap();
		QListWidgetItem* item = list_vectorVBO->currentItem();
		if (view && map && item)
		{
			MapParameters& p = m_plugin->h_viewParameterSet[view][map];
			Utils::VBO* vbo = map->getVBO(item->text());
			int idx = p.vectorVBOs.indexOf(vbo);
			float& scale = p.scaleFactors[idx];
			float newScale = float(i) / 50.0f;
			if (fabs(scale - newScale) > 0.01f)
			{
				scale = newScale;
				m_plugin->pythonRecording("changeVectorScaleFactor", "", view->getName(), map->getName(),
					item->text(), newScale);
				view->updateGL();
			}
		}
	}
}

void Surface_RenderVector_DockTab::colorChanged(int i)
{
	if (!b_updatingUI)
	{
		View* view = m_schnapps->getSelectedView();
		MapHandlerGen* map = m_schnapps->getSelectedMap();
		QListWidgetItem* item = list_vectorVBO->currentItem();
		if (view && map && item)
		{
			MapParameters& p = m_plugin->h_viewParameterSet[view][map];
			Utils::VBO* vbo = map->getVBO(item->text());
			int idx = p.vectorVBOs.indexOf(vbo);
			QColor& col = p.colors[idx];
			if (col != combo_color->color())
			{
				col = combo_color->color();
				m_plugin->pythonRecording("changeVectorColor", "", view->getName(), map->getName(),
					item->text(), combo_color->color().name());
				view->updateGL();
			}
		}
	}
}





void Surface_RenderVector_DockTab::addPositionVBO(QString name)
{
	b_updatingUI = true;
	combo_positionVBO->addItem(name);
	b_updatingUI = false;
}

void Surface_RenderVector_DockTab::removePositionVBO(QString name)
{
	b_updatingUI = true;
	int curIndex = combo_positionVBO->currentIndex();
	int index = combo_positionVBO->findText(name, Qt::MatchExactly);
	if(curIndex == index)
		combo_positionVBO->setCurrentIndex(0);
	combo_positionVBO->removeItem(index);
	b_updatingUI = false;
}

void Surface_RenderVector_DockTab::addVectorVBO(QString name)
{
	b_updatingUI = true;
	list_vectorVBO->addItem(name);
	QListWidgetItem* item = list_vectorVBO->item(list_vectorVBO->count() - 1);
	item->setFlags(item->flags() | Qt::ItemIsEditable);
	item->setCheckState(Qt::Unchecked);
	b_updatingUI = false;
}

void Surface_RenderVector_DockTab::removeVectorVBO(QString name)
{
	b_updatingUI = true;
	QList<QListWidgetItem*> vbo = list_vectorVBO->findItems(name, Qt::MatchExactly);
	if(!vbo.empty())
		delete vbo[0];
	b_updatingUI = false;
}

void Surface_RenderVector_DockTab::updateMapParameters()
{
	b_updatingUI = true;

	combo_positionVBO->clear();
	combo_positionVBO->addItem("- select VBO -");

	list_vectorVBO->clear();
	list_vectorVBO->clearSelection();

	View* view = m_schnapps->getSelectedView();
	MapHandlerGen* map = m_schnapps->getSelectedMap();
	if(view && map)
	{
		const MapParameters& p = m_plugin->h_viewParameterSet[view][map];

		unsigned int i = 1;
		foreach(Utils::VBO* vbo, map->getVBOSet().values())
		{
			if(vbo->dataSize() == 3)
			{
				combo_positionVBO->addItem(QString::fromStdString(vbo->name()));
				if(vbo == p.positionVBO)
					combo_positionVBO->setCurrentIndex(i);

				list_vectorVBO->addItem(QString::fromStdString(vbo->name()));
				QListWidgetItem* item = list_vectorVBO->item(list_vectorVBO->count() - 1);
				item->setFlags(item->flags() | Qt::ItemIsEditable);
				item->setCheckState(Qt::Unchecked);
				if(p.vectorVBOs.contains(vbo))
					list_vectorVBO->item(i-1)->setCheckState(Qt::Checked);

				++i;
			}
		}
	}

	slider_vectorsScaleFactor->setDisabled(true);
	combo_color->setDisabled(true);

	b_updatingUI = false;
}

} // namespace SCHNApps

} // namespace CGoGN
