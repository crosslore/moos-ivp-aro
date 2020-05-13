/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: Expander.cpp                                         */
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

#include <cstdio>
#include <cstdlib>
#include <string>
#include <iostream>
#include "TermUtils.h"
#include "MBUtils.h"
#include "FileBuffer.h"
#include "Expander.h"
#include "ColorParse.h" // For debugging

using namespace std;

//--------------------------------------------------------
// Constructor

Expander::Expander(string given_infile, string given_outfile)
{
  m_infile  = given_infile;
  m_outfile = given_outfile;
  m_force = false;  
  m_strict = false;
  
  m_max_subs_per_line = 100;
  m_initial_filenames.push_back(given_infile);
  m_path.push_back(".");
  m_pmode.push_back("top");
}

//--------------------------------------------------------
// Procedure: expand
//     Notes: key functions, chompString and stripBlankEnds
//            are from MBUtils.h

bool Expander::expand()
{
  bool result;
  vector<string> fvector = expandFile(m_infile, m_initial_macros, 
				      m_initial_filenames, result);

  if(result == false) {
    cout << "#  Aborting without writing the new file." << endl;
    return(false);
  }
  else
    m_newlines = fvector;
  
  writeOutput();
  return(true);
}
  
//--------------------------------------------------------
// Procedure: expandFile
//     Notes: key functions, chompString and stripBlankEnds
//            are from MBUtils.h

vector<string> Expander::expandFile(string filename, 
				    map<string, string>& macros, 
				    vector<string> filenames, 
				    bool& result)
{
  vector<string> return_vector;
  vector<string> empty_vector;

  vector<string> fvector = fileBuffer(filename);
  unsigned int i, vsize = fvector.size();
  if(vsize == 0) {
    cout << "#  Warning: The file " << filename << " was empty." << endl;    
    result = true;
    return(empty_vector);
  }

  for(i=0; i<vsize; i++) {
    string line = stripBlankEnds(findReplace(fvector[i], '\t', ' '));
    string line_orig = line;
    string left = stripBlankEnds(biteString(line, ' '));
    string rest = stripBlankEnds(line);

    //------------------------------------------------------------
    if(left == "#ifdef") {
      if(!skipLines()) {
	bool ifdef = checkIfDef(rest, macros);
	if(!ifdef)
	  pushMode("ifdefno");
	else
	  pushMode("ifdefyes");
      }
      else {
	string curr_mode = currMode();
	pushMode(curr_mode);
      }
    }
    
    //------------------------------------------------------------
    else if(left == "#elseifdef") {
      if((currMode() != "ifdefyes") && (currMode() != "ifdefno") &&
	 (currMode() != "ifdefnomore")) {
	cout << "#  Error in file " << filename << " line:" << i+1 << endl;
	cout << "#  Dangling #elseifdef" << endl;
	result = false;
	return(empty_vector);
      }
      
      // Being in the ifdefyes mode means one of the "above" ifdef cases
      // matched and the current #elseifdef is moot.
      if(currMode() == "ifdefyes")
	currMode("ifdefnomore");
      else if(currMode() == "ifdefno") {
	bool ifdef = checkIfDef(rest, macros);
	if(ifdef)
	  currMode("ifdefyes");
      }
    }

    //------------------------------------------------------------
    else if(left == "#else") {
      if(currMode() == "top") {
	cout << "#  Error in file " << filename << " line:" << i+1 << endl;
	cout << "#  Dangling #else" << endl;
	result = false;
	return(empty_vector);
      }

      // Being in the ifdefyes mode means one of the "above" ifdef cases
      // matched and the current #else is moot.
      if((currMode() == "ifdefyes") || (currMode() == "ifndefyes"))
	currMode("elseno");
      else if((currMode() == "ifdefno") || (currMode() == "ifndefno"))
	currMode("elseyes");
    }

    //------------------------------------------------------------
    else if(left == "#ifndef") {
      if(!skipLines()) {
	bool ifndef = checkIfNDef(rest, macros);
	if(!ifndef)
	  pushMode("ifndefno");
	else
	  pushMode("ifndefyes");
      }
      else {
	string curr_mode = currMode();
	pushMode(curr_mode);
      }
    }

    //-------------------------------------------------------------
    else if(left == "#endif") {
      if(currMode() == "top") {
	cout << "#  Error in file " << filename << " line:" << i+1 << endl;
	cout << "#  #endif detected with no open #ifdef or #ifndef" << endl;
	result = false;
	return(empty_vector);
      }
      popMode();
    }

    //--------------------------------------------------------------
    else if(!skipLines() && (left == "#include")) {
      applyMacrosToLine(rest, macros);
      string file_str = stripBlankEnds(rest);
      if(isQuoted(file_str))
	file_str = stripQuotes(file_str);
      string full_file_str = findFileInPath(file_str);

      if(!verifyInfile(full_file_str)) {
	cout << "#  Error in file " << filename << " line:" << i+1 << endl;
	cout << "#  The #include file \"" << file_str << "\" not found." << endl;
	result = false;
	return(empty_vector);
      }
	
      if(vectorContains(filenames, full_file_str)) {
	cout << "#  Error in file " << filename << " line:" << i+1 << endl;
	cout << "#  A #include LOOP was detected - " << endl;
	cout << "#  trying to #include file " << file_str << endl;
	result = false;
	return(empty_vector);
      }
      vector<string> ilines = expandFile(full_file_str, macros, 
					 filenames, result);
      if(!result) {
	cout << "#  (Included from file " << filename << ")" << endl;
	return(empty_vector);
      }

      return_vector = mergeVectors(return_vector, ilines);
    }

    //----------------------------------------------------------------
    else if(!skipLines() && (left == "#define")) {
      rest = compactConsecutive(rest, ' ');
      string macro = stripBlankEnds(biteString(rest, ' '));
      string value = stripBlankEnds(rest);

      // Support "#define $(FOO) BAR" as well as "#define FOO BAR"
      int mlen = macro.length();
      if((mlen > 3) && (macro.at(0)=='$') &&
	 (macro.at(1)=='(') && (macro.at(mlen-1)==')'))
	macro = macro.substr(2, mlen-3);
	 
      if(macro == "") {
	cout << "#  Error in file " << filename << " line:" << i+1 << endl;
	cout << "#  A #define line must have an argument" << endl;
	result = false;
	return(empty_vector);
      }
      else {
	if(value == "") 
	  value = "<defined>";
	macros[macro] = value;
      }
    }
    else if(!skipLines()) {
      applyMacrosToLine(fvector[i], macros);
      return_vector.push_back(fvector[i]);
    }   

#if 0 // BEGIN DEBUGGING OUTPUT BLOCK
    string pline = fvector[i];
    if(pline.length() > 0)
      pline.at(pline.length()-1) = '\0';

    cout << "Line[" << i+1 << "]: " << pline << flush;
    cout << termColor("blue");
    cout << "   (" << currMode() << ")   ";

    if(skipLines())
      cout << termColor("red") << "skip" << termColor();
    else
      cout << termColor("green") << "ok" << termColor();
    cout << endl;
#endif // END DEBUGGING OUTPUT BLOCK


  }   
  

  result = true;
  return(return_vector);
}
  
//--------------------------------------------------------
// Procedure: addMacro()

void Expander::addMacro(string key, string value, bool over_ok)
{
  m_initial_macros[key] = value;
}


//--------------------------------------------------------
// Procedure: verifyInfile()

bool Expander::verifyInfile()
{
  return(verifyInfile(m_infile));
}

//--------------------------------------------------------
// Procedure: verifyInfile()

bool Expander::verifyInfile(const string& filename)
{
  FILE *f = fopen(filename.c_str(), "r");
  if(f)
    fclose(f);
  else
    return(false);
  return(true);
}

//--------------------------------------------------------
// Procedure: writeOutput

bool Expander::writeOutput()
{
  //  Abort condition: Output file exists but cannot
  //  be overwritten. tests:  fopen(r), !fopen(r+)

  FILE *f = fopen(m_outfile.c_str(), "r");
  if(f) {
    fclose(f); 
    f = fopen(m_outfile.c_str(), "r+");
    if(f)
      fclose(f);
    else {
      cout << "Aborted: The file " << m_outfile;
      cout << " already exists and cannot be overwritten." << endl;
      return(false);
    }
  }
    
  // Abort condition: Output file exists, and CAN be overwritten
  // but the user declines to do so. tests: fopen(r+)

  f = fopen(m_outfile.c_str(), "r+");
  if(f) {
    fclose(f);
    bool done = false;
    while(!done && !m_force) {
      cout << "Warning: File " << m_outfile << " already exists.";
      
      char answer = 'y';
      if(!m_force) {
	cout <<  " Replace?(y/n)" << endl;
	answer = getCharNoWait();
      }

      if(answer == 'n') {
	cout << "Aborted: The file " << m_outfile;
	cout << " will not be created" << endl;
	return(false);
      }
      if(answer == 'y')
	done = true;
    }
  }

  // Fourth abort condition: Output file cannot be written to
  // created or overwritten. Tests: !fopen(w), 

  f = fopen(m_outfile.c_str(), "w");
  if(!f) {
    cout << "Aborted: The file" << m_outfile;
    cout << " cannot be written to" << endl;
    return(false);
  }

  for(unsigned int i=0; i<m_newlines.size(); i++)
    fprintf(f, "%s\n", m_newlines[i].c_str());

  fclose(f);

  if (!m_force)
      cout << "...successfully completed" << endl;
  
  return(true);
}

//--------------------------------------------------------
// Procedure: addPath

void Expander::addPath(string str)
{
  vector<string> svector = parseString(str, ':');
  unsigned int i, vsize = svector.size();
  for(i=0; i<vsize; i++) {
    string new_entry = stripBlankEnds(svector[i]);
    m_path.push_back(new_entry);
  }
}

//--------------------------------------------------------
// Procedure: applyMacrosToLine

bool Expander::applyMacrosToLine(string& line, 
				 map<string, string> macros)
{
  map<string, string>::iterator p;

  int    subs = 0;
  bool   done = false;
  string newline = line;
  while(!done) {
    bool substitution_made = false;
    for(p = macros.begin(); p != macros.end(); p++) {
      string key = "$(" + p->first + ")";
      string val = p->second;
      
      // Assuming key is of the form "$(FOOBAR)"
      string pkey = key;
      if((pkey.length() > 0) && (pkey.at(0) == '$'))
	pkey.at(0) = '%';
      
      if(strContains(newline, pkey)) {
	newline = findReplace(newline, pkey, toupper(val));
	substitution_made = true;
	subs++;
      }
      
      if(strContains(newline, key)) {
	newline = findReplace(newline, key, val);
	substitution_made = true;
	subs++;
      }
    }
    if(!substitution_made || (subs > m_max_subs_per_line))
      done = true;
  }

  string res = containsMacro(newline);

  if(res != "") {
    cout << "Warning: The following line of " << m_infile << endl;
    cout << "  creating " << m_outfile << endl;
    cout << "  may contain an undefined macro:" << endl;
    cout << "> " << res << endl;

    if(m_strict)
        exit(EXIT_FAILURE);
    
    return(false);
  }
  else {
    line = newline;
    return(true);
  }
}

//--------------------------------------------------------
// Procedure: containsMacro()

string Expander::containsMacro(string line)
{
  int pos = line.find("$(");
  if(pos != -1)
    return(line);
  else {
    pos = line.find("%(");
    if(pos != -1)
      return(line);
    else
      return("");
  }
}

//--------------------------------------------------------
// Procedure: findFileInPath
//      Note: Looking in the current directory (./) is only done if
//            either it was explicitly added to the path, or if the
//            path itself is empty.

string Expander::findFileInPath(string filename)
{
  unsigned int i, vsize = m_path.size();
  if(vsize == 0)
    return(filename);

  for(i=0; i<vsize; i++) {
    string full_name = (m_path[i] + "/" + filename);
    FILE *f = fopen(full_name.c_str(), "r");
    if(f) {
      fclose(f);
      return(full_name);
    }
  }
  return("");
}

//--------------------------------------------------------
// Procedure: checkIfDef

bool Expander::checkIfDef(string entry, map<string, string> macros)
{
  // Assume this ifdef does not hold unless one of the ifdef 
  // macro-value pairs holds. The #ifdef construct supports the
  // disjunction of macro-value pairs.
  bool ifdef = false;
  
  vector<string> kvector = parseString(entry, "||"); 
  unsigned int k, ksize = kvector.size();
  for(k=0; ((k<ksize) && !ifdef); k++) {
    kvector[k] = stripBlankEnds(kvector[k]);
    string macro_name  = stripBlankEnds(biteString(kvector[k], ' '));
    string macro_value = stripBlankEnds(kvector[k]);
    if(macro_value == "") {
      if(macros[macro_name] != "")
	ifdef = true;
    }
    else if(macros[macro_name] == macro_value)
      ifdef = true;
  }
  
  return(ifdef);
}

//--------------------------------------------------------
// Procedure: checkIfNDef

bool Expander::checkIfNDef(string entry, map<string, string> macros)
{
  // If "entry" is empty then declare this ifndef to be false
  // If it is non-empty make sure each component is found.
  bool ifndef = false;
  if(entry != "")
    ifndef = true;
  
  // Now go thru each clause, ensure that each is not defined
  bool done = false;
  while(ifndef && !done) {
    string clause = stripBlankEnds(biteString(entry, ' '));
    if(clause == "")
      done = true;
    else {
      if(macros[clause] != "")
	ifndef = false;
    }
  }
  return(ifndef);
}

//--------------------------------------------------------
// Procedure: currMode(string)

void Expander::currMode(string new_mode)
{
  unsigned int vsize = m_pmode.size();
  if(vsize > 1)
    m_pmode[vsize-1] = new_mode;
}

//--------------------------------------------------------
// Procedure: pushMode(string)

void Expander::pushMode(string new_mode)
{
    m_pmode.push_back(new_mode);
}

//--------------------------------------------------------
// Procedure: currMode()

string Expander::currMode()
{
  unsigned int vsize = m_pmode.size();
  if(vsize > 0)
    return(m_pmode[vsize-1]);
  else
    return("error - stack underflow");
}

//--------------------------------------------------------
// Procedure: popMode()

bool Expander::popMode()
{
    unsigned int vsize = m_pmode.size();
    if(vsize > 1) {
        m_pmode.pop_back();
        return(true);
    }
  else
    return(false);
}

//--------------------------------------------------------
// Procedure: skipLines
//      Notr: Determines if, given the current stack of modes,
//            whether a currently considered line should be
//            skipped.

bool Expander::skipLines()
{
  string curr_mode = currMode();
  if(modeStackContains("ifdefno"))
    return(true);
  if(modeStackContains("ifdefnomore"))
    return(true);
  if(modeStackContains("elseno"))
    return(true);
  if(modeStackContains("ifndefno"))
    return(true);

  if(curr_mode == "top")
    return(false);
  if(curr_mode == "ifdefyes")
    return(false);
  if(curr_mode == "elseyes")
    return(false);
  if(curr_mode == "ifndefyes")
    return(false);
  return(true);
}

//--------------------------------------------------------
// Procedure: modeStackContains
//      Note: Returns true if the given string is anywhere
//            in the stack of saved modes.

bool Expander::modeStackContains(string str)
{
  unsigned int i, vsize = m_pmode.size();
  for(i=0; i<vsize; i++) {
    if(m_pmode[i] == str)
      return(true);
  }
  return(false);
}

