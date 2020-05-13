/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: CTDSensor_Model.cpp                               */
/*    DATE: Jan 28th, 2012                                       */
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

#include <cstdlib>
#include <iostream>
#include <cmath>
#include <ctime>
#include "MBUtils.h"
#include "GeomUtils.h"
#include "AngleUtils.h"
#include "CTDSensor_Model.h"
#include "NodeRecordUtils.h"

using namespace std;

//------------------------------------------------------------
// Constructor

CTDSensor_Model::CTDSensor_Model()
{
  // State Variables
  // -------------------------------------------------------------
  m_curr_time  = 0;
  m_start_time  = 0;
  m_time_warp  = 1;
  m_iterations = 0;
  m_reports    = 0;
  m_last_report_time  = 0;
  m_last_summary_time = 0;    // last time settings options summary posted

  // Configuration variables
  m_term_report_interval = 0.8;      // realtime (non-timewarp) seconds
  // Front model parameters
  //-------------------------------------------------------------
  m_xmin  = 0;       
  m_xmax  = 500;       
  m_ymin  = 0;       
  m_ymax  = 400;       
  m_offset = 200;
  m_angle = 20;
  m_amplitude = 50;
  m_period = 120;
  m_wavelength = 300;
  m_alpha = 100;  
  m_beta = 30;
  m_T_N = 20;
  m_T_S = 25;
  m_sigma = 0.2;
}

//------------------------------------------------------------
// Procedure: setParam

bool CTDSensor_Model::setParam(string param, string value)
{
  bool handled = false;

  if (param == "xmin") 
    {
      m_xmin = atof(value.c_str());
      handled = true;
    }
  else if (param == "xmax")
    {
      m_xmax = atof(value.c_str());
      handled = true;
    }
  else if (param == "ymin")
    {
      m_ymin = atof(value.c_str());
      handled = true;
    }
  else if (param == "ymax")
    {
      m_ymax = atof(value.c_str());
      handled = true;
    }
  else if (param == "offset")
    {
      m_offset = atof(value.c_str());
      handled = true;
      cout << "offset = " << m_offset << endl; 
    }
  else if (param == "angle")
    {
      m_angle = atof(value.c_str());
      handled = true;
      cout << "angle = " << m_angle << endl; 
    }
  else if (param == "amplitude")
    {
      m_amplitude = atof(value.c_str());
      handled = true;
      cout << "amplitude = " << m_amplitude << endl; 
    }
  else if (param == "period")
    {
      m_period = atof(value.c_str());
      handled = true;
      cout << "m_period = " << m_period << endl; 
    }
  else if (param == "wavelength")
    {
      m_wavelength = atof(value.c_str());
      handled = true;
      cout << "wavelength = " << m_wavelength << endl; 
    }
  else if (param == "alpha")
    {
      m_alpha = atof(value.c_str());
      handled = true;
      cout << "alpha = " << m_alpha << endl; 
    }
  else if (param == "beta")
    {
      m_beta = atof(value.c_str());
      handled = true;
      cout << "beta = " << m_beta << endl; 
    }
  else if (param == "temperature_north")
    {
      m_T_N = atof(value.c_str());
      handled = true;
      cout << "m_T_N = " << m_T_N << endl; 
    }
  else if (param == "temperature_south")
    {
      m_T_S = atof(value.c_str());
      handled = true;
      cout << "m_T_S = " << m_T_S << endl; 
    }
  else if (param == "sigma")
    {
      m_sigma = atof(value.c_str());
      handled = true;
    }


  if(!handled)
    memoErr("Unhandled config, param="+param+", value="+value);

  return(handled);
}


//------------------------------------------------------------
// Procedure: handleMsg

bool CTDSensor_Model::handleMsg(string key, double dval, string sval,
				    double mtime, bool isdouble, bool isstring,
				    string src_app, string src_community)
{
  bool handled = false;
  if((key == "NODE_REPORT") || (key == "NODE_REPORT_LOCAL"))
    handled = handleNodeReport(sval);

  if(key == "UCTD_SENSOR_REQUEST")
    handled = handleSensorRequest(sval);
  
  if(key == "UCTD_PARAMETER_ESTIMATE")
    {
      handled = handleSensingReport(sval);
      setCurrTime(MOOSTime());
    }
  if(key == "DEPLOY_ALL")
    {
      if (sval == "true")
	{
	  setStartTime(MOOSTime());
	  cout << "Deploy, Start Time " << m_start_time << endl;
	}
    }
  if(!handled) {
    string msg = "Uhandled msg: " + key;
    if(isstring) 
      msg += ", sval=" + sval;
    else if(isdouble)
      msg += ", dval=" + doubleToStringX(dval,6);
    else
      msg += ", unknown_type";
    msg += ",[" + src_app + "],[" + src_community + "]";
    memoErr(msg);
  }
  
  return(handled);
}

//------------------------------------------------------------
// Procedure: setCurrTIme

void CTDSensor_Model::setCurrTime(double new_time)
{
  if(new_time > m_curr_time)
    m_curr_time = new_time;
}

//------------------------------------------------------------
// Procedure: setStartTIme

void CTDSensor_Model::setStartTime(double new_time)
{
  if(new_time > m_start_time)
    m_start_time = new_time;
}

//------------------------------------------------------------
// Procedure: iterate()

void CTDSensor_Model::iterate()
{
  m_iterations++;

  // Part 1: Consider printing a report to the terminal
  double warp_elapsed_time = m_curr_time - m_last_report_time;
  double real_elapsed_time = warp_elapsed_time;
  if(m_time_warp > 0)
    real_elapsed_time = warp_elapsed_time / m_time_warp;
  if(real_elapsed_time > m_term_report_interval) {
    //    printReport();
    m_last_report_time = m_curr_time;
  }

}

//------------------------------------------------------------
// Procedure: getMessages

vector<VarDataPair> CTDSensor_Model::getMessages(bool clear)
{
  if(!clear)
    return(m_messages);
  
  vector<VarDataPair> rval = m_messages;
  m_messages.clear();
  return(rval);
}


//------------------------------------------------------------
// Procedure: FieldModelConfig  

bool CTDSensor_Model::FieldModelConfig()
{

  front.setVars(m_offset, m_angle, m_amplitude ,
		m_period, m_wavelength, m_alpha, 
		m_beta, m_T_N, m_T_S);
  front.setRegion(m_xmin, m_xmax,
		  m_ymin, m_ymax); 
  
  return(true);
}
    
//------------------------------------------------------------
// Procedure: SensorConfig  

bool CTDSensor_Model::SensorConfig()
{

  front.setSigma(m_sigma);
  
  return(true);
}
    
//---------------------------------------------------------
// Procedure: handleNodeReport
//   Example: NAME=alpha,TYPE=KAYAK,UTC_TIME=1267294386.51,
//            X=29.66,Y=-23.49,LAT=43.825089, LON=-70.330030, 
//            SPD=2.00, HDG=119.06,YAW=119.05677,DEPTH=0.00,     
//            LENGTH=4.0,MODE=ENGAGED

bool CTDSensor_Model::handleNodeReport(const string& node_report_str)
{
  NodeRecord node_record = string2NodeRecord(node_report_str);

  if(!node_record.valid()) {
    memoErr("Unhandled node record");
    return(false);
  }
  
  string vname = node_record.getName();
  
  m_map_node_records[vname]     = node_record;
  m_map_last_node_update[vname] = m_curr_time;

  memo("Node report received");

  return(true);
}

//---------------------------------------------------------
// Procedure: handleSensorRequest
//   Example: vname=alpha

bool CTDSensor_Model::handleSensorRequest(const string& request)
{
  // Part 1: Parse the string request

  string vname;
  vector<string> svector = parseString(request, ',');
  unsigned int i, vsize = svector.size();
  for(i=0; i<vsize; i++) {
    string param = tolower(biteStringX(svector[i], '='));
    string value = svector[i];
    if(param == "vname")
      vname = value;
  }
  
  if(vname == "") {
    memoErr("Sensor request received with null vehicle name");
    return(false);
  }
  memo("Sensor request received from " + vname);

  // Part 2: Find out the latest node report info for the given vname;
  if(m_map_node_records.count(vname) == 0) {
    memoErr("Cannot handle sensor request: no node reports received from " +vname);
    return(false);
  }
  
  double vx = m_map_node_records[vname].getX();
  double vy = m_map_node_records[vname].getY();

  // Part 3: If this vehicle has not initialized its sensor setting, then 
  //         perform a default setting.
 
      // Part 5: For each source point, determine if the point is within range to
  //         the requesting vehicle.
  postSensorReport(vx, vy, vname);  // rolling dice inside

  return(true);
}


//---------------------------------------------------------
// Procedure: handleSensingReport
//   Example: vname=alpha,amplitude=50,offset=-100,period=100,wavelength=190,...
//   Compares field parameters estimated by vname to ground truth


bool CTDSensor_Model::handleSensingReport(const string& request)
{
  // Part 1: Parse the string report

  string vname;
  vector<string> svector = parseString(request, ',');
  unsigned int i, vsize = svector.size();

  for(i=0; i<vsize; i++) {
    string param = tolower(biteStringX(svector[i], '='));
    string value = svector[i];
    if(param == "vname")
      vname = value;
    else if (param == "offset")
      r_offset = atof(value.c_str());
    else if (param == "angle")
      r_angle = atof(value.c_str());
    else if (param == "amplitude")
      r_amplitude = atof(value.c_str());
    else if (param == "period")
      r_period = atof(value.c_str());
    else if (param == "wavelength")
      r_wavelength = atof(value.c_str());
    else if (param == "alpha")
      r_alpha = atof(value.c_str());
    else if (param == "beta")
      r_beta = atof(value.c_str());
    else if (param == "tempnorth")
      r_T_N = atof(value.c_str());
    else if (param == "tempsouth")
      r_T_S = atof(value.c_str());
  }

  if(vname == "") {
    memoErr("Sensor mission report received with null vehicle name");
    return(false);
  }

  // compute estimation error
  double ref_amp = m_amplitude;
  if (ref_amp < 1) ref_amp =  1;
  double ref_off = fabs(m_offset);
  if (ref_off < 1) ref_off =  1;
  double ref_ang = fabs(m_angle);
  if (ref_ang < 1) ref_ang =  1;
  double ref_per = m_period;
  if (ref_per < 1) ref_per =  1;
  double ref_wav = m_wavelength;
  if (ref_wav < 1) ref_wav =  1;
  double ref_alp = fabs(m_alpha);
  if (ref_alp < 1) ref_alp =  1;
  double ref_bet = m_beta;
  if (ref_bet < 1) ref_bet =  1;
  double ref_tn = m_T_N;
  if (ref_tn < 1) ref_tn =  1;
  double ref_ts = m_T_S;
  if (ref_ts < 1) ref_ts =  1;

  double error = pow((m_amplitude-r_amplitude)/ref_amp,2)
    + pow((m_offset-r_offset)/ref_off,2)
    + pow((m_angle-r_angle)/ref_ang,2)
    + pow((m_period-r_period)/ref_per,2)
    + pow((m_wavelength-r_wavelength)/ref_wav,2)
    + pow((m_alpha-r_alpha)/ref_alp,2)
    + pow((m_beta-r_beta)/ref_bet,2)
    + pow((m_T_N-r_T_N)/ref_tn,2)
    + pow((m_T_S-r_T_S)/ref_ts,2);
  error /= 9;
  error = sqrt(error);
  double score = 1/error;

  memo("Sensor mission report received from " + vname);
  memo("Error " + doubleToString(error));

  double time_threshold = 500;
  double elapsed = m_curr_time - m_start_time;
  double time_fac = 1;
  if (elapsed > time_threshold)
    time_fac = elapsed/time_threshold;
  score = 1e2*score/time_fac;
  memo("Score " + doubleToString(score));
  cout << "=================== " << endl;
  cout << "Report from " << vname << endl ;
  cout << "=================== " << endl;
  cout << "Error        " << error << endl ;
  cout << "Elapsed time " << m_curr_time-m_start_time << endl ;
  cout << "Score        " << score << endl ;

  cout << "Parameter estimates " << endl;
  cout << "=================== " << endl;
  cout << "Offset     " << r_offset << " Actual " << m_offset << endl; 
  cout << "Angle      " << r_angle << " Actual " << m_angle << endl; 
  cout << "Amplitude  " << r_amplitude << " Actual " << m_amplitude << endl; 
  cout << "Period     " << r_period << " Actual " << m_period << endl; 
  cout << "Wavelength " << r_wavelength << " Actual " << m_wavelength << endl; 
  cout << "Alpha      " << r_alpha << " Actual " << m_alpha << endl; 
  cout << "Beta       " << r_beta << " Actual " << m_beta << endl; 
  cout << "Temp_North " << r_T_N << " Actual " << m_T_N << endl; 
  cout << "Temp_South " << r_T_S << " Actual " << m_T_S << endl; 

  postSensingScore(vname,error, score);

  return(true);
}


//---------------------------------------------------------
// Procedure: addMessage()

void CTDSensor_Model::addMessage(const string& varname,
			   const string& value)
{
  VarDataPair pair(varname, value);
  m_messages.push_back(pair);
}

//---------------------------------------------------------
// Procedure: addMessage()

void CTDSensor_Model::addMessage(const string& varname, double value)
{
  VarDataPair pair(varname, value);
  m_messages.push_back(pair);
}


//------------------------------------------------------------
// Procedure: postSensorReport()

void CTDSensor_Model::postSensorReport(double ptx, double pty, string vname)
{
  // Get the sensor range
  double temp = front.tempFunction(m_curr_time,ptx,pty);

  string gt = "vname=" + vname 
    + ",utc=" + doubleToString(m_curr_time,1)
    +",x=" + doubleToString(ptx,1)
    +",y=" + doubleToString(pty,1)
    +",temp=" + doubleToString(temp,2);
  
  addMessage("UCTD_TEMP_REPORT_" + toupper(vname), gt);

  double msmnt = front.tempMeas(m_curr_time, ptx, pty);

  string report = "vname=" + vname 
    + ",utc=" + doubleToString(m_curr_time,1)
    +",x=" + doubleToString(ptx,1)
    +",y=" + doubleToString(pty,1)
    +",temp=" + doubleToString(msmnt,2);
  
  addMessage("UCTD_MSMNT_REPORT_" + toupper(vname), report);
  
}

//------------------------------------------------------------
// Procedure: postSensingScore()

void CTDSensor_Model::postSensingScore(string vname, double error, double score)
{
  // Get the sensor range

  string gt = "vname=" + vname 
    + ",error=" + doubleToString(error)
    + ",score=" + doubleToString(score);
   
  addMessage("UCTD_SCORE_REPORT", gt);

}

//------------------------------------------------------------
// Procedure: setTermReportInterval

bool CTDSensor_Model::setTermReportInterval(string str)
{
  if(!isNumber(str)) {
    memoErr("Improper term_report_interval: " + str);
    return(false);
  }

  m_term_report_interval = atof(str.c_str());
  m_term_report_interval = vclip(m_term_report_interval, 0, 10);
  return(true);
}


//------------------------------------------------------------
// Procedure: memo()
//   Purpose: Collect output meant for the terminal on each iteration.
//            Duplicate memos simply have their counter increased so the
//               screen is not flooded with a line for each occurance. 
//            Some thought must be given to the memo content such that 
//            memos posted regularly do not differ in content on trivial
//            differences; to avoid terminal reports that run off the 
//            screen.

void CTDSensor_Model::memo(const string& memo_str)
{
  m_map_memos[memo_str]++;
}

//------------------------------------------------------------
// Procedure: memoErr()
//   Purpose: Same as the memo() function, but...
//            when these memos are sent to the terminal, they may be
//            in a different color indicating the error nature.

void CTDSensor_Model::memoErr(const string& memo_str)
{
  m_map_err_memos[memo_str]++;
}

void CTDSensor_Model::postTrueParameters()
{
  string sval;
  sval = "vname=shoreside";
  sval += ",offset=" + doubleToString(m_offset);
  sval += ",angle=" + doubleToString(m_angle);
  sval += ",amplitude=" + doubleToString(m_amplitude);
  sval += ",period=" + doubleToString(m_period);
  sval += ",wavelength=" + doubleToString(m_wavelength);
  sval += ",alpha=" + doubleToString(m_alpha);
  sval += ",beta=" + doubleToString(m_beta);
  sval += ",tempnorth=" + doubleToString(m_T_N);
  sval += ",tempsouth=" + doubleToString(m_T_S);
  addMessage("UCTD_TRUE_PARAMETERS",sval);
}
