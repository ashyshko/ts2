#include <CWayPoint.h>

CWayPoint::CWayPoint()
{
}

CWayPoint::~CWayPoint()
{
    assert( pathes_in.empty() );
    assert( pathes_out.empty() );
}

WayPointId_t CWayPoint::WayPointId() const
{
    return ToWayPointId(this);
}