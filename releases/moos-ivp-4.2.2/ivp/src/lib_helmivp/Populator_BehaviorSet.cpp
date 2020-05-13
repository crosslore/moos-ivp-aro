/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: Populator_BehaviorSet.cpp                            */
/*    DATE:                                                      */
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
#ifdef _WIN32
#pragma warning(disable : 4786)
#endif

#ifdef _WIN32
#define strncasecmp _strnicmp
#endif

#include <iostream>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include "MBUtils.h"
#include "FileBuffer.h"
#include "Populator_BehaviorSet.h"

using namespace std;

//-------------------------------------------------------------
// Procedure: Constructor

Populator_BehaviorSet::Populator_BehaviorSet(IvPDomain g_domain,
					     InfoBuffer *g_buffer)
{
  m_domain        = g_domain;
  m_info_buffer   = g_buffer;
  m_parse_mode    = "top";

  m_bfactory_static.setDomain(g_domain);
  m_bfactory_dynamic.setDomain(g_domain);

  m_mode_set.setInfoBuffer(g_buffer);
}

//-------------------------------------------------------------
// Procedure: populate

BehaviorSet *Populator_BehaviorSet::populate(set<string> bhv_files)
{
  cout << "Number of behavior files: " << bhv_files.size() << endl;
  unsigned int  line_ix;
  set<string>::const_iterator p;
  for(p=bhv_files.begin(); p!=bhv_files.end(); p++) {

    string filename = *p;

    FILE *f = fopen(filename.c_str(), "r");
    
    cout << "Processing Behavior File: " << filename << "  START" << endl;

    if(!f) 
      cout << "    Could not find File: " << filename << endl;
    else {
      cout << "    Successfully found file: " << filename << endl;
      fclose(f);
      
      vector<string> file_vector = fileBufferSlash(filename);
      unsigned int lineCount = file_vector.size();
    
      for(line_ix=0; line_ix<lineCount; line_ix++) {
	string line = stripBlankEnds(file_vector[line_ix]);

	bool is_comment = false;
	line = stripBlankEnds(line);
	if(!strncmp("//", line.c_str(), 2))
	  is_comment = true;
	if(!strncmp("#", line.c_str(), 1))
	  is_comment = true;
	
	// Begin Brace-Separate.  If the beginning or end of a line
	// has a brace, pretend as if it was given on two separate
	// lines.
	unsigned int len = line.length();	
	string pre_line, post_line;
	if(!is_comment) {
	  unsigned left_braces = charCount(line, '{');
	  unsigned right_braces = charCount(line, '{');
	  if((len>1) && ((line.at(0) == '{') || (line.at(0) == '}'))) {
	    pre_line += line.at(0);
	    line.erase(0,1);
	  }
	  len = line.length();	
	  if(len > 1) {
	    if(line.at(len-1) == '{') {
	      post_line += line.at(len-1);
	      line.erase(len-1,1);
	    }
	    if((right_braces > left_braces) && (line.at(len-1) == '}')) {
	      post_line += line.at(len-1);
	      line.erase(len-1,1);
	    }
	  }
	}
	// End Brace-Separate.
	  
	bool ok = true;
	if(pre_line != "")
	  ok = ok && handleLine(pre_line, line_ix);
	ok = ok && handleLine(line, line_ix);
	if(post_line != "")
	  ok = ok && handleLine(post_line, line_ix);
	
	//cout << "After line " << i+1 << " mode:[" << m_parse_mode
	//<< "]" << endl; cout << "(" << line << ")" << endl;
	
	if(!ok) {
	  cout << "    Problem with line " << line_ix+1;
	  cout << "    in the BehaviorSet file: " << filename << endl;
	  cout << "Pre_line: [" << pre_line << "]" << endl;
	  cout << "Post_line: [" << post_line << "]" << endl;
	  cout << "Line:     [" << line     << "]" << endl;
	  return(0);
	}

      }
    }
    cout << "Processing Behavior File: " << filename << "  END" << endl;
  }

  // Build the behaviorset with specs and try to instantiate all the
  // behaviors. If some fail instantiation, abort the behaviorset.
  BehaviorSet *bset = new BehaviorSet;
  bset->setDomain(m_domain);
  unsigned int i;
  for(i=0; i<m_behavior_specs.size(); i++)
    bset->addBehaviorSpec(m_behavior_specs[i]);
  bool ok = bset-> buildBehaviorsFromSpecs();
  if(!ok) {
    delete(bset);
    return(0);
  }    

  // Given that all the behaviors were able to be instantiated from
  // their specs, fill out the rest of the behaviorset and return it.
  bset->connectInfoBuffer(m_info_buffer);
  for(i=0; i<initial_vars.size(); i++)
    bset->addInitialVar(initial_vars[i]);
  for(i=0; i<default_vars.size(); i++)
    bset->addDefaultVar(default_vars[i]);
  bset->setModeSet(m_mode_set);
  string sval = m_mode_set.getStringDescription();
  cout << "mode description: " << sval << endl;

  return(bset);
}

//-------------------------------------------------------------
// Procedure: populate
//      Note: A convenience function when the behaviors are 
//            given by only one file.

BehaviorSet *Populator_BehaviorSet::populate(string filestr)
{
  set<string> bhv_files;
  bhv_files.insert(filestr);
  return(populate(bhv_files));
}

//----------------------------------------------------------
// Procedure: printBehaviorSpecs()

void Populator_BehaviorSet::printBehaviorSpecs()
{
  unsigned int i, vsize = m_behavior_specs.size();
  for(i=0; i<vsize; i++) {
    m_behavior_specs[i].print();
  }
}

//----------------------------------------------------------
// Procedure: handleLine
//   Returns: true  if all OK
//            false otherwise
//
bool Populator_BehaviorSet::handleLine(string line, 
				       unsigned int line_num)
{
  // Comments are anything to the right of a "#" or "//"
  line = stripComment(line, "//");
  line = stripComment(line, "#");
  line = stripBlankEnds(line);

  // HANDLE A BLANK LINE or COMMMENT LINE 
  if(line.size() == 0)  { 
    if(m_parse_mode == "misc-defined-ish") 
      m_parse_mode = "top";
    else if(m_parse_mode == "set-defined-ish") {
      m_parse_mode = "top";
      m_mode_set.addEntry(m_mode_entry);
      m_mode_entry.clear();
    }
    return(true);  
  }
  
  // Handle LEFT BRACE
  if(line == "{") {
    if(m_parse_mode == "top")
      m_parse_mode = "misc-defining";
    else if(m_parse_mode == "bhv-declared")
      m_parse_mode = "bhv-defining";
    else if(m_parse_mode == "set-declared")
      m_parse_mode = "set-defining";
    else { 
      cout << "Unexpected open brace '{'" << endl;
      return(false);
    }
    return(true);
  }

  // Handle RIGHT BRACE
  if(line == "}") {
    if(m_parse_mode == "misc-defining")
      m_parse_mode = "misc-defined-ish";
    else if(m_parse_mode == "bhv-defining") {
      m_parse_mode = "top";
      m_behavior_specs.push_back(m_curr_bhv_spec);
      m_curr_bhv_spec.clear();
    }
    else if(m_parse_mode == "set-defining")
      m_parse_mode = "set-defined-ish";
    else { 
      cout << "Unexpected close brace '}'" << endl;
      return(false);
    }
    return(true);
  }
    
  bool ish_mode = false;
  if((m_parse_mode == "misc-defined-ish") || 
     (m_parse_mode == "set-defined-ish"))
    ish_mode = true;
    
  // In each of the strncasecmp lines below we pattern match on the
  // string PLUS ONE BLANK, to allow for the possibility that a 
  // symbol such as "set", if it were alone on a line, might actually
  // be something picked up by one of the ish_modes.
  if((m_parse_mode == "top") || (ish_mode == true)) {
    if((!strncasecmp("initialize ", line.c_str(), 11)) ||
       (!strncasecmp("initialize_ ", line.c_str(), 12))) {
      if(m_parse_mode == "set-defined-ish")
	closeSetMode();
      m_parse_mode = "top";
      string init_str = biteString(line, ' ');
      string init_val = stripBlankEnds(line);
      vector<string> init_vector = parseStringQ(line, ',');
      unsigned int iv, ivsize = init_vector.size();
      for(iv=0; iv<ivsize; iv++) {
	string pair = stripBlankEnds(init_vector[iv]);
	string left = stripBlankEnds(biteString(pair, '='));
	string right = stripBlankEnds(pair);
	if((left=="") || strContainsWhite(left) || (right==""))
	  return(false);
	VarDataPair msg(left, right, "auto");
	if(init_str == "initialize_")
	  msg.set_key("defer");
	else
	  msg.set_key("post");
	initial_vars.push_back(msg);
      }
      return(true);
    }
    else if(!strncasecmp("behavior ", line.c_str(), 9)) {
      if(m_parse_mode == "set-defined-ish")
	closeSetMode();
      string bhv_str  = biteString(line, '=');
      string bhv_name = stripBlankEnds(line);
      m_curr_bhv_spec.setBehaviorKind(bhv_name, line_num);
      m_parse_mode = "bhv-declared";
      return(true);
    }    
    else if(!strncasecmp("set ", line.c_str(), 4)) {
      if(m_parse_mode == "set-defined-ish")
	closeSetMode();
      string set_str = tolower(biteString(line, ' '));
      string set_val = stripBlankEnds(line);
      m_mode_entry.clear();
      string mode_var = stripBlankEnds(biteString(set_val, '='));
      string mode_val = stripBlankEnds(set_val);
      if((mode_var != "") && (mode_val != ""))
	m_mode_entry.setHead(mode_var, mode_val);
      else
	return(false);
      m_parse_mode = "set-declared";
      return(true);
    }
  }

  // If we're in a misc-defining or -ish mode, ignore any non-empty
  // line as the possible trailing entry(ies).
  if((m_parse_mode == "misc-defining") || 
     (m_parse_mode == "misc-defining-ish"))
    return(true);
 
  if(m_parse_mode == "set-defined-ish") {
    bool ok = m_mode_entry.setElseValue(line);
    if(ok) 
      closeSetMode();
    m_parse_mode = "top";
    return(ok);
  }
  
  if(m_parse_mode == "bhv-defining") {
    m_curr_bhv_spec.addBehaviorConfig(line, line_num);
    return(true);
  }

  if(m_parse_mode == "set-defining")  {
    string a_condition_string = line;
    bool ok = m_mode_entry.addCondition(a_condition_string);
    return(ok);
  }

  return(false);
}

//----------------------------------------------------------
// Procedure: closeSetMode

void Populator_BehaviorSet::closeSetMode()
{
  m_mode_set.addEntry(m_mode_entry);
  m_mode_entry.clear();
}

