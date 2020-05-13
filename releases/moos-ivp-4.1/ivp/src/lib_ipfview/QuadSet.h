/****************************************************************/
/*    NAME: Mike Benjamin                                       */
/*    FILE: QuadSet.h                                           */
/*    DATE: July 4th 2006                                       */
/****************************************************************/

#ifndef QUAD_SET_HEADER
#define QUAD_SET_HEADER

#include <vector>
#include <string>
#include "Quad3D.h"
#include "IvPFunction.h"
#include "FColorMap.h"

class QuadSet 
{
public:
  QuadSet();
  ~QuadSet() {};

  void   applyIPF(IvPFunction *ipf, std::string, std::string);
  void   applyColorMap(const FColorMap&, double=0, double=0);
  bool   addQuadSet(const QuadSet*);
  void   normalize(double, double);
  void   setAdjust(double, double);
  void   print() const;
  void   clear();

  int    size() const         {return(m_quads.size());};
  Quad3D getQuad(int i) const {return(m_quads[i]);};

protected:
  std::vector<Quad3D> m_quads;

  double              m_low_val;
  double              m_high_val;

  // Adjusting the range of values - purely for altering the 
  // the way the color map is applied.
  double              m_low_val_adjust;;
  double              m_high_val_adjust;

  // Values of the objecive function can be made to snap to 
  // intervals. Can be used to alter the rendering.
  double              m_snap_val;
};
#endif
