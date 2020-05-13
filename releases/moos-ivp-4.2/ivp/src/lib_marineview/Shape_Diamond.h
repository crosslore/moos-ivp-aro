/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: Shape_Diamond.h                                      */
/*    DATE: July 6th 2008                                        */
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

#ifndef SHAPE_DIAMOND_HEADER
#define SHAPE_DIAMOND_HEADER

double g_diamondBody[]=
{
   0.0,    12.0,
  -8.0,     0.0,
   0.0,   -12.0,
   8.0,     0.0,
   0.0,    12.0
};

double       g_diamondCtrX     = 0.0;
double       g_diamondCtrY     = 0.0;
unsigned int g_diamondBodySize = 5;
double       g_diamondWidth    = 16;

#endif





