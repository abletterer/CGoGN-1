#ifndef _VOLUME_IMPORT_PLUGIN_H_
#define _VOLUME_IMPORT_PLUGIN_H_

#include "dll.h"
#include "plugin_processing.h"

namespace CGoGN
{

namespace SCHNApps
{

class VOLUME_IMPORT_API Volume_Import_Plugin : public PluginProcessing
{
	Q_OBJECT
	Q_INTERFACES(CGoGN::SCHNApps::Plugin)
#if CGOGN_QT_DESIRED_VERSION == 5
	Q_PLUGIN_METADATA(IID "CGoGN.SCHNapps.Plugin")
#endif
public:
	Volume_Import_Plugin()
	{
		std::cout << "VOL IMPORT CONSTR" << std::endl;
		std::cout << "&PTR="<< &(GenericMap::s_instances)<< std::endl;
		std::cout << "PTR="<< GenericMap::s_instances<< std::endl;

	}

	~Volume_Import_Plugin()
	{}

private:
	virtual bool enable();
	virtual void disable() {}

public slots:
	MapHandlerGen* importFromFile(const QString& fileName);
	void importFromFileDialog();

private:
	QAction* importAction;
};

} // namespace SCHNApps

} // namespace CGoGN

#endif
