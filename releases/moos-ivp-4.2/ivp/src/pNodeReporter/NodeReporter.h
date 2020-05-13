/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: NodeReporter.h                                       */
/*    DATE: Feb 9th 2006 (TransponderAIS)                        */
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

#ifndef NODE_REPORTER_HEADER
#define NODE_REPORTER_HEADER

#include <string>
#include <vector>
#include "MOOSLib.h"
#include "MOOSGeodesy.h"
#include "NodeRecord.h"

class NodeReporter : public CMOOSApp
{
public:
  NodeReporter();
  virtual ~NodeReporter() {};
  
  bool OnNewMail(MOOSMSG_LIST &NewMail);
  bool Iterate();
  bool OnConnectToServer();
  bool OnStartUp();

 protected:
  void handleLocalHelmSummary(const std::string&);
  std::string assembleNodeReport(NodeRecord);
  std::string assemblePlatformReport();
  
  void updatePlatformVar(std::string, std::string);
  bool addPlatformVar(std::string);
  void registerVariables();
  void setCrossFillPolicy(std::string);
  void crossFillCoords(NodeRecord&, double, double);
  void crossFillLocalToGlobal(NodeRecord&);
  void crossFillGlobalToLocal(NodeRecord&);

 protected: // Configuration Variables (Node Reports)
  std::string  m_vessel_name;
  std::string  m_crossfill_policy;
  std::string  m_node_report_var;
  double       m_nohelm_thresh;

 protected: // State Variables (Node Reports)
  CMOOSGeodesy m_geodesy;
  std::string  m_helm_mode;
  std::string  m_helm_allstop_mode;
  std::string  m_alt_nav_prefix;
  std::string  m_alt_nav_name;
  bool         m_helm_engaged;
  double       m_helm_lastmsg;

  NodeRecord   m_record;
  NodeRecord   m_record_gt;
  double       m_record_gt_updated;

  double       m_nav_xy_updated;
  double       m_nav_latlon_updated;
  double       m_nav_xy_updated_gt;
  double       m_nav_latlon_updated_gt;

 protected: // State Variables (General)
  bool         m_time_updated;
  double       m_utc_time;
  
 protected: // Config and State Vars (Blackout Interval)
  double  m_blackout_interval;
  double  m_blackout_baseval;
  double  m_blackout_variance;
  double  m_last_post_time;

 protected: // Config and State Vars (Platform Reports)
  std::string              m_plat_report_var;
  std::vector<std::string> m_plat_vars;
  std::vector<std::string> m_plat_vals;
  std::vector<std::string> m_plat_alias;
  std::vector<double>      m_plat_post_gap;
  std::vector<double>      m_plat_post_tstamp;
  std::vector<double>      m_plat_recv_tstamp;
};

#endif
