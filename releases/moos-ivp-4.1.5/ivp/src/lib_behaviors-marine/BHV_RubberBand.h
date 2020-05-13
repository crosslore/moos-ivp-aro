/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: BHV_RubberBand.h                                    */
/*    DATE: Aug 25 2006                                          */
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
 
#ifndef BHV_RUBBER_BAND_HEADER
#define BHV_RUBBER_BAND_HEADER

#include <string>
#include <vector>
#include "IvPBehavior.h"
#include "XYSegList.h"

class BHV_RubberBand : public IvPBehavior {
public:
  BHV_RubberBand(IvPDomain);
  ~BHV_RubberBand() {};
  
  IvPFunction* onRunState();
  bool         setParam(std::string, std::string);
  void         onIdleState();
  void         onSetParamComplete();
  void         onRunToIdleState();


protected:
  bool         updateInfoIn();  
  bool         updateCenter();
  void         postStationMessage(bool);

  void    postViewableTrailPoint();
  void    postErasableTrailPoint();
  XYPoint m_trail_point;

private:  // Configuration Parameters
  double      m_station_x;
  double      m_station_y;
  bool        m_station_set;

  double      m_outer_radius;
  double      m_inner_radius;
  bool        m_center_activate;

  // The outer speed is the speed at the outer radius. The 
  // desired_speed will decrease linearly until the inner
  // ring, and will be zero inside the inner ring.
  double      m_outer_speed;
  
  // The extra_speed is the desired speed should the
  // vehicle drift outside the outer radius.
  double      m_extra_speed;


  // distance stiffness is the rate of increase in the priority with range
  double stiffness ;
  double relevance;

private:  // State Variables
  bool         m_center_pending;
  std::string  m_center_assign;
  double       m_osx;
  double       m_osy;
};

#endif










