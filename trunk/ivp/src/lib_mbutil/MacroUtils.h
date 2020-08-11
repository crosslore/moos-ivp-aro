/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: MacroUtils.h                                         */
/*    DATE: June 24th, 2020                                      */
/*                                                               */
/* This file is part of IvP Helm Core Libs                       */
/*                                                               */
/* IvP Helm Core Libs is free software: you can redistribute it  */
/* and/or modify it under the terms of the Lesser GNU General    */
/* Public License as published by the Free Software Foundation,  */
/* either version 3 of the License, or (at your option) any      */
/* later version.                                                */
/*                                                               */
/* IvP Helm Core Libs is distributed in the hope that it will    */
/* be useful but WITHOUT ANY WARRANTY; without even the implied  */
/* warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR       */
/* PURPOSE. See the Lesser GNU General Public License for more   */
/* details.                                                      */
/*                                                               */
/* You should have received a copy of the Lesser GNU General     */
/* Public License along with MOOS-IvP.  If not, see              */
/* <http://www.gnu.org/licenses/>.                               */
/*****************************************************************/
 
#ifndef MACRO_UTILS_HEADER
#define MACRO_UTILS_HEADER

#include <string>

std::string macroExpand(const std::string&, const std::string&, const std::string&);

std::string macroExpand(const std::string&, const std::string&, bool);

std::string macroExpand(const std::string&, const std::string&, double);

std::string macroExpand(const std::string&, const std::string&, int);

std::string macroExpand(const std::string&, const std::string&, unsigned int);

#endif
