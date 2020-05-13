/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: Expander.h                                           */
/*    DATE: May 28th, 2005                                       */
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

#ifndef EXPANDER_HEADER
#define EXPANDER_HEADER

#include <vector>
#include <string>
#include <map>

class Expander
{
 public:
  Expander(std::string, std::string);
  ~Expander() {};
  
  bool expand();
  bool verifyInfile();
  bool verifyInfile(const std::string& filename);
  bool writeOutput();
  void addMacro(std::string, std::string, bool=false);
  void setForce(bool v) {m_force=v;};
  void setStrict(bool v) {m_strict=v;};
  void addPath(std::string);

 protected:
  std::vector<std::string> 
    expandFile(std::string filename,
	       std::map<std::string, std::string>& macros,
	       std::vector<std::string> filenames, 
	       bool& result);
  
  bool applyMacrosToLine(std::string&, 
			 std::map<std::string, std::string>);

  std::string containsMacro(std::string);
  std::string findFileInPath(std::string);

  bool checkIfDef(std::string, std::map<std::string, std::string>);
  bool checkIfNDef(std::string, std::map<std::string, std::string>);

  void currMode(std::string);
  void pushMode(std::string);
  bool popMode();
  std::string currMode();
  bool skipLines();

  bool modeStackContains(std::string);
    
 private:
  std::vector<std::string> m_path;
  std::vector<std::string> m_newlines;

  // For preventing self-referencing include-loops
  std::vector<std::string>           m_initial_filenames;
  std::map<std::string, std::string> m_initial_macros;

  std::string m_infile;
  std::string m_outfile;

  // if m_strict, quit if undefined macros are found
  bool m_strict;
  bool m_force;
  int  m_max_subs_per_line;
  std::vector<std::string> m_pmode;

};

#endif 
