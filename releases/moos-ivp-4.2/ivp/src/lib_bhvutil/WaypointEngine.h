/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: WaypointEngine.h                                     */
/*    DATE: May 6th, 2007                                        */
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
 
#ifndef WAYPOINT_ENGINE_HEADER
#define WAYPOINT_ENGINE_HEADER

#include "XYSegList.h"

class WaypointEngine {
 public:
  WaypointEngine();
  ~WaypointEngine() {};

  void   setSegList(const XYSegList& seglist);
  void   setReverse(bool);
  void   setRepeat(unsigned int);
  void   setCaptureRadius(double);
  void   setNonmonotonicRadius(double);
  void   setCurrIndex(unsigned int);
  void   setPerpetual(bool v) {m_perpetual = v;};
  void   setCenter(double, double);
  void   resetForNewTraversal();
  void   setRepeatsEndless(bool v) {m_repeats_endless=v;};

  double getPointX(unsigned int) const;
  double getPointY(unsigned int) const;  
  double getPointX() const      {return(m_seglist.get_vx(m_curr_ix));};
  double getPointY() const      {return(m_seglist.get_vy(m_curr_ix));};
  bool   isComplete() const     {return(m_complete);};
  int    getCurrIndex() const   {return(m_curr_ix);};
  bool   currPtChanged();
  void   resetCPA()       {m_current_cpa = -1;};

  unsigned int size() const     {return(m_seglist_raw.size());};
  unsigned int getCycleCount()  {return(m_cycle_count);};
  unsigned int getTotalHits()   {return(m_capture_hits + m_nonmono_hits);};
  unsigned int getCaptureHits() {return(m_capture_hits);};
  unsigned int getNonmonoHits() {return(m_nonmono_hits);};
  unsigned int resetsRemaining() const;

  XYSegList getSegList()  {return(m_seglist);};

  std::string setNextWaypoint(double os_x, double os_y);

 protected:
  XYSegList m_seglist;
  XYSegList m_seglist_raw;

  bool      m_reverse;
  bool      m_perpetual;
  double    m_capture_radius;
  double    m_nm_radius;

  double    m_current_cpa;
  bool      m_complete;
  int       m_prev_ix;
  int       m_curr_ix;
  unsigned int  m_cycle_count;

  bool          m_repeats_endless;
  unsigned int  m_repeats_allowed;
  unsigned int  m_repeats_sofar;


  unsigned int  m_capture_hits;
  unsigned int  m_nonmono_hits;
};

#endif









