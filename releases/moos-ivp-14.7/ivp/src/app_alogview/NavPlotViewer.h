/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: NavPlotViewer.h                                      */
/*    DATE: May 31st, 2005                                       */
/*                                                               */
/* This file is part of MOOS-IvP                                 */
/*                                                               */
/* MOOS-IvP is free software: you can redistribute it and/or     */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation, either version  */
/* 3 of the License, or (at your option) any later version.      */
/*                                                               */
/* MOOS-IvP is distributed in the hope that it will be useful,   */
/* but WITHOUT ANY WARRANTY; without even the implied warranty   */
/* of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See  */
/* the GNU General Public License for more details.              */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with MOOS-IvP.  If not, see                     */
/* <http://www.gnu.org/licenses/>.                               */
/*****************************************************************/

#ifndef NAVPLOT_VIEWER_HEADER
#define NAVPLOT_VIEWER_HEADER

#include <vector>
#include "FL/Fl.H"
#include "FL/Fl_Gl_Window.H"
#include "FL/gl.h"
#include "FL/fl_draw.H"
#include "BackImg.h"
#include "LogPlot.h"
#include "HelmPlot.h"
#include "VPlugPlot.h"
#include "MarineViewer.h"

class NavPlotViewer : public MarineViewer
{
 public:
  NavPlotViewer(int x,int y,int w,int h,const char *l=0);
  virtual ~NavPlotViewer() {};
  
  void   draw();
  bool   setParam(std::string p, std::string v);
  bool   setParam(std::string p, double v);  
  void   addLogPlotNAVX(const LogPlot& lp);
  void   addLogPlotNAVY(const LogPlot& lp); 
  void   addLogPlotHDG(const LogPlot& lp); 
  void   addLogPlotStartTime(double); 
  void   addVPlugPlot(const VPlugPlot& vp); 
  unsigned int addHelmPlot(const HelmPlot& hp); 

  void   setCenterView(std::string centering="ctr_of_bounding");
  void   setStepType(const std::string&);
  void   stepTime(double);
  void   setCurrTime(double);
  void   setHPlotLeftIndex(unsigned int);
  void   setHPlotRightIndex(unsigned int);
  void   setIndex(unsigned int, unsigned int);
  double getCurrTime();
  double getStartTimeHint();

  std::string getHPlotVName(const std::string&);
  std::string getHPlotMode(const std::string&);
  std::string getHPlotDecision(const std::string&);
  std::string getHPlotBehaviors(const std::string&, const std::string&);

  std::map<std::string, unsigned int> getVIterMap();

 public: // Configuration parameters
  void   setFrame(std::string s)   {m_frame = s;};

protected:
  void  drawNavPlots();
  void  drawNavPlot(unsigned int ix);
  void  drawTrails();
  void  drawTrail(unsigned int ix);
  void  drawVPlugPlots();
  void  drawVPlugPlot(unsigned int ix);
  void  drawFrame();

  std::string shortenBehaviors(std::string);

private:
  unsigned int  m_hplot_left_ix;
  unsigned int  m_hplot_right_ix;
  double        m_curr_time;
  std::string   m_frame;

  // Bounding box of all vehicle positions and timestamps
  double        m_min_time;
  double        m_max_time;
  double        m_min_xpos;
  double        m_min_ypos;
  double        m_max_xpos;
  double        m_max_ypos;

  // vectors - each index corresponds to one vehicle
  std::vector<LogPlot>   m_navx_plot;
  std::vector<LogPlot>   m_navy_plot;
  std::vector<LogPlot>   m_hdg_plot;
  std::vector<HelmPlot>  m_helm_plot;
  std::vector<VPlugPlot> m_vplug_plot;
  std::vector<double>    m_start_time;

  std::string m_trails;
  int         m_trail_gap;
  bool        m_behaviors_verbose;
  bool        m_step_by_secs;
};

#endif 



