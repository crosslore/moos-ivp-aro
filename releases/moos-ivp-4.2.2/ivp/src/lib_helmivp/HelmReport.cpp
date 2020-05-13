/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: HelmReport.cpp                                       */
/*    DATE: Sep 26th, 2006                                       */
/*                                                               */
/* (IvPHelm) The IvP autonomous control Helm is a set of         */
/* classes and algorithms for a behavior-based autonomous        */
/* control architecture with IvP action selection.               */
/*                                                               */
/* The algorithms embodied in this software are protected under  */
/* U.S. Pat. App. Ser. Nos. 10/631,527 and 10/911,765 and are    */
/* the property of the United States Navy.                       */
/*                                                               */
/* Permission to use, copy, modify and distribute this software  */
/* and its documentation for any non-commercial purpose, without */
/* fee, and without a written agreement is hereby granted        */
/* provided that the above notice and this paragraph and the     */
/* following three paragraphs appear in all copies.              */
/*                                                               */
/* Commercial licences for this software may be obtained by      */
/* contacting Patent Counsel, Naval Undersea Warfare Center      */
/* Division Newport at 401-832-4736 or 1176 Howell Street,       */
/* Newport, RI 02841.                                            */
/*                                                               */
/* In no event shall the US Navy be liable to any party for      */
/* direct, indirect, special, incidental, or consequential       */
/* damages, including lost profits, arising out of the use       */
/* of this software and its documentation, even if the US Navy   */
/* has been advised of the possibility of such damage.           */
/*                                                               */
/* The US Navy specifically disclaims any warranties, including, */
/* but not limited to, the implied warranties of merchantability */
/* and fitness for a particular purpose. The software provided   */
/* hereunder is on an 'as-is' basis, and the US Navy has no      */
/* obligations to provide maintenance, support, updates,         */
/* enhancements or modifications.                                */
/*****************************************************************/

#include "HelmReport.h"
#include "MBUtils.h"

using namespace std;

//-----------------------------------------------------------
// Constructor

HelmReport::HelmReport() 
{ 
  m_iteration     = 0;
  m_ofnum         = 0;
  m_time_utc      = 0;
  m_warning_count = 0;
  m_create_time   = 0;
  m_solve_time    = 0;
  m_loop_time     = 0;
  m_halted        = false;
}

//-----------------------------------------------------------
// Procedure: addDecision()

void HelmReport::addDecision(const string& var, double val) 
{
  m_decisions[var] = val;
}

//-----------------------------------------------------------
// Procedure: setDecision()

void HelmReport::setDecision(const string& var, double val) 
{
  m_decisions[var] = val;
}

//-----------------------------------------------------------
// Procedure: getDecision()

double HelmReport::getDecision(const string& var) 
{
  map<string, double>::iterator p;
  p = m_decisions.find(var);
  double result = 0;
  if(p != m_decisions.end())
    result = p->second;
  return(result);
}

//-----------------------------------------------------------
// Procedure: hasDecision()

bool HelmReport::hasDecision(const string& var) 
{
  map<string, double>::iterator p = m_decisions.find(var);
  if(p != m_decisions.end())
    return(true);
  else
    return(false);
}

//-----------------------------------------------------------
// Procedure: delDecision()

void HelmReport::delDecision(const string& var) 
{
  m_decisions.erase(var);
}


//-----------------------------------------------------------
// Procedure: addRunningBHV
//            addIdleBHV
//            addCompletedBHV

void HelmReport::addRunningBHV(const string& descriptor, double time, 
			       const string& update_summary)
{
  if(m_running_bhvs != "")
    m_running_bhvs += ":";
  m_running_bhvs += descriptor;
  m_running_bhvs += "$" + doubleToString(time, 1);
  m_running_bhvs += "$" + update_summary;
}

void HelmReport::addIdleBHV(const string& descriptor, double time,
			    const string& update_summary)
{
  if(m_idle_bhvs != "")
    m_idle_bhvs += ":";
  m_idle_bhvs += descriptor;
  m_idle_bhvs += "$" + doubleToString(time, 1);
  m_idle_bhvs += "$" + update_summary;
}

void HelmReport::addCompletedBHV(const string& descriptor, double time,
				 const string& update_summary)
{
  if(m_completed_bhvs != "")
    m_completed_bhvs += ":";
  m_completed_bhvs += descriptor;
  m_completed_bhvs += "$" + doubleToString(time, 1);
  m_completed_bhvs += "$" + update_summary;
}

//-----------------------------------------------------------
// Procedure: addActiveBHV

//   bhv_waypoint:100,bhv_avoid:200,bhv_opregion:100

void HelmReport::addActiveBHV(const string& descriptor, double time, 
			      double pwt, double pcs, double cpu_time, 
			      const string& update_summary, 
			      unsigned int ipfs, bool orig)
{
  if(m_active_bhvs != "")
    m_active_bhvs += ":";
  
  m_active_bhvs += descriptor;
  m_active_bhvs += "$" + doubleToString(time, 1);
  m_active_bhvs += "$" + doubleToStringX(pwt, 2);
  m_active_bhvs += "$" + doubleToStringX(pcs);
  m_active_bhvs += "$" + doubleToString(cpu_time,2);
  m_active_bhvs += "$" + update_summary;
  m_active_bhvs += "$" + uintToString(ipfs);
  m_active_bhvs += "$" + uintToString(orig);
}


//-----------------------------------------------------------
// Procedure: getRunningBehaviors()

string HelmReport::getRunningBehaviors() const
{
  return(m_running_bhvs);
}

//-----------------------------------------------------------
// Procedure: getActiveBehaviors()

string HelmReport::getActiveBehaviors() const
{
  return(m_active_bhvs);
}

//-----------------------------------------------------------
// Procedure: getIdleBehaviors()

string HelmReport::getIdleBehaviors() const
{
  return(m_idle_bhvs);
}

//-----------------------------------------------------------
// Procedure: getCompletedBehaviors()

string HelmReport::getCompletedBehaviors() const
{
  return(m_completed_bhvs);
}

//-----------------------------------------------------------
// Procedure: getReportAsString()

string HelmReport::getReportAsString()
{
  string report;

  report += ("iter=" + uintToString(m_iteration));
  report += (",ofnum=" + uintToString(m_ofnum));
  report += (",warnings=" + uintToString(m_warning_count));
  report += (",utc_time=" + doubleToString(m_time_utc, 2));
  report += (",solve_time=" + doubleToString(m_solve_time, 2));
  report += (",create_time=" + doubleToString(m_create_time, 2));
  report += (",loop_time=" + doubleToString(m_loop_time, 2));
  
  map<string, double>::iterator p;
  for(p=m_decisions.begin(); p!=m_decisions.end(); p++) {
    string var = p->first;
    double val = p->second;
    report += ",var=" + var + ":";
    report += doubleToString(val, 1);
  }

  // Now check to see if the helm did not produce a decision for one
  // of the variables in the originally declared domain for the helm
  unsigned int i, dsize = m_domain.size();
  for(i=0; i<dsize; i++) {
    string varname = m_domain.getVarName(i);
    if(!hasDecision(varname))
      report += (",var=" + varname + ":varbalk");
  }

  if(m_halted)
    report += ",halted=true";
  else
    report += ",halted=false";

  report += ",running_bhvs=";
  if(m_running_bhvs == "")
    report += "none";
  else
    report += m_running_bhvs;

  report += ",modes=";
  if(m_modes == "")
    report += "none";
  else
    report += m_modes;

  report += ",active_bhvs=";
  if(m_active_bhvs == "")
    report += "none";
  else
    report += m_active_bhvs;

  report += ",idle_bhvs=";
  if(m_idle_bhvs == "")
    report += "none";
  else
    report += m_idle_bhvs;
  
  report += ",completed_bhvs=";
  if(m_completed_bhvs == "")
    report += "none";
  else
    report += m_completed_bhvs;

  return(report);
}


//-----------------------------------------------------------
// Procedure: getReportAsString(const HelmReport&)

string HelmReport::getReportAsString(const HelmReport& prep)
{
  string report = "iter=" + uintToString(m_iteration);
  report += (",utc_time=" + doubleToString(m_time_utc, 2));

  if(m_ofnum != prep.getOFNUM())
    report += (",ofnum=" + uintToString(m_ofnum));
  if(m_warning_count != prep.getWarnings())
    report += (",warnings=" + uintToString(m_warning_count));
  if(m_solve_time != prep.getSolveTime())
    report += (",solve_time=" + doubleToString(m_solve_time, 2));
  if(m_create_time != prep.getCreateTime())
    report += (",create_time=" + doubleToString(m_create_time, 2));
  if(m_loop_time != prep.getLoopTime())
    report += (",loop_time=" + doubleToString(m_loop_time, 2));
  
  m_decision_summary = "";
  map<string, double>::iterator p;
  for(p=m_decisions.begin(); p!=m_decisions.end(); p++) {
    string var = p->first;
    double val = p->second;
    m_decision_summary += ",var=" + var + ":";
    m_decision_summary += doubleToString(val, 1);
  }

  // Now check to see if the helm did not produce a decision for one
  // of the variables in the originally declared domain for the helm
  unsigned int i, dsize = m_domain.size();
  for(i=0; i<dsize; i++) {
    string varname = m_domain.getVarName(i);
    if(!hasDecision(varname))
      m_decision_summary += (",var=" + varname + ":varbalk");
  }

  if(m_decision_summary != prep.getDecisionSummary())
    report += m_decision_summary;

  if(m_halted != prep.getHalted())
    report += (",halted=" + boolToString(m_halted));

  if(m_modes != prep.getModeSummary()) {
    report += ",modes=";
    if(m_modes == "")
      report += "none";
    else
      report += m_modes;
  }

  if(m_running_bhvs != prep.getRunningBehaviors()) {
    report += ",running_bhvs=";
    if(m_running_bhvs == "")
      report += "none";
    else
      report += m_running_bhvs;
  }

  if(m_active_bhvs != prep.getActiveBehaviors()) {
    report += ",active_bhvs=";
    if(m_active_bhvs == "")
      report += "none";
    else
      report += m_active_bhvs;
  }

  if(m_idle_bhvs != prep.getIdleBehaviors()) {
    report += ",idle_bhvs=";
    if(m_idle_bhvs == "")
      report += "none";
    else
      report += m_idle_bhvs;
  }

  if(m_completed_bhvs != prep.getCompletedBehaviors()) {
    report += ",completed_bhvs=";
    if(m_completed_bhvs == "")
      report += "none";
    else
      report += m_completed_bhvs;
  }

  return(report);
}


