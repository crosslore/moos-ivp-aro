/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: NodeReporter.cpp                                     */
/*    DATE: Feb 13th 2006 (TransponderAIS)                       */
/*    DATE: Jun  8th 2009 (NodeReporter)                         */
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

#include <list>
#include <iterator>
#include <algorithm>
#include "NodeReporter.h"
#include "MBUtils.h"
#include "AngleUtils.h"
#include "NodeRecord.h"

#define USE_UTM

using namespace std;

//-----------------------------------------------------------------
// Procedure: Constructor

NodeReporter::NodeReporter()
{
  m_vessel_name  = "unknown";

  m_record.setType("unknown");
  m_record.setLength(0);

  m_helm_engaged = false;
  m_helm_lastmsg = -1;
  m_nohelm_thresh = 5;
  m_helm_mode    = "none";
  m_helm_allstop_mode = "unknown";

  m_blackout_interval = 0;
  m_blackout_baseval  = 0;
  m_blackout_variance = 0;
  m_last_post_time    = -1;
  m_time_updated      = false;

  // Below is a good alt_nav prefix suggestion,  but left blank to keep
  // feature off unless another app is generating the alt nav solution.
  // m_alt_nav_prefix    = "NAV_GT";
  m_alt_nav_name      = "_GT";

  m_crossfill_policy = "literal";

  // Timestamps used for executing the "use-latest" crossfill policy 
  m_nav_xy_updated        = 0;
  m_nav_latlon_updated    = 0;
  m_nav_xy_updated_gt     = 0;
  m_nav_latlon_updated_gt = 0;

  m_record_gt_updated     = 0;

  m_node_report_var = "NODE_REPORT_LOCAL";
  m_plat_report_var = "PLATFORM_REPORT_LOCAL";
}

//-----------------------------------------------------------------
// Procedure: OnNewMail

bool NodeReporter::OnNewMail(MOOSMSG_LIST &NewMail)
{
  m_utc_time = MOOSTime();
  m_time_updated = true;

  MOOSMSG_LIST::iterator p;
  
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;

    string key   = msg.m_sKey;
    string sdata = msg.m_sVal;
    double ddata = msg.m_dfVal;

    if(key == "NAV_X") {
      m_record.setX(ddata);
      m_nav_xy_updated = m_utc_time;
    }
    else if(key == "NAV_Y") {
      m_record.setY(ddata);
      m_nav_xy_updated = m_utc_time;
    }
    else if(key == "NAV_LAT") {
      m_record.setLat(ddata);
      m_nav_latlon_updated = m_utc_time;
    }
    else if(key == "NAV_LONG") {
      m_record.setLon(ddata);
      m_nav_latlon_updated = m_utc_time;
    }
    else if(key == "NAV_SPEED")
      m_record.setSpeed(ddata);
    else if(key == "NAV_HEADING")
      m_record.setHeading(angle360(ddata));
    else if(key == "NAV_DEPTH")
      m_record.setDepth(ddata);
    else if(key == "NAV_YAW") 
      m_record.setYaw(ddata);
    
    bool record_gt_updated = true;
    if(key == (m_alt_nav_prefix + "X")) {
      m_record_gt.setX(ddata);
      m_nav_xy_updated_gt = m_utc_time;
    }
    else if(key == (m_alt_nav_prefix + "Y")) {
      m_record_gt.setY(ddata);
      m_nav_xy_updated_gt = m_utc_time;
    }
    else if(key == (m_alt_nav_prefix + "LAT")) {
      m_record_gt.setLat(ddata);
      m_nav_latlon_updated_gt = m_utc_time;
    }
    else if(key == (m_alt_nav_prefix + "LONG")) {
      m_record_gt.setLon(ddata);
      m_nav_latlon_updated_gt = m_utc_time;
    }
    else if(key == (m_alt_nav_prefix + "SPEED"))
      m_record_gt.setSpeed(ddata);
    else if(key == (m_alt_nav_prefix + "HEADING"))
      m_record_gt.setHeading(angle360(ddata));
    else if(key == (m_alt_nav_prefix + "DEPTH"))
      m_record_gt.setDepth(ddata);
    else if(key == (m_alt_nav_prefix + "YAW"))
      m_record_gt.setYaw(ddata);
    else 
      record_gt_updated = false;
    
    if(record_gt_updated)
      m_record_gt_updated = m_utc_time;

    else if(key == "IVPHELM_SUMMARY") {
      m_helm_lastmsg = m_utc_time;
      handleLocalHelmSummary(sdata);
    }
    else if(key == "IVPHELM_ALLSTOP") {
      m_helm_lastmsg = m_utc_time;
      m_helm_allstop_mode = sdata;
    }
    else if(key == "IVPHELM_ENGAGED") {
      m_helm_lastmsg = m_utc_time;
      string value = tolower(stripBlankEnds(sdata));
      m_helm_engaged = (value == "engaged");
    }

    if(vectorContains(m_plat_vars, key)) {
      string value = sdata;
      if(msg.IsDouble())
	value = dstringCompact(doubleToString(ddata, 8));
      updatePlatformVar(key, value);
    }
  }
  return(true);
}

//-----------------------------------------------------------------
// Procedure: OnConnectoToServer()

bool NodeReporter::OnConnectToServer()
{
  registerVariables();
  return(true);
}

//-----------------------------------------------------------------
// Procedure: registerVariables()

void NodeReporter::registerVariables()
{
  m_Comms.Register("NAV_X", 0);
  m_Comms.Register("NAV_Y", 0);
  m_Comms.Register("NAV_LAT", 0);
  m_Comms.Register("NAV_LONG", 0);
  m_Comms.Register("NAV_SPEED", 0);
  m_Comms.Register("NAV_HEADING", 0);
  m_Comms.Register("NAV_YAW", 0);
  m_Comms.Register("NAV_DEPTH", 0);

  if(m_alt_nav_prefix != "") {
    if(!strEnds(m_alt_nav_prefix, "_"))
      m_alt_nav_prefix += "_";
    m_Comms.Register(m_alt_nav_prefix + "X", 0);
    m_Comms.Register(m_alt_nav_prefix + "Y", 0);
    m_Comms.Register(m_alt_nav_prefix + "LAT", 0);
    m_Comms.Register(m_alt_nav_prefix + "LONG", 0);
    m_Comms.Register(m_alt_nav_prefix + "SPEED", 0);
    m_Comms.Register(m_alt_nav_prefix + "HEADING", 0);
    m_Comms.Register(m_alt_nav_prefix + "YAW", 0);
    m_Comms.Register(m_alt_nav_prefix + "DEPTH", 0);
  }  

  m_Comms.Register("IVPHELM_SUMMARY", 0);
  m_Comms.Register("IVPHELM_ENGAGED", 0);
  m_Comms.Register("IVPHELM_ALLSTOP", 0);
}

//-----------------------------------------------------------------
// Procedure: Iterate()
// Note: The "blackout_interval" is a way to have a vehicle simulate 
//       a vehicle that is only making period reports available to 
//       other vehicles. Even if this effect in the real world is due
//       more to transmission loss or latency.
//    

bool NodeReporter::Iterate()
{
  // If the time was updated while handling mail, don't update it again
  if(!m_time_updated)
    m_utc_time = MOOSTime();
  m_time_updated = false;

  // If (a) this is the first chance to post, or (b) there is no
  // blackout interval being implemented, or (c) the time since last
  // post exceeds the blackout interval, then perform a posting.
  if((m_last_post_time == -1) || (m_blackout_interval <= 0) ||
     ((m_utc_time - m_last_post_time) > m_blackout_interval)) {

    if(m_crossfill_policy != "literal")
      crossFillCoords(m_record, m_nav_xy_updated, m_nav_latlon_updated);
      
    string report = assembleNodeReport(m_record);
    m_Comms.Notify(m_node_report_var, report);
    
    double elapsed_time = m_utc_time - m_record_gt_updated;
    if(elapsed_time < 5) {
      if(m_crossfill_policy != "literal")
	crossFillCoords(m_record_gt, m_nav_xy_updated_gt, m_nav_latlon_updated_gt);
      
      string report_gt = assembleNodeReport(m_record_gt);
      m_Comms.Notify(m_node_report_var, report_gt);
    }

    // Note the post time and apply it to the blackout calculation
    m_last_post_time = m_utc_time;
    m_blackout_interval  = m_blackout_baseval;

    if(m_blackout_variance > 0) {
      int    range = (int)(m_blackout_variance * 100.0);
      int    val   = rand() % range;
      double delta = (double)((double)val / 100.0);
      m_blackout_interval -= m_blackout_variance;
      m_blackout_interval += (delta * 2.0);
    }

    if(m_blackout_interval < 0)
      m_blackout_interval = 0;
  }

  string platform_report = assemblePlatformReport();
  if(platform_report != "")
    m_Comms.Notify(m_plat_report_var, platform_report);

  return(true);
}


//-----------------------------------------------------------------
// Procedure: OnStartUp()
//      Note: Happens before connection is open

bool NodeReporter::OnStartUp()
{
  CMOOSApp::OnStartUp();
  MOOSTrace("pNodeReporter starting....\n");

  // Ownship_name == Community must be provided
  if(!m_MissionReader.GetValue("Community", m_vessel_name))
    return(MOOSFail("Vehicle (Community) Name not provided\n"));

   // look for latitude, longitude global variables
  double lat_origin, lon_origin;
  bool ok1 = m_MissionReader.GetValue("LatOrigin", lat_origin);
  bool ok2 = m_MissionReader.GetValue("LongOrigin", lon_origin);
  if(!ok1 || !ok2)
    return(MOOSFail("Lat or Lon Origin not set in *.moos file.\n"));

  // initialize m_geodesy
  if(!m_geodesy.Initialise(lat_origin, lon_origin))
    return(MOOSFail("Geodesy init failed.\n"));

  list<string> sParams;
  m_MissionReader.EnableVerbatimQuoting(false);
  if(m_MissionReader.GetConfiguration(GetAppName(), sParams)) {
    list<string>::iterator p;
    for(p=sParams.begin(); p!=sParams.end(); p++) {
      string param = stripBlankEnds(toupper(biteString(*p, '=')));
      string value = stripBlankEnds(*p);
      double dval  = atof(value.c_str());

      if((param == "PLATFORM_TYPE") ||           // Preferred
	 (param == "VESSEL_TYPE"))               // Deprecated
	m_record.setType(value);
      else if((param =="PLATFORM_LENGTH") ||     // Preferred
	      (param =="VESSEL_LENGTH")) {       // Deprecated
	if(isNumber(value) && (dval >= 0))
	  m_record.setLength(dval);
      }
      else if(param == "BLACKOUT_INTERVAL") {
	if(isNumber(value) && (dval >= 0)) {
	  m_blackout_baseval  = dval;
	  m_blackout_interval = dval;
	}
      }
      else if(param == "BLACKOUT_VARIANCE") {
	if(isNumber(value) && (dval >= 0))
	  m_blackout_variance = dval;
      }
      else if((param == "PLAT_REPORT_INPUT") ||     // Preferred
	      (param == "PLAT_REPORT_VAR") ||       // Deprecated
	      (param == "PLATFORM_VARIABLE"))       // Deprecated
	addPlatformVar(value);
      else if((param == "NODE_REPORT_OUTPUT") ||    // Preferred
	      (param == "NODE_REPORT_SUMMARY") ||   // Deprecated
	      (param == "NODE_REPORT_VAR")) {       // Deprecated
	if(!strContainsWhite(value))
	  m_node_report_var = value;
      }      
      else if(param == "PLAT_REPORT_OUTPUT") {
	if(!strContainsWhite(value))
	  m_plat_report_var = value;
      }      
      else if(param == "NOHELM_THRESHOLD") {
	if(isNumber(value) && (dval > 0))
	  m_nohelm_thresh = dval;
      }      
      else if(param =="CROSSFILL_POLICY")
	setCrossFillPolicy(value);
      else if(param =="ALT_NAV_PREFIX")
	m_alt_nav_prefix = value;
      else if(param =="ALT_NAV_NAME")
	m_alt_nav_name = value;
    }
  }

  registerVariables();
  unsigned int k, ksize = m_plat_vars.size();
  for(k=0; k<ksize; k++)
    m_Comms.Register(m_plat_vars[k], 0);

  // If the length is unknown, put in some good guesses
  if(m_record.getLength() == 0) {
    if(tolower(m_record.getType()) == "kayak")
      m_record.setLength(4); // meters;
    if(tolower(m_record.getType()) == "auv")
      m_record.setLength(4); // meters;
    if(tolower(m_record.getType()) == "uuv")
      m_record.setLength(4); // meters;
    if(tolower(m_record.getType()) == "ship")
      m_record.setLength(8); // meters
    if(tolower(m_record.getType()) == "glider")
      m_record.setLength(3); // meters
  }
  
  m_record.setName(m_vessel_name);
  // To start with m_record_gt is just a copy of m_record.
  m_record_gt = m_record;       

  if(strBegins(m_alt_nav_name, "_"))
    m_record_gt.setName(m_vessel_name + m_alt_nav_name);
  else
    m_record_gt.setName(m_alt_nav_name);
  
  return(true);
}

//-----------------------------------------------------------------
// Procedure: handleLocalHelmSummary()
//     Notes: Handles the incoming summary variable posted by the 
//            helm. This routine grabs the "modes" field of that
//            summary for inclusion in the NODE_REPORT summary.

void NodeReporter::handleLocalHelmSummary(const string& sdata)
{
  vector<string> svector = parseString(sdata, ',');
  unsigned int i, vsize = svector.size();
  for(i=0; i<vsize; i++) {
    string left  = stripBlankEnds(biteString(svector[i], '='));
    string right = stripBlankEnds(svector[i]);
    if(left == "modes")
      m_helm_mode = right;
  }
}

//------------------------------------------------------------------
// Procedure: addPlatformVar
//   Purpose: Add components to the Platform Report

bool NodeReporter::addPlatformVar(string entry)
{
  string varname;
  string alias;
  double postgap=0;

  vector<string> svector = parseString(entry, ',');
  unsigned int i, isize = svector.size();
  for(i=0; i<isize; i++) {
    string left  = stripBlankEnds(biteString(svector[i],'='));
    string right = stripBlankEnds(svector[i]);
    if(right == "")
      varname = left;
    else {
      if(tolower(left) == "gap") {
	postgap = atof(right.c_str());
	if(postgap < 0)
	  postgap = 0;
      }
      else if(tolower(left == "alias")) {
	if(!strContainsWhite(right))
	  alias = right;
      }
    }
  }

  if(strContainsWhite(varname))
    return(false);
  if(vectorContains(m_plat_vars, varname))
    return(false);

  m_plat_vars.push_back(varname);
  m_plat_vals.push_back("");
  m_plat_alias.push_back(alias);
  m_plat_recv_tstamp.push_back(0);
  m_plat_post_tstamp.push_back(0);
  m_plat_post_gap.push_back(postgap);

  return(true);

}

//------------------------------------------------------------------
// Procedure: updatePlatformVar()
//   Purpose: 

void NodeReporter::updatePlatformVar(string variable, string value)
{
  unsigned int i, isize = m_plat_vars.size();
  for(i=0; i<isize; i++) {
    if(m_plat_vars[i] == variable) {
      m_plat_vals[i] = value;
      m_plat_recv_tstamp[i]  = m_utc_time;
    }
  }
}


//------------------------------------------------------------------
// Procedure: assemblePlatformReport
//   Purpose: 

string NodeReporter::assemblePlatformReport()
{
  string return_str;
  unsigned int i, isize = m_plat_vars.size();
  for(i=0; i<isize; i++) {
    if(m_plat_recv_tstamp[i] != 0) {
      double time_since_last_post = m_utc_time - m_plat_post_tstamp[i];
      bool   unposted_change = false;
      if((m_plat_recv_tstamp[i] - m_plat_post_tstamp[i]) > 0)
	unposted_change = true;
      if(unposted_change && (time_since_last_post > m_plat_post_gap[i])) {
	if(return_str != "")
	  return_str += ",";
	if(m_plat_alias[i] == "")
	  return_str += (m_plat_vars[i] + "=" + m_plat_vals[i]);
	else
	  return_str += (m_plat_alias[i] + "=" + m_plat_vals[i]);
	m_plat_post_tstamp[i] = m_utc_time;
      }
    }
  }
  if(return_str != "") {
    string augment_str = ("platform=" + m_vessel_name + ",");
    augment_str += "utc_time=" + doubleToString(m_utc_time) + ",";
    return_str = augment_str + return_str;
  }
  return(return_str);
}


//------------------------------------------------------------------
// Procedure: assembleNodeReport
//   Purpose: Assemble the node report from member variables.

string NodeReporter::assembleNodeReport(NodeRecord record)
{
  record.setTimeStamp(m_utc_time); 

  string  mode;
  if((m_utc_time-m_helm_lastmsg) > m_nohelm_thresh) {
    string awol_time = "EVER";
    if(m_helm_lastmsg > 0)
      awol_time = doubleToString((m_utc_time-m_helm_lastmsg), 0);
    mode = "NOHELM-" + awol_time;
  }
  else if(m_helm_engaged == false)
    mode = "DISENGAGED";
  else {
    if(m_helm_mode != "none")
      mode = m_helm_mode;
    else
      mode = "ENGAGED";
  }
  
  record.setMode(mode);
  record.setAllStop(m_helm_allstop_mode);

  string summary = record.getSpec();
  return(summary);
}

//------------------------------------------------------------------
// Procedure: setCrossFillPolicy
//      Note: Determines how or whether the local and global coords
//            are updated from one another.

void NodeReporter::setCrossFillPolicy(string policy)
{
  policy = tolower(policy);
  policy = findReplace(policy, '_', '-');
  if((policy=="literal")||(policy=="fill-empty")||(policy=="use-latest"))
    m_crossfill_policy = policy;
}


//------------------------------------------------------------------
// Procedure: crossFillCoords
//   Purpose: Potentially update NAV_X/Y from NAV_LAT/LON or v.versa.

void NodeReporter::crossFillCoords(NodeRecord& record, 
				   double nav_xy_updated,
				   double nav_latlon_updated)
{
  // The "fill-empty" policy will fill the other coordinates only if 
  // the other coordinates have NEVER been written to.
  if(m_crossfill_policy == "fill-empty") {
    if(!record.valid("x") && !record.valid("y") &&
       record.valid("lat") && record.valid("lon"))
      crossFillGlobalToLocal(record);
    if(record.valid("x") && record.valid("y") &&
       !record.valid("lat") && !record.valid("lon")) {
      crossFillLocalToGlobal(record);
    }
  }

  // The "use-latest" policy will fill the other coordinates only if 
  // the other coordinates are more stale than their counterpart
  if(m_crossfill_policy == "use-latest") {
    if(nav_xy_updated == nav_latlon_updated)
      return;

    if(nav_xy_updated < nav_latlon_updated) // Local coords older
      crossFillGlobalToLocal(record);
    else                                    // Global coords older
      crossFillLocalToGlobal(record);
  }
}


//------------------------------------------------------------------
// Procedure: crossFillLocalToGlobal
//   Purpose: Update the Global coordinates based on the Local coords.

void NodeReporter::crossFillLocalToGlobal(NodeRecord& record)
{
  double nav_x = record.getX();
  double nav_y = record.getY();

  double nav_lat, nav_lon;

#ifdef USE_UTM
  m_geodesy.LocalGrid2LatLong(nav_x, nav_y, nav_lat, nav_lon);
#else
  m_geodesy.UTM2LatLong(nav_x, nav_y, nav_lat, nav_lon);
#endif      

  record.setLat(nav_lat);
  record.setLon(nav_lon);  
}

//------------------------------------------------------------------
// Procedure: crossFillGlobalToLocal
//   Purpose: Update the Local coordinates based on the Global coords.

void NodeReporter::crossFillGlobalToLocal(NodeRecord& record)
{
  double nav_lat = record.getLat();
  double nav_lon = record.getLon();

  double nav_x, nav_y;

#ifdef USE_UTM
  m_geodesy.LatLong2LocalUTM(nav_lat, nav_lon, nav_y, nav_x);
#else
  m_geodesy.LatLong2LocalGrid(nav_lat, nav_lon, nav_y, nav_x);
#endif      

  record.setX(nav_x);
  record.setY(nav_y);
}
