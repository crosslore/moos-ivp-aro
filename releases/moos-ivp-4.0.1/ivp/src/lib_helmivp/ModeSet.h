/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: ModeSet.h                                            */
/*    DATE: Dec 26th, 2008                                       */
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

#ifndef MODE_SET_HEADER
#define MODE_SET_HEADER

#include <vector>
#include <string>
#include "ModeEntry.h"
#include "InfoBuffer.h"
#include "VarDataPair.h"

class ModeSet {
public:
  ModeSet() {m_info_buffer=0;};
  ~ModeSet() {};

  void addEntry(ModeEntry entry) {m_entries.push_back(entry);};

  void setInfoBuffer(InfoBuffer *b) {m_info_buffer = b;};
  
  unsigned int size() {return(m_entries.size());};
  
  void evaluate();
  void print();

  std::vector<VarDataPair> getVarDataPairs() 
    {return(m_mode_var_data_pairs);};
  std::string getModeSummary();

  std::vector<std::string> getConditionVars();

  std::string getStringDescription();

 protected:
  void consultFromInfoBuffer();
  void updateInfoBuffer();

protected:
  std::vector<ModeEntry>    m_entries;
  std::vector<VarDataPair>  m_mode_var_data_pairs;

  InfoBuffer *m_info_buffer;
};

#endif