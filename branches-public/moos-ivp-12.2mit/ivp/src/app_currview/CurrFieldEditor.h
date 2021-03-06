/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: CurrentField.h                                       */
/*    DATE: Jan 4th 2011                                         */
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

#ifndef CURR_FIELD_EDITOR_HEADER
#define CURR_FIELD_EDITOR_HEADER

#include <string>
#include <vector>
#include "XYVector.h"
#include "MOOSGeodesy.h"

class CurrentField
{
public:
  CurrFieldEditor();
  ~CurrFieldEditor() {};

  bool populate(std::string filename);
  void addVector(const XYVector&, bool marked=false);
  void getLocalForce(double x, double y, double& fx, double& fy) const;
  void setRadius(double radius);
  bool initGeodesy(double datum_lat, double datum_lon);
  void print();

  void deleteVector(unsigned int ix);
  void markupVector(unsigned int ix);
  void unmarkVector(unsigned int ix);
  void markVector(unsigned int ix);


  bool isMarked(unsigned int ix);
  void deleteMarkedVectors();

  void modVector(unsigned int ix, std::string param, double val);
  void modMarkedVectors(std::string param, double val);
  bool unmarkAllVectors();
  bool markAllVectors();
  void applySnap(double);

  XYVector getVector(unsigned int) const;
  bool     getVMarked(unsigned int) const;
  double   getXPos(unsigned int) const;
  double   getYPos(unsigned int) const;
  double   getForce(unsigned int) const;
  double   getDirection(unsigned int) const;
  bool     hasActiveVertex() const;

  unsigned int selectVector(double x, double y, double& dist);

  unsigned int getActiveIX() {return(m_active_ix);};
  unsigned int size()        {return(m_vectors.size());};
  std::string getName()      {return(m_field_name);};
  double getRadius()         {return(m_radius);};

  std::vector<XYVector> getVectors();
  std::vector<XYVector> getVectorsMarked();
  std::vector<XYVector> getVectorsUnMarked();

  std::vector<std::string> getListing();

 protected:
  bool   handleLine(std::string);

protected:
  std::vector<XYVector> m_vectors;
  std::vector<bool>     m_vmarked;
  std::string           m_field_name;
  double                m_radius;
  bool                  m_active_vertex;
  unsigned int          m_active_ix;
  CMOOSGeodesy          m_geodesy;
};

#endif 

