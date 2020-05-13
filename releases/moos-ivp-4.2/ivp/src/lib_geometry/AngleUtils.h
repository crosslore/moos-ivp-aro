/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: AngleUtils.h                                         */
/*    DATE: Nov 26, 2000                                         */
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
 
#ifndef ANGLEUTILS_HEADER
#define ANGLEUTILS_HEADER

#include "XYPoint.h"

double  relAng(double xa, double ya, double xb, double yb);
double  relAng(const XYPoint& a, const XYPoint& b);
double  radAngleWrap(double radians);
double  degToRadians(double degrees);
double  radToDegrees(double radians);
double  angle180(double degrees);
double  angle360(double degrees);
double  angleDiff(double, double);
// true heading/E-N conversion added by henrik
double  radToHeading(double radians);
double  headingToRadians(double degrees);

bool    containsAngle(double deg1, double deg2, double deg3);

#endif

