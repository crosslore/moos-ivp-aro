/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: Populator_VPlugPlots.h                               */
/*    DATE: August 9th, 2009                                     */
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

#ifndef POPULATOR_VPLUG_PLOTS_HEADER
#define POPULATOR_VPLUG_PLOTS_HEADER

#include <vector>
#include "VPlugPlot.h"
#include "ALogEntry.h"

class Populator_VPlugPlots 
{
public:
  Populator_VPlugPlots() {};
  ~Populator_VPlugPlots() {};

  bool      populateFromEntries(const std::vector<ALogEntry>&);

  VPlugPlot getVPlugPlot() {return(m_vplug_plot);};

protected:
  VPlugPlot m_vplug_plot;
};
#endif 

