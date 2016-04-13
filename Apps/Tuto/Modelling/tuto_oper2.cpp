/*******************************************************************************
* CGoGN: Combinatorial and Geometric modeling with Generic N-dimensional Maps  *
* version 0.1                                                                  *
* Copyright (C) 2009-2012, IGG Team, LSIIT, University of Strasbourg           *
*                                                                              *
* This library is free software; you can redistribute it and/or modify it      *
* under the terms of the GNU Lesser General Public License as published by the *
* Free Software Foundation; either version 2.1 of the License, or (at your     *
* option) any later version.                                                   *
*                                                                              *
* This library is distributed in the hope that it will be useful, but WITHOUT  *
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or        *
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License  *
* for more details.                                                            *
*                                                                              *
* You should have received a copy of the GNU Lesser General Public License     *
* along with this library; if not, write to the Free Software Foundation,      *
* Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA.           *
*                                                                              *
* Web site: http://cgogn.unistra.fr/                                           *
* Contact information: cgogn@unistra.fr                                        *
*                                                                              *
*******************************************************************************/

#include "tuto_oper2.h"
#include "Algo/Geometry/boundingbox.h"
#include "Algo/Tiling/Surface/square.h"
#include "Algo/Geometry/centroid.h"
#include "Algo/Import/import.h"
#include "Algo/Export/export.h"

using namespace CGoGN ;

int main(int argc, char **argv)
{
	//	// interface
	QApplication app(argc, argv);
	MyQT sqt;


    sqt.setDock(& sqt.dock);
    sqt.setCallBack( sqt.dock.listOper, SIGNAL(currentRowChanged(int)), SLOT(operation(int)) );
    sqt.setCallBack( sqt.dock.svg, SIGNAL(clicked()), SLOT(svg()) );
    sqt.setCallBack( sqt.dock.widthSlider, SIGNAL(valueChanged(int)), SLOT(width(int)) );
	int n=3;
	if (argc==2)
		n = atoi(argv[1]);

	// example code itself
	sqt.createMap(n);

	sqt.width(4.0);

	// set help message in menu
	sqt.setHelpMsg("First Tuto: \nCreate two faces\nsew them\nand affect positions");
	// final show for redraw
	sqt.show();
	// and wait for the end
	return app.exec();
}

void MyQT::operation(int x)
{
	switch(x)
	{
	case 0:
		CGoGNout <<"split vertex"<<CGoGNendl;
		if ((m_selected != NIL) && (m_selected2 != NIL))
		{
			dm.markAll();
			myMap.splitVertex(m_selected,m_selected2);
			PFP::VEC3 c1 = Algo::Surface::Geometry::faceCentroid<PFP>(myMap, m_selected, position);
			PFP::VEC3 c2 = Algo::Surface::Geometry::faceCentroid<PFP>(myMap, m_selected2, position);
			position[m_selected] = position[m_selected] * 0.7f + c1*0.3f;
			position[m_selected2] = position[m_selected2] * 0.7f + c2*0.3f;
			updateMap();
		}
		break;
	case 1:
		CGoGNout <<"delete vertex"<<CGoGNendl;
		if (m_selected != NIL)
		{
			dm.markAll();
			m_selected = myMap.deleteVertex(m_selected);
			updateMap();
		}
		break;
	case 2:
		CGoGNout <<"cut edge"<<CGoGNendl;
		if (m_selected != NIL)
		{
			dm.markAll();
			PFP::VEC3 Q = position[myMap.phi1(m_selected)];
			myMap.cutEdge(m_selected);
			position[myMap.phi1(m_selected)] = (position[m_selected] + Q)/2.0f;
			updateMap();
		}
		break;
	case 3:
		CGoGNout <<"uncut edge"<<CGoGNendl;
		if (m_selected != NIL)
		{
			myMap.uncutEdge(m_selected);
			updateMap();
		}
		break;
	case 4:
		CGoGNout <<"collapse edge"<<CGoGNendl;
		if (m_selected != NIL)
		{
			PFP::VEC3 M = (position[m_selected] + position[myMap.phi1(m_selected)])/2.0f;
			m_selected = myMap.collapseEdge(m_selected,true);
			position[m_selected] = M;
			updateMap();
		}
		break;
	case 5:
		CGoGNout <<"flip edge"<<CGoGNendl;
		if (m_selected != NIL)
		{
			myMap.flipEdge(m_selected);
			updateMap();
		}
		break;
	case 6:
		CGoGNout <<"flip back edge"<<CGoGNendl;
		if (m_selected != NIL)
		{
			myMap.flipBackEdge(m_selected);
			updateMap();
		}
		break;
	case 7:
		CGoGNout <<"split face"<<CGoGNendl;
		if ((m_selected != NIL) && (m_selected2 != NIL) && myMap.sameFace(m_selected,m_selected2))
		{
			myMap.splitFace(m_selected,m_selected2);
			updateMap();
		}
		break;
	case 8:
		CGoGNout <<"merge face"<<CGoGNendl;
		if (m_selected != NIL)
		{
			myMap.mergeFaces(m_selected);
			updateMap();
			m_selected=NIL;
		}
		break;
	case 9:
		CGoGNout <<"delete face"<<CGoGNendl;
		if (m_selected != NIL)
		{
			myMap.deleteFace(m_selected);
			updateMap();
			m_selected=NIL;
			m_selected2=NIL;
		}
		break;
	case 10:
		CGoGNout <<"split surface"<<CGoGNendl;
		if (!m_selecteds.empty())
		{
			myMap.splitSurface(m_selecteds);
			updateMap();
			m_selected=NIL;
			m_selected2=NIL;
			m_selecteds.clear();
		}
		break;

	default:
		break;
	}

	updateGL();
	dock.listOper->setCurrentRow(-1);
}

void MyQT::createMap(int n)
{
	position = myMap.addAttribute<VEC3, VERTEX, MAP>("position");

    Algo::Surface::Tilings::Square::Grid<PFP> grid(myMap, n, n, true);
    grid.embedIntoGrid(position, 1.,1.,0.);

    //  bounding box of scene
	Geom::BoundingBox<PFP::VEC3> bb = Algo::Geometry::computeBoundingBox<PFP>(myMap, position) ;
	setParamObject(bb.maxSize(), bb.center().data()) ;
	m_shift = bb.maxSize()/200.0f;

	// first show for be sure that GL context is binded
	show();

	// render the topo of the map without boundary darts

	dm.markAll();

	m_render_topo->setDartWidth(4.0f);
	m_render_topo->setInitialDartsColor(0.0f,0.0f,0.0f);
	m_render_topo->setInitialBoundaryDartsColor(0.0f,0.0f,0.0f);

#ifdef PRIMAL_TOPO
	m_render_topo->updateData<PFP>(myMap, position, 0.9);
#else
   m_render_topo->updateData<PFP>(myMap, position, 0.9f, 0.9f,true);
#endif

}

void MyQT::updateMap()
{
	m_render_topo->setInitialBoundaryDartsColor(0.0f,0.0f,0.0f);
#ifdef PRIMAL_TOPO
	m_render_topo->updateData<PFP>(myMap, position, 0.9);
#else
	m_render_topo->updateData<PFP>(myMap, position, 0.9f, 0.9f,true);
#endif

}

// initialization GL callback
void MyQT::cb_initGL()
{
	glClearColor(1.0f,1.0f,1.0f,1.0f);
#ifdef PRIMAL_TOPO
	m_render_topo = new Algo::Render::GL2::TopoPrimalRender() ;
#else
	m_render_topo = new Algo::Render::GL2::TopoRender(0.01f) ;
#endif
}

// redraw GL callback (clear and swap already done)
void MyQT::cb_redraw()
{
	glDepthFunc(GL_LESS);
	m_render_topo->drawTopo();

	if (m_selected != NIL)
		m_render_topo->overdrawDart(myMap, m_selected, 11, 1.0f, 0.0f, 0.0f);

	if (m_selected2 != NIL)
		m_render_topo->overdrawDart(myMap, m_selected2, 11, 0.0f, 1.0f, 0.0f);

	if(!m_selecteds.empty())
	{
		for(std::vector<Dart>::iterator it = m_selecteds.begin() ; it != m_selecteds.end() ; ++it)
			m_render_topo->overdrawDart(myMap, *it, 11, 0.0f, 0.0f, 1.0f);
	}

		

}

void MyQT::cb_mousePress(int button, int x, int y)
{
	if (Shift())
	{
		Dart d = m_render_topo->picking(myMap, x,y); // nb
		if (button == Qt::LeftButton)
		{
			if (d != Dart::nil())
				m_selected = d;
		}
		if (button == Qt::RightButton)
		{
			if (d != Dart::nil())
				m_selected2 = d;
		}
		updateGL();
	}
	if(Control())
	{
		Dart d = m_render_topo->picking(myMap, x,y); // nb
		if (button == Qt::LeftButton)
		{
			if (d == Dart::nil())
				m_selecteds.clear();
			else
				m_selecteds.push_back(d);
		}
	}
}

void MyQT::cb_keyPress(int keycode)
{
	switch(keycode)
	{
	case Qt::Key_Up:
		if (m_selected!=NIL)
			position[m_selected][1] += m_shift;
		updateMap();
		updateGL();
		break;
	case Qt::Key_Down:
		if (m_selected!=NIL)
			position[m_selected][1] -= m_shift;
		updateMap();
		updateGL();
		break;
	case Qt::Key_Left:
		if (m_selected!=NIL)
			position[m_selected][0] -= m_shift;
		updateMap();
		updateGL();
		break;
	case Qt::Key_Right:
		if (m_selected!=NIL)
			position[m_selected][0] += m_shift;
		updateMap();
		updateGL();
		break;
	case '1':
		if (m_selected!=NIL)
			m_selected = myMap.phi1(m_selected);
		updateMap();
		updateGL();
		break;
	case '2':
		if (m_selected!=NIL)
			m_selected = myMap.phi2(m_selected);
		updateMap();
		updateGL();
		break;

	case ' ':
		m_selected=NIL;
		m_selected2=NIL;
		updateMap();
		updateGL();
		break;

	}
	updateGL();
}

void MyQT::svg()
{
	std::string filename = selectFileSave("snapshot file", ".", "(*.svg)");
	m_render_topo->svgout2D(filename, modelViewMatrix(),projectionMatrix());
}

void MyQT::cb_Open()
{
	std::string filters("all (*.*);; trian (*.trian);; off (*.off);; ply (*.ply);; map (*.map)") ;
	std::string filename = selectFile("Open Mesh", "", filters) ;
	if (!filename.empty())
		importMesh(filename);
	updateGL();
}

void MyQT::cb_Save()
{
	std::string filename = selectFileSave("Export Off file ",".","(*.off)");
	Algo::Surface::Export::exportOFF<PFP>(myMap,position,filename.c_str());
	//std::cout << "could not save file : " << filename << std::endl;
}

void MyQT::importMesh(std::string& filename)
{
	myMap.clear(true) ;

	size_t pos = filename.rfind(".");    // position of "." in filename
	std::string extension = filename.substr(pos);

	if (extension == std::string(".map"))
	{
		myMap.loadMapBin(filename);
		position = myMap.getAttribute<VEC3, VERTEX, MAP>("position") ;
	}
	else
	{
		std::vector<std::string> attrNames ;
		if(!Algo::Surface::Import::importMesh<PFP>(myMap, filename.c_str(), attrNames))
		{
			CGoGNerr << "could not import " << filename << CGoGNendl ;
			return;
		}
		position = myMap.getAttribute<VEC3, VERTEX, MAP>(attrNames[0]) ;
	}


	m_selected  = NIL;
	m_selected2 = NIL;

	Geom::BoundingBox<PFP::VEC3> bb = Algo::Geometry::computeBoundingBox<PFP>(myMap, position) ;
	setParamObject(bb.maxSize(), bb.center().data()) ;
	m_shift = bb.maxSize()/200.0f;

	updateMap();
	updateGLMatrices() ;
}

void MyQT::width(int w)
{
	m_render_topo->setDartWidth(w);
	m_render_topo->setRelationWidth(w);
	updateGL();
}
