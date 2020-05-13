/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: VPlug_GeoShapes.cpp                                  */
/*    DATE: May 19th, 2011                                       */
/*                                                               */
/* This program is free software; you can redistribute it and/or */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation; either version  */
/* 2 of the License, or (at your option) any later version.      */
/*                                                               */
/* This program is distributed in the hope that it will be       */
/* useful, but WITHOUT ANY WARRANTY; without even the implied    */
/* warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR       */
/* PURPOSE. See the GNU General Public License for more details. */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with this program; if not, write to the Free    */
/* Software Foundation, Inc., 59 Temple Place - Suite 330,       */
/* Boston, MA 02111-1307, USA.                                   */
/*****************************************************************/

#include "VPlug_GeoShapesMap.h"

using namespace std;

//----------------------------------------------------------------
// Constructor

VPlug_GeoShapesMap::VPlug_GeoShapesMap()
{
  m_xmin = 0;
  m_xmax = 0;
  m_ymin = 0;
  m_ymax = 0;
}

//----------------------------------------------------------------
// Procedure: clear()

void VPlug_GeoShapesMap::clear(const string& vname)
{
  if((vname == "") || (vname == "all")) {
    m_xmin = 0;
    m_xmax = 0;
    m_ymin = 0;
    m_ymax = 0;
    m_geoshapes_map.clear();
    m_vnames.clear();
  }
  else
    m_geoshapes_map[vname].clear();

}

//----------------------------------------------------------------
// Procedure: addGeoShape()

bool VPlug_GeoShapesMap::addGeoShape(const string& param, 
				     const string& value, 
				     const string& vname,
				     double timestamp)
{
  bool handled = false;
  unsigned int starting_map_size = m_geoshapes_map.size();

  string lparam = toupper(param);
  if(lparam == "VIEW_POLYGON")
    handled = m_geoshapes_map[vname].addPolygon(value);
  else if(lparam == "VIEW_SEGLIST")
    handled = m_geoshapes_map[vname].addSegList(value);
  else if(lparam == "VIEW_POINT")
    handled = m_geoshapes_map[vname].addPoint(value);
  else if(lparam == "VIEW_VECTOR")
    handled = m_geoshapes_map[vname].addVector(value);
  else if(lparam == "VIEW_CIRCLE")
    handled = m_geoshapes_map[vname].addCircle(value);
  else if(lparam == "VIEW_RANGE_PULSE")
    handled = m_geoshapes_map[vname].addRangePulse(value, timestamp);
  else if(lparam == "VIEW_COMMS_PULSE")
    handled = m_geoshapes_map[vname].addCommsPulse(value, timestamp);
  else if((lparam == "VIEW_MARKER") || (lparam == "MARKER"))
    handled = m_geoshapes_map[vname].addMarker(value);
  else if(lparam == "grid_CONFIG")
    handled = m_geoshapes_map[vname].addGrid(value);
  else if(lparam == "grid_DELTA")
    handled = m_geoshapes_map[vname].updateGrid(value);
  else if(lparam == "VIEW_GRID")
    handled = m_geoshapes_map[vname].addConvexGrid(value);

  if(handled)
    updateBounds(m_geoshapes_map[vname]);

  unsigned int new_map_size = m_geoshapes_map.size();
  if(new_map_size > starting_map_size)
    refreshVehiNames();

  return(handled);
}


//----------------------------------------------------------------
// Procedure: getPolygons
// Procedure: getSegLists
// Procedure: getHexagons
// Procedure: getGrids
// Procedure: getCircles
// Procedure: getPoints
// Procedure: getVectors
// Procedure: getRangePulses
// Procedure: getCommsPulses
// Procedure: getMarkers

vector<XYPolygon> VPlug_GeoShapesMap::getPolygons(const string& vname)
{
  return(m_geoshapes_map[vname].getPolygons());
}
vector<XYSegList> VPlug_GeoShapesMap::getSegLists(const string& vname)
{
  return(m_geoshapes_map[vname].getSegLists());
}
vector<XYHexagon> VPlug_GeoShapesMap::getHexagons(const string& vname)
{
  return(m_geoshapes_map[vname].getHexagons());
}
vector<XYGrid> VPlug_GeoShapesMap::getGrids(const string& vname)
{
  return(m_geoshapes_map[vname].getGrids());
}
vector<XYConvexGrid> VPlug_GeoShapesMap::getConvexGrids(const string& vname)
{
  return(m_geoshapes_map[vname].getConvexGrids());
}
const map<string, XYCircle>& VPlug_GeoShapesMap::getCircles(const string& vname)
{
  return(m_geoshapes_map[vname].getCircles());
}
const map<string, XYPoint>& VPlug_GeoShapesMap::getPoints(const string& vname)
{
  return(m_geoshapes_map[vname].getPoints());
}
vector<XYVector> VPlug_GeoShapesMap::getVectors(const string& vname)
{
  return(m_geoshapes_map[vname].getVectors());
}
vector<XYRangePulse> VPlug_GeoShapesMap::getRangePulses(const string& vname)
{
  return(m_geoshapes_map[vname].getRangePulses());
}
vector<XYCommsPulse> VPlug_GeoShapesMap::getCommsPulses(const string& vname)
{
  return(m_geoshapes_map[vname].getCommsPulses());
}
const map<string, XYMarker>& VPlug_GeoShapesMap::getMarkers(const string& vname)
{
  return(m_geoshapes_map[vname].getMarkers());
}


//----------------------------------------------------------------
// Procedure: size

unsigned int VPlug_GeoShapesMap::size(const string& gtype) const
{
  unsigned int return_size = 0;
  map<string, VPlug_GeoShapes>::const_iterator p;
  for(p=m_geoshapes_map.begin(); p!= m_geoshapes_map.end(); p++) {
    if(gtype == "markers")
      return_size += p->second.sizeMarkers();
    else if(gtype == "range_pulses")
      return_size += p->second.sizeRangePulses();
    else if(gtype == "comms_pulses")
      return_size += p->second.sizeCommsPulses();
    else if(gtype == "points")
      return_size += p->second.sizePolygons();
    else if(gtype == "polygons")
      return_size += p->second.sizePolygons();
    else if(gtype == "seglists")
      return_size += p->second.sizeSegLists();
    else if(gtype == "vectors")
      return_size += p->second.sizeVectors();
    else if(gtype == "circles")
      return_size += p->second.sizeCircles();
    else if(gtype == "grids")
      return_size += p->second.sizeGrids();
    else if(gtype == "convex_grids")
      return_size += p->second.sizeConvexGrids();
    else if(gtype == "hexagons")
      return_size += p->second.sizeHexagons();
  }  

  return(return_size);
}

//----------------------------------------------------------------
// Procedure: refreshVehiNames

void VPlug_GeoShapesMap::refreshVehiNames()
{
  m_vnames.clear();
  map<string, VPlug_GeoShapes>::const_iterator p;
  for(p=m_geoshapes_map.begin(); p!= m_geoshapes_map.end(); p++)
    m_vnames.push_back(p->first);
}

//----------------------------------------------------------------
// Procedure: updateBounds()

void VPlug_GeoShapesMap::updateBounds(const VPlug_GeoShapes& vplug)
{
  if(vplug.getXMin() < m_xmin)
    m_xmin = vplug.getXMin();
  if(vplug.getXMax() > m_xmax)
    m_xmax = vplug.getXMax();

  if(vplug.getYMin() < m_ymin)
    m_ymin = vplug.getYMin();
  if(vplug.getYMax() > m_ymax)
    m_ymax = vplug.getYMax();
}


