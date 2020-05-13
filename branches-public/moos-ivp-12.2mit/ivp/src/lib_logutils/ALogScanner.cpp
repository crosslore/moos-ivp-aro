/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: ALogScanner.cpp                                      */
/*    DATE: June 3rd, 2008                                       */
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

#include "MBUtils.h"
#include "ALogScanner.h"
#include "LogUtils.h"
#include <cstdio>

using namespace std;

//--------------------------------------------------------
// Procedure: scan
//     Notes: 
ScanReport ALogScanner::scan()
{
  ScanReport report;
  bool done = false;
  while(!done) {
    ALogEntry entry = getNextRawALogEntry(m_file, true);
    string status = entry.getStatus();
    // Check for the end of the file
    if(status == "eof")
      done = true;
    // If otherwise a "normal" line, process
    else if(status != "invalid") {
      string src = entry.getSource();
      // If the source is the IvP Helm, then see if the behavior
      // information is present and append to the source
      if(src == "pHelmIvP") {
	string src_aux = entry.getSrcAux();
	string iter = biteString(src_aux, ':');
	string bhv  = src_aux;
	if(bhv != "")
	  src = src + ":" + bhv;
      }
      
      report.addLine(entry.getTimeStamp(),
		     entry.getVarName(),
		     src,
		     entry.getStringVal());
    }
  }
  return(report);
}

//--------------------------------------------------------
// Procedure: openALogFile

bool ALogScanner::openALogFile(string alogfile)
{
  m_file = fopen(alogfile.c_str(), "r");
  if(!m_file)
    return(false);
  else
    return(true);
}