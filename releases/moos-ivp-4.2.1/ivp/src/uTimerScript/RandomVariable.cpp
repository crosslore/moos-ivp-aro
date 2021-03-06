/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: RandomVariable.cpp                                   */
/*    DATE: Dec 18th 2009                                        */
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
#include "RandomVariable.h"
#include "MBUtils.h"

using namespace std;

//---------------------------------------------------------
// Constructor

RandomVariable::RandomVariable()
{
  // Initial values for state variables.
  m_value     = 0;
  m_timestamp = 0;

  // Initial values for configuration parameters
  m_varname = "random_var";
  m_keyname = "";
  m_min_val = 0;
  m_max_val = 1;
}


//---------------------------------------------------------
// Procedure: setRange

bool RandomVariable::setRange(double min, double max)
{
  if(min > max)
    return(false);
  
  m_min_val = min;
  m_max_val = max;
  
  reset();
  return(true);
}

//---------------------------------------------------------
// Procedure: reset

double RandomVariable::reset(double timestamp)
{
  if(m_timestamp >= 0)
    m_timestamp = timestamp;
  
  // Unlikely but check and handle special case anyway.
  if(m_min_val >= m_max_val) {
    m_value = m_min_val;
    return(m_value);
  }

  int    rand_int = rand() % 10000;
  double rand_pct = (double)(rand_int) / 10000;
  m_value = m_min_val + ((m_max_val-m_min_val) * rand_pct);

  return(m_value);
}
  
//---------------------------------------------------------
// Procedure: getAge

double RandomVariable::getAge(double timestamp)
{
  if((m_timestamp < 0) || (timestamp < m_timestamp))
    return(0);
  else
    return(timestamp - m_timestamp);
}
  
  
//---------------------------------------------------------
// Procedure: getStringValue

string RandomVariable::getStringValue() const
{
  return(doubleToStringX(m_value,0));
}
  
  
//---------------------------------------------------------
// Procedure: getStringSummary

string RandomVariable::getStringSummary()
{
  string str = "varname=" + m_varname;
  if(m_keyname != "")
    str += ", keyname=" + m_keyname;
  str += ", min=" + doubleToStringX(m_min_val);
  str += ", max=" + doubleToStringX(m_max_val);
  return(str);
}
  
  

