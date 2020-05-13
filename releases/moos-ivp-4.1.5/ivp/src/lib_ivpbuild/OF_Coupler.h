/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: OF_Coupler.h                                         */
/*    DATE: Jul 1st 2006                                         */
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

#ifdef _WIN32
#pragma warning(disable : 4786)
#pragma warning(disable : 4503)
#endif
#ifndef OF_COUPLER_HEADER
#define OF_COUPLER_HEADER

class IvPFunction;

class OF_Coupler {
public:
  OF_Coupler();
  virtual ~OF_Coupler() {};

  void disableNormalize();
  void enableNormalize(double minwt=0, double maxwt=100);
  
  IvPFunction *couple(IvPFunction* ipf_one, IvPFunction* ipf_two);
  IvPFunction *couple(IvPFunction* ipf_one, IvPFunction* ipf_two, 
		      double pwt_one, double pwt_two);

 protected:
  IvPFunction *coupleRaw(IvPFunction*, IvPFunction*);

 protected:
  bool   m_normalize;
  double m_normalmin;
  double m_normalmax;
};
#endif











