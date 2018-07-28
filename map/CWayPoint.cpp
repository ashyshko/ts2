#include <CWayPoint.h>

CWayPoint::CWayPoint() 
    :   path_in(s_invalid_path_id)
{
}

CWayPoint::~CWayPoint()
{
    assert( path_in == s_invalid_path_id );
    assert( pathes_out.empty() );
}

WayPointId_t CWayPoint::WayPointId() const
{
    return ToWayPointId(this);
}