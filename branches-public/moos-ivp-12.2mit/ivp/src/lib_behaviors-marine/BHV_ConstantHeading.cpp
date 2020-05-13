/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: BHV_ConstantHeading.cpp                              */
/*    DATE: Jul 21st 2005                                        */
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

#ifdef _WIN32
#pragma warning(disable : 4786)
#pragma warning(disable : 4503)
#endif
#include <iostream>
#include <cmath> 
#include <cstdlib>
#include "BHV_ConstantHeading.h"
#include "BuildUtils.h"
#include "ZAIC_PEAK.h"
#include "MBUtils.h"
#include "AngleUtils.h"

using namespace std;

//-----------------------------------------------------------
// Procedure: Constructor

BHV_ConstantHeading::BHV_ConstantHeading(IvPDomain gdomain) : 
  IvPBehavior(gdomain)
{
  this->setParam("descriptor", "bhv_constantHeading");

  m_domain = subDomain(m_domain, "course");

  m_desired_heading = 0;
  m_peakwidth       = 10;
  m_basewidth       = 170;
  m_summitdelta     = 25;

  // The default duration at the IvPBehavior level is "-1", which
  // indicates no duration applied to the behavior by default. By
  // setting to zero here, we force the user to provide a duration
  // value otherwise it will timeout immediately.
  m_duration        = 0;
}

//-----------------------------------------------------------
// Procedure: setParam

bool BHV_ConstantHeading::setParam(string param, string val) 
{
  if(IvPBehavior::setParam(param, val))
    return(true);
  
  double dval = atof(val.c_str());

  if((param == "heading") && isNumber(val)) {
    m_desired_heading = angle360(dval);
    return(true);
  }
  else if((param == "peakwidth") && isNumber(val)) {
    m_peakwidth = vclip_min(dval, 0);
    return(true);
  }
  else if((param == "basewidth") && isNumber(val)) {
    m_basewidth = vclip_min(dval, 0);
    return(true);
  }
  else if((param == "summitdelta") && isNumber(val)) {
    m_summitdelta = vclip(dval, 0, 100);
    return(true);
  }

  return(false);
}

//-----------------------------------------------------------
// Procedure: onRunState
//

IvPFunction *BHV_ConstantHeading::onRunState() 
{
  if(!m_domain.hasDomain("course")) {
    postEMessage("No 'heading/course' variable in the helm domain");
    return(0);
  }

  ZAIC_PEAK zaic(m_domain, "course");
  zaic.setSummit(m_desired_heading);
  zaic.setBaseWidth(m_basewidth);
  zaic.setPeakWidth(m_peakwidth);
  zaic.setSummitDelta(m_summitdelta);
  zaic.setValueWrap(true);
  
  IvPFunction *ipf = zaic.extractIvPFunction();
  if(ipf)
    ipf->setPWT(m_priority_wt);
  else 
    postEMessage("Unable to generate constant-heading IvP function");

  string zaic_warnings = zaic.getWarnings();
  if(zaic_warnings != "")
    postWMessage(zaic_warnings);

  return(ipf);
}










