/*****************************************************************/
/*    NAME: M.Benjamin, H.Schmidt, J. Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: ZAIC_Vector.h                                        */
/*    DATE: Apr 30th, 2010                                       */
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
#ifndef IVPBUILD_ZAIC_VECTOR_HEADER
#define IVPBUILD_ZAIC_VECTOR_HEADER

#include <vector>
#include <string>
#include "IvPDomain.h"
#include "IvPFunction.h"

class PDMap;

class ZAIC_Vector {
public:
  ZAIC_Vector(IvPDomain g_domain, const std::string& g_varname);
  virtual ~ZAIC_Vector() {clearBldPieces();};

  void   setDomainVals(std::vector<double> v);
  void   setRangeVals(std::vector<double> v);
  double getParam(std::string);
  void   print();

  std::string  getErrors();
  bool         hasErrors();
  void         clearErrors();   

  std::string  getWarnings();
  bool         hasWarnings();
  void         clearWarnings();

  unsigned int size() {return(m_domain_pts);};

  IvPFunction* extractOF();
  IvPFunction* extractIvPFunction()  {return(extractOF());};
  IvPDomain    getIvPDomain()        {return(m_ivp_domain);};
  
 protected:
  bool   stateOK();
  void   convertValues();
  void   sortMapping();
  PDMap* setPDMap();
  void   clearBldPieces();

  void   addConfigWarning(std::string);
  void   addBuildWarning(std::string);
  void   addError(std::string);

  IvPBox* buildBox(unsigned int ix_low, double val_low,
		   unsigned int ix_hgh, double val_hgh);

protected:  // Parameters
  std::vector<double> m_domain_vals;
  std::vector<double> m_range_vals;

  double m_minutil;
  double m_maxutil;

protected: // State values

  std::vector<unsigned int> m_idomain_vals;
  std::vector<double>       m_irange_vals;

  std::vector<std::string>  m_errors;
  std::vector<std::string>  m_build_warnings;
  std::vector<std::string>  m_config_warnings;
  
  std::vector<IvPBox*> m_bld_pieces;

  unsigned int m_domain_pts;
  unsigned int m_domain_ix;
  double       m_domain_high;
  double       m_domain_low;
  double       m_domain_delta;

  bool         m_sort_needed;
  bool         m_domain_ok;
  IvPDomain    m_ivp_domain;
};
#endif
