#pragma once

#include <set>
#include <Defs.h>

class CWayPoint
{
public:
    PathId_t path_in;
    std::set<PathId_t> pathes_out;

    CWayPoint() 
        :   path_in(s_invalid_path_id)
    {
    }
    
    ~CWayPoint()
    {
        assert( path_in == s_invalid_path_id );
        assert( pathes_out.empty() );
    }

    WayPointId_t WayPointId() const
    {
        return ToWayPointId(this);
    }
};

inline WayPointId_t ToWayPointId( const CWayPoint* wp )
{
    return (WayPointId_t)wp;
}

inline CWayPoint* ToWayPoint( WayPointId_t wp_id )
{
    return (CWayPoint*)wp_id;
}