#pragma once

#include <map>
#include <vector>
#include <Defs.h>
#include <CWayPoint.h>

class CPath
{
public:
    WayPointId_t begin_wp;
    WayPointId_t end_wp;

    Distance_t distance;

    struct SJunctionData
    {
        JunctionId_t junc;
        PathId_t other_path;
        Distance_t pos_begin;
        Distance_t pos_end;
        bool high_priority;

        SJunctionData( JunctionId_t junc_, PathId_t other_path_, 
                        Distance_t pos_begin_, Distance_t pos_end_, bool high_priority_ );
    };

    std::vector<SJunctionData> juncs;

    CPath( WayPointId_t begin_wp_, WayPointId_t end_wp_, Distance_t distance_ );

    ~CPath();

    PathId_t PathId() const;
    
    CWayPoint* BeginWayPoint() const;
    CWayPoint* EndWayPoint() const;

    void AddJunctionData( const SJunctionData& junc_data );
    void RemoveJunctionData( JunctionId_t junc );
};

inline PathId_t ToPathId( const CPath* path_ctx )
{
    return (PathId_t)path_ctx;
}

inline CPath* ToPath( PathId_t path_id )
{
    return (CPath*)path_id;
}

bool CalculateRoute( WayPointId_t start_wp, WayPointId_t end_wp, std::vector<PathId_t>& pathes );