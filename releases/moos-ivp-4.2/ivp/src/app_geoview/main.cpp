/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: main.cpp                                             */
/*    DATE: May 1st, 2005                                        */
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

#include <vector>
#include "GEO_GUI.h"
#include "MBUtils.h"
#include "ReleaseInfo.h"
#include "IO_GeomUtils.h"
#include "XYPolygon.h"
#include "XYSegList.h"
#include "XYGrid.h"
#include "XYCircle.h"
#include "XYHexagon.h"
#include "LMV_Utils.h"

using namespace std;

//--------------------------------------------------------
// Procedure: idleProc

void idleProc(void *)
{
  Fl::flush();
  millipause(10);
}

//--------------------------------------------------------
// Procedure: main

int main(int argc, char *argv[])
{
  int i;
  unsigned int j;

  // Look for a request for version information
  if(scanArgs(argc, argv, "-v", "--version", "-version")) {
    showReleaseInfo("geoview", "gpl");
    return(0);
  }
  
  // Look for a request for usage information
  if(scanArgs(argc, argv, "-h", "--help", "-help")) {
    cout << "Usage: geoview [filename] [filename]..." << endl;
    return(0);
  }

  Fl::add_idle(idleProc);
  GEO_GUI* gui = new GEO_GUI(900, 800, "geoview");

  string tif_file = "Default.tif";  // default

  for(i=1; i<argc; i++) {
    string argi  = tolower(argv[i]);
    if((argi == "mit") || (argi=="charles"))
      tif_file = "AerialMIT-1024.tif";
    else if((argi == "wmit") || (argi=="wireframe") || (argi=="wf"))
      tif_file = "WireFrameMIT-1024.tif";
    else if((argi == "mb") || (argi=="monterey"))
      tif_file = "Monterey-2048.tif";
    else if((argi == "forrest-lake") || (argi=="fl"))
      tif_file = "forrest19.tif";
    else if((argi == "mbd"))
      tif_file = "Monterey-2048-30-30-100.tif";
  }

  vector<string>    all_poly_strings;
  vector<string>    all_segl_strings;
  vector<string>    all_grid_strings;
  vector<string>    all_circle_strings;
  vector<XYHexagon> all_hexagons;
  vector<string>    all_markers;
  vector<string>    all_opvertices;
  vector<string>    all_geodesy;

  for(i=1; i<argc; i++) {
    string argi  = argv[i];
    
    if(strContains(argi, ".tif"))
      tif_file = argi;
    else if(argi == "-noimg")
      tif_file = "";
    else {
      vector<string> svector;
      svector = readEntriesFromFile(argi, "poly:polygon");
      for(j=0; j<svector.size(); j++)
	all_poly_strings.push_back(svector[j]);

      svector = readEntriesFromFile(argi, "segl:seglist:points");
      for(j=0; j<svector.size(); j++)
	all_segl_strings.push_back(svector[j]);

      svector = readEntriesFromFile(argi, "circle");
      for(j=0; j<svector.size(); j++)
	all_circle_strings.push_back(svector[j]);

      svector = readEntriesFromFile(argi, "grid:xygrid");
      for(j=0; j<svector.size(); j++)
	all_grid_strings.push_back(svector[j]);

      vector<XYHexagon> hvector = readHexagonsFromFile(argi);
      for(j=0; j<hvector.size(); j++)
	all_hexagons.push_back(hvector[j]);
      vector<string> mvector = readEntriesFromFile(argi, "marker");
      for(j=0; j<mvector.size(); j++)
	all_markers.push_back(mvector[j]);
      vector<string> ovector = readEntriesFromFile(argi, "op_vertex");
      for(j=0; j<ovector.size(); j++)
	all_opvertices.push_back(ovector[j]);
      vector<string> dvector = readEntriesFromFile(argi, "geodesy");
      for(j=0; j<dvector.size(); j++)
	all_geodesy.push_back(dvector[j]);
    }
  }
 
  gui->pviewer->setParam("tiff_file", tif_file);

  cout << "# of file polys: " << all_poly_strings.size() << endl;
  for(j=0; j<all_poly_strings.size(); j++)
    gui->pviewer->setParam("view_polygon", all_poly_strings[j]);

  cout << "# of file seglists: " << all_segl_strings.size() << endl;
  for(j=0; j<all_segl_strings.size(); j++)
    gui->pviewer->setParam("view_seglist", all_segl_strings[j]);
  
  cout << "# of file grids: " << all_grid_strings.size() << endl;
  for(j=0; j<all_grid_strings.size(); j++)
    gui->pviewer->setParam("grid", all_grid_strings[j]);
  
  cout << "# of file circles: " << all_circle_strings.size() << endl;
  for(j=0; j<all_circle_strings.size(); j++)
    gui->pviewer->setParam("view_circle", all_circle_strings[j]);
  
#if 0
  cout << "# of file hexagons: " << all_hexagons.size() << endl;
  for(j=0; j<all_hexagons.size(); j++)
    gui->pviewer->addPoly(all_hexagons[j]);
#endif  

  cout << "# of file marker entries: " << all_markers.size() << endl;
  for(j=0; j<all_markers.size(); j++)
    gui->pviewer->setParam("marker", all_markers[j]);

  cout << "# of file op entries: " << all_opvertices.size() << endl;
  for(j=0; j<all_opvertices.size(); j++)
    gui->pviewer->setParam("op_vertex", all_opvertices[j]);

  cout << "# of file geodesy entries: " << all_geodesy.size() << endl;
  for(j=0; j<all_geodesy.size(); j++)
    gui->pviewer->setParam("geodesy_init", all_geodesy[j]);

  gui->updateXY();

  return Fl::run();
}


