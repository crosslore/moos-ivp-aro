/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: Obstacle.cpp                                         */
/*    DATE: May 29th, 2020                                       */
/*****************************************************************/

#include "MBUtils.h"
#include "Obstacle.h"

using namespace std;

//---------------------------------------------------------
// Constructor

Obstacle::Obstacle()
{
  m_range      = 0;
  m_duration   = -1;
  m_tstamp     = 0;
  m_max_points = 20;

  m_pts_total  = 0;
  m_changed    = false;
  m_updates_total = 0;
  m_min_range  = -1;
}


//---------------------------------------------------------
// Procedure: addPoint()

bool Obstacle::addPoint(XYPoint point)
{
  m_points.push_front(point);

  if(m_points.size() > m_max_points)
    m_points.pop_back();

  // Obstacle is point-based. Must have non-empty set of points
  // or it will be considered expired.
  m_duration = 0;

  m_changed = true;
  m_pts_total++;
  return(true);
}
  
//---------------------------------------------------------
// Procedure: setPoly
//      Note: Sometimes the poly originates as a given poly,
//            and sometimes from a source that has generated
//            the poly from the obstacle points.

bool Obstacle::setPoly(XYPolygon poly)
{
  if(!poly.is_convex())
    return(false);

  m_polygon = poly;

  m_changed = true;
  m_updates_total++;
  return(true);
}

//---------------------------------------------------------
// Procedure: setRange()

void Obstacle::setRange(double dval)
{
  m_range = dval;
  if((m_min_range < 0) || (m_range < m_min_range))
    m_min_range = m_range;
}


//---------------------------------------------------------
// Procedure: getTimeToLive()
//      Note: -1 indicates this obstacle does not have a duration
//            Or it never had a timestamp applied
//      Note: 0 indicates that the time_to_live has been exhausted

double Obstacle::getTimeToLive(double curr_time) const
{
  if((m_duration <= 0) || (m_tstamp <= 0))
    return(-1);
  
  double age = curr_time - m_tstamp;
  double ttlive = m_duration - age;
  if(ttlive < 0)
    ttlive = 0;

  return(ttlive);
}


//---------------------------------------------------------
// Procedure: getPoints()

vector<XYPoint> Obstacle::getPoints() const
{
  vector<XYPoint> rvector;
  
  list<XYPoint>::const_iterator p;
  for(p=m_points.begin(); p!= m_points.end(); p++)
    rvector.push_back(*p);

  return(rvector);
}

//---------------------------------------------------------
// Procedure: isGiven()

bool Obstacle::isGiven() const
{
  if((m_points.size() == 0) && (m_polygon.size() > 0))
    return(true);

  return(false);
}


  
//---------------------------------------------------------
// Procedure: pruneByAge()
//      Note: For point-based obstacles, this procedure will
//            remove old points and mark it as removable if
//            there are no longer any points.
//      Note: For given obstacles, it will check if a duration
//            is provided, and if so, mark it as removable if
//            there hasn't been any recent updates.
//   Returns: true if this obstacle is empty/removable.

bool Obstacle::pruneByAge(double max_age, double curr_time)
{
  // Part 1: If obstacle is given (not point-based) then check
  // if its last update is older than the declared duration
  // A duration of -1 means the duration is unlimited.
  if(m_points.size() == 0) {
    if(m_duration >= 0) {
      if((curr_time - m_tstamp) > m_duration)
	return(true);
    }
    return(false);
  }
  
  // Part 2: Go through all points and prune based on age
  list<XYPoint>::iterator p;
  for(p=m_points.begin(); p!=m_points.end(); ) {
    XYPoint pt = *p;
    double age = curr_time - pt.get_time();
    if(age > max_age) {
      m_changed = true;
      p = m_points.erase(p);
    }
    else
      ++p;
  }
  // If points are empty, return that it is removable
  if(m_points.size() == 0)
    return(true);

  return(false);
}
