/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: XMS_Info.cpp                                         */
/*    DATE: Dec 14th 2011                                        */
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
#include <iostream>
#include "ColorParse.h"
#include "ReleaseInfo.h"
#include "XMS_Info.h"

using namespace std;

//----------------------------------------------------------------
// Procedure: showSynopsis

void showSynopsis()
{
  blk("SYNOPSIS:                                                       ");
  blk("------------------------------------                            ");
  blk("  uXMS is a terminal-based (non GUI) tool for scoping a MOOSDB  ");
  blk("  Users may precisely configure the set of variables they wish  ");
  blk("  to scope on by naming them explicitly on the command line or  ");
  blk("  in the MOOS configuration block. The variable set may also be ");
  blk("  configured by naming one or more MOOS proceses on which all   ");
  blk("  variables published by those processes will be scoped.        ");
  blk("  Users may also scope on the *history* of a single variable.   ");
}

//----------------------------------------------------------------
// Procedure: showHelpAndExit

void showHelpAndExit()
{
  blk("                                                                ");
  blu("=============================================================== ");
  blu("Usage: uXMS file.moos [OPTIONS]                                 ");
  blu("=============================================================== ");
  blk("                                                                ");
  showSynopsis();
  blk("                                                                ");
  blk("Options:                                                        ");
  mag("  --alias","=<ProcessName>                                      ");
  blk("      Launch uXMS with the given process name rather than uXMS. ");
  mag("  --all,-a                                                      ");
  blk("    Show ALL MOOS variables in the MOOSDB                       ");
  mag("  --clean,-c                                                    ");
  blk("    Ignore scope variables in file.moos                         ");
  mag("  --colormap","=VAR,color                                       ");
  blk("    Display all entries where the variable, source, or community");
  blk("    has VAR as substring. Allowable olors: blue, red, magenta,  ");
  blk("    cyan, or green.                                             ");
  mag("  --colorany","=VAR,VAR,...,VAR                                 ");
  blk("    Display all entries where the variable, community, or source");
  blk("    has VAR as substring. Color auto-chosen from unused colors. ");
  mag("  --example, -e                                                 ");
  blk("      Display example MOOS configuration block.                 ");
  mag("  --help, -h                                                    ");
  blk("      Display this help message.                                ");
  mag("  --history=variable                                            ");
  blk("    Allow history-scoping on variable                           ");
  mag("  --interface, -i                                               ");
  blk("      Display MOOS publications and subscriptions.              ");
  mag("  --mask","=[virgin,empty]                                      ");
  blk("    virgin: Don't display virgin variables                      ");
  blk("    empty:  Don't display empty strings                         ");
  mag("  --mode","=[paused,events,STREAMING]                           ");
  blk("    Determine display mode. Paused: scope updated only on user  ");
  blk("    request. Events: data updated only on change to a scoped    "); 
  blk("    variable. Streaming: updates continuously on each app-tick. ");
  mag("  --server_host","=value                                        ");
  blk("    Connect to MOOSDB at IP=value, not from the .moos file.     ");
  mag("  --server_port","=value                                        "); 
  blk("    Connect to MOOSDB at port=value, not from the .moos file.   ");
  mag("  --show","=[source,time,community,aux]                         ");
  blk("    Turn on data display in the named column, source, time, or  ");
  blk("    community. All off by default enabling aux shows the        ");
  blk("    auxilliary source in the souce column.                      ");
  mag("  --src","=pSrc,pSrc,...,pSrc                                   ");
  blk("    Scope only on vars posted by the given MOOS processes       ");
  mag("  --trunc","=value [10,1000]                                    ");
  blk("    Truncate the output in the data column.                     ");
  mag("  --termint","=value [0,10]  (default is 0.6)                   ");
  blk("    Minimum real-time seconds between terminal reports.         ");
  mag("  -t                                                            ");
  blk("    Short for --trunc=45                                        ");
  mag("  --version,-v                                                  ");
  blk("    Display the release version of uXMS.                        ");
  blk("                                                                ");
  exit(0);
}

//----------------------------------------------------------------
// Procedure: showExampleConfigAndExit

void showExampleConfigAndExit()
{
  blk("                                                                ");
  blu("=============================================================== ");
  blu("uXMS Example MOOS Configuration                                 ");
  blu("=============================================================== ");
  blk("                                                                ");
  blk("ProcessConfig = uXMS                                            ");
  blk("{                                                               ");
  blu("  AppTick   = 4                                                 ");
  blu("  CommsTick = 4                                                 ");
  blk("                                                                ");
  blk("  VAR = NAV_X, NAV_Y, NAV_SPEED, NAV_HEADING                    ");
  blk("  VAR = PROC_WATCH_SUMMARY                                      ");
  blk("  VAR = PROC_WATCH_EVENT                                        ");
  blk("                                                                ");
  blk("  HISTORY_VAR = DB_CLIENTS                                      ");
  blu("  DISPLAY_VIRGINS = true          // default                    ");
  blk("  DISPLAY_EMPTY_STRINGS = true    // default                    ");
  blk("  DISPLAY_SOURCE        = false   // default                    ");
  blk("  DISPLAY_TIME          = false   // default                    ");
  blk("  DISPLAY_COMMUNITY     = false   // default                    ");
  blk("  DISPLAY_ALL           = false   // default                    ");
  blk("                                                                ");
  blk("  TERM_REPORT_INTERVAL  = 0.6     // default (seconds           ");
  blk("                                                                ");
  blk("  COLOR_MAP    = pHelmIvP, red   // All postings by pHelmIvP red");
  blk("  COLOR_MAP    = NAV_SPEED, blue // Only var NAV_SPEED is blue  ");
  blk("                                                                ");
  blk("  REFRESH_MODE = streaming       // default (or events/paused) ");
  blk("  CONTENT_MODE = scoping         // default (or history)       ");
  blk("}                                                               ");
  blk("                                                                ");
  exit(0);
}


//----------------------------------------------------------------
// Procedure: showInterfaceAndExit

void showInterfaceAndExit()
{
  blk("                                                                ");
  blu("=============================================================== ");
  blu("uXMS INTERFACE                                                  ");
  blu("=============================================================== ");
  blk("                                                                ");
  showSynopsis();
  blk("                                                                ");
  blk("SUBSCRIPTIONS:                                                  ");
  blk("------------------------------------                            ");
  blk("  Subscribes only for variables requested to scope.             ");
  blk("                                                                ");
  blk("                                                                ");
  blk("PUBLICATIONS:                                                   ");
  blk("------------------------------------                            ");
  blk("  None.                                                         ");
  blk("                                                                ");
  exit(0);
}

//----------------------------------------------------------------
// Procedure: showReleaseInfoAndExit

void showReleaseInfoAndExit()
{
  showReleaseInfo("uXMS", "gpl");
  exit(0);
}

