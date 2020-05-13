/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: CBA_MOOSApp.cpp                                      */
/*    DATE: Aug 17th 2011 (Waterloo)                             */
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

#include <iterator>
#include "CBA_MOOSApp.h"
#include "ColorParse.h"
#include "MBUtils.h"

using namespace std;

//---------------------------------------------------------
// Procedure: OnNewMail

bool CBA_MOOSApp::OnNewMail(MOOSMSG_LIST &NewMail)
{
  m_model.setCurrTime(MOOSTime());
  MOOSMSG_LIST::iterator p;
  for(p=NewMail.begin(); p!=NewMail.end(); p++) {
    CMOOSMsg &msg = *p;
	
    bool ok;
    ok = m_model.handleMsg(msg.GetKey(),msg.GetDouble(), msg.GetString(),
			   msg.GetTime(), msg.IsDouble(), msg.IsString(),
			   msg.GetSource());
    if(!ok) {
      cout << termColor("magenta") << endl;
      cout << "Unhandled Message: " << msg.GetKey() << endl;
      cout << termColor() << flush;
    }
  }
  return(true);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool CBA_MOOSApp::OnConnectToServer()
{
  RegisterVariables();  
  return(true);
}


//------------------------------------------------------------
// Procedure: RegisterVariables

void CBA_MOOSApp::RegisterVariables()
{
  m_Comms.Register("FOOBAR", 0);
}


//---------------------------------------------------------
// Procedure: Iterate()

bool CBA_MOOSApp::Iterate()
{
  m_model.setCurrTime(MOOSTime());
  m_model.iterate();
  postMessages(m_model.getMessages());
  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()

bool CBA_MOOSApp::OnStartUp()
{
  cout << termColor("blue");
  MOOSTrace("\npCBBA starting...\n");
  m_model.setCurrTime(MOOSTime());
  
  STRING_LIST sParams;
  m_MissionReader.GetConfiguration(GetAppName(), sParams);

  STRING_LIST::iterator p;
  for(p = sParams.begin(); p!=sParams.end(); p++) {
    string sLine  = *p;
    string param  = biteStringX(sLine, '=');
    string value  = sLine;
    bool ok = false;
    if((tolower(param) == "apptick") ||
       (tolower(param) == "commstick"))
      ok = true;
    else {
      unsigned int pass, total_passes = 3;
      for(pass=0; pass<total_passes; pass++)
	ok = ok || m_model.setParam(param, value, pass);
      if(!ok) {
	cout << termColor("red");
	cout << "WARNING: Unhandled configuration line:" << endl;
	cout << termColor("black");
	cout << "   Parameter=" << param << " Value=" << value << endl;
	cout << termColor("blue");
      }
    }
  }
  
  RegisterVariables();

  Figlog figlog = m_model.getFigLog();
  figlog.print();

  m_model.print();
  MOOSTrace("pCBBA started. \n\n");
  cout << termColor() << flush;
  return(true);
}


//---------------------------------------------------------
// Procedure: postMessages()

void CBA_MOOSApp::postMessages(vector<VarDataPair> msgs)
{
  bool verbose = m_model.verbose();

  unsigned int i, vsize = msgs.size();
  for(i=0; i<vsize; i++) {
    string varname = msgs[i].get_var();
    if(verbose)
      cout << "DBPost: " << varname << "  ";;
    if(msgs[i].is_string()) {
      m_Comms.Notify(varname, msgs[i].get_sdata());
      if(verbose)
	cout << msgs[i].get_sdata() << endl;
    }
    else {
      m_Comms.Notify(varname, msgs[i].get_ddata());
      if(verbose)
	cout << msgs[i].get_ddata() << endl;
    }
  }
}

