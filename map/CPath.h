#pragma once

#include <Defs.h>

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
        bool hi_priority;

        SJunctionData( JunctionId_t junc_, PathId_t other_path_, 
                        Distance_t pos_begin_, Distance_t pos_end_, bool high_priority )
            :   junc(junc_),
                other_path(other_path_),
                pos_begin(pos_begin_),
                pos_end(pos_end_),
                hi_priority(hi_priority_)
        {

        }
    };

    std::vector<SJunctionData> juncs;

    CPath( WayPointId_t begin_wp_, WayPointId_t end_wp_, Distance_t distance_ )
        :   from_wp(begin_wp_),
            to_wp(end_wp_),
            distance(distance_)
    {
        assert( EndWayPoint()->path_in == s_invalid_path_id );
        assert( BeginWayPoint()->pathes_out.count( PathId() ) == 0 );
        EndWayPoint()->path_in = PathId();
        BeginWayPoint()->pathes_out.insert( PathId() );
    }

    ~CPath()
    {
        assert( EndWayPoint()->path_in == PathId() );
        assert( BeginWayPoint()->pathes_out.count( PathId() == 1 ) );
        EndWayPoint()->path_in = s_invalid_path_id;
        BeginWayPoint()->pathes_out.erase( PathId() );

        assert( juncs.empty() );
    }

    PathId_t PathId() const
    {
        return ToPathId(this);
    }

    CWayPoint* BeginWayPoint() const
    {
        return ToWayPoint(begin_wp);
    }

    CWayPoint* EndWayPoint() const
    {
        return ToWayPoint(end_wp);
    }

    void AddJunctionData( const SJunctionData& junc_data )
    {
        assert( junc_data.begin_pos >= 0 && junc_data.begin_pos <= junc_data.end_pos && junc_data.end_pos < this->distance );
        auto insert_it = std::upper_bound( juncs.begin(), juncs.end(), junc_data, 
            []( const SJunctionData& v1, const SJunctionData& v2 ) { return v1.pos_begin < v2.pos_begin; } );
            juncs.insert( insert_it, junc_data );
    }

    void RemoveJunctionData( JunctionId_t junc )
    {
        auto erase_it = std::find_if( juncs.begin(), juncs.end(),
            [junc]( const SJunctionData& v ) { return v.junc == junc; } );
        assert( erase_it != juncs.end() );
        juncs.erase(erase_it);
    }
};

inline PathId_t ToPathId( const CPath* path_ctx )
{
    return (PathId_t)path_ctx;
}

inline CPath* ToPath( PathId_t path_id )
{
    return (CPath*)path_id;
}

inline bool CalculateRoute( WayPointId_t start_wp, WayPointId_t end_wp, std::vector<PathId_t>& pathes )
{
    assert( pathes.empty() );

    struct SPointData
    {
        long long total_distance;
        PathId_t path;

        SPointData( long long total_distance_, PathId_t path_ )
            :   total_disance(total_disance_),
                path(path_)
        {

        }
    };

    typedef std::map<WayPointId_t, SPointData> CPointMap;
    CPointMap points;
    std::set<CPointMap::iterator> current_points;

    current_points.insert( points.insert( std::make_pair( start_wp, 0 ) ).first );

    long long best_distance = std::numeric_limits<long long>::max();

    while( !current_points.empty() )
    {
        std::set<CPointMap::iterator> new_points;

        for( auto current_point_it: current_points )
        {
            WayPointId_t wp_id = current_point_it->first;
            DistanceId_t current_distance = current_point_it->second.total_disance;
            
            if( current_distance >= best_distance )
            {
                continue;
            }
            
            CWayPoint* wp = ToWayPoint(wp_id);
            const std::set<PathId_t>& pathes_out = wp->pathes_out;

            for( const PathId_t path_id: pathes_out )
            {
                CPath* path = ToPath(path_id);
                long long total_distance = current_distance + path->distance;

                if( total_distance >= best_distance )
                {
                    continue;
                }

                WayPointId_t out_wp_id = path->to;
                
                auto point_data_it = points.find(out_wp_id);
                if( point_data_it == points.end() )
                {
                    points.insert( SPointData( total_distance, path_id ) );
                }
                else
                {
                    SPointData& point_data = point_data_it->second;
                    
                    if( point_data.total_distance <= total_distance )
                    {
                        continue;
                    }

                    point_data.total_distance = total_distance;
                    point_data.path = path_id;
                }

                new_points.insert(out_wp_id);
                if( out_wp_id == end_wp )
                {
                    best_distance = total_disance;
                }
            }
        }

        new_points.swap(current_points);
    }

    WayPointId_t reverse_wp = end_wp;
    while( reverse_wp != start_wp )
    {
        auto point_it = points.find(reverse_wp);
        
        if( point_it == points.end() )
        {
            assert( reverse_wp == end_wp );
            return false;
        }

        PathId_t path_id = point_it->second.path;
        CPath* path = ToPath(path_id);
        pathes.push_back(path_id);
        reverse_wp = path->begin_wp;
    }

    std::reverse( pathes.begin(), pathes.end() );
    return true;
}
