#include <CPath.h>
#include <map>
#include <set>

CPath::SJunctionData::SJunctionData( JunctionId_t junc_, PathId_t other_path_, 
                                        Distance_t pos_begin_, Distance_t pos_end_, bool high_priority_ )
    :   junc(junc_),
        other_path(other_path_),
        pos_begin(pos_begin_),
        pos_end(pos_end_),
        high_priority(high_priority_)
{

}

CPath::CPath( WayPointId_t begin_wp_, WayPointId_t end_wp_, Distance_t distance_ )
    :   begin_wp(begin_wp_),
        end_wp(end_wp_),
        distance(distance_)
{
    assert( EndWayPoint()->path_in == s_invalid_path_id );
    assert( BeginWayPoint()->pathes_out.count( PathId() ) == 0 );
    EndWayPoint()->path_in = PathId();
    BeginWayPoint()->pathes_out.insert( PathId() );
}

CPath::~CPath()
{
    assert( EndWayPoint()->path_in == PathId() );
    assert( BeginWayPoint()->pathes_out.count( PathId() == 1 ) );
    EndWayPoint()->path_in = s_invalid_path_id;
    BeginWayPoint()->pathes_out.erase( PathId() );

    assert( juncs.empty() );
}

PathId_t CPath::PathId() const
{
    return ToPathId(this);
}

CWayPoint* CPath::BeginWayPoint() const
{
    return ToWayPoint(begin_wp);
}

CWayPoint* CPath::EndWayPoint() const
{
    return ToWayPoint(end_wp);
}

void CPath::AddJunctionData( const SJunctionData& junc_data )
{
    assert( junc_data.pos_begin >= 0 && junc_data.pos_begin <= junc_data.pos_end && junc_data.pos_end < this->distance );
    auto insert_it = std::upper_bound( juncs.begin(), juncs.end(), junc_data, 
        []( const SJunctionData& v1, const SJunctionData& v2 ) { return v1.pos_begin < v2.pos_begin; } );
        juncs.insert( insert_it, junc_data );
}

void CPath::RemoveJunctionData( JunctionId_t junc )
{
    auto erase_it = std::find_if( juncs.begin(), juncs.end(),
        [junc]( const SJunctionData& v ) { return v.junc == junc; } );
    assert( erase_it != juncs.end() );
    juncs.erase(erase_it);
}

bool CalculateRoute( WayPointId_t start_wp, WayPointId_t end_wp, std::vector<PathId_t>& pathes )
{
    assert( pathes.empty() );

    struct SPointData
    {
        long long total_distance;
        PathId_t path;

        SPointData() : total_distance(0), path(s_invalid_path_id) {}

        SPointData( long long total_distance_, PathId_t path_ )
            :   total_distance(total_distance_),
                path(path_)
        {

        }
    };

    typedef std::map<WayPointId_t, SPointData> CPointMap;
    CPointMap points;
    std::set<WayPointId_t> current_points;
    
    points[start_wp] = SPointData( 0LL, s_invalid_path_id );
    current_points.insert(start_wp);

    long long best_distance = std::numeric_limits<long long>::max();

    while( !current_points.empty() )
    {
        std::set<WayPointId_t> new_points;

        for( auto wp_id: current_points )
        {
            assert( points.count( wp_id ) == 1 );
            Distance_t current_distance = points[wp_id].total_distance;
            
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

                WayPointId_t out_wp_id = path->end_wp;
                
                auto point_data_it = points.find(out_wp_id);
                if( point_data_it == points.end() )
                {
                    point_data_it = points.insert( CPointMap::value_type( out_wp_id, SPointData( total_distance, path_id ) ) ).first;
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
                    best_distance = total_distance;
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
