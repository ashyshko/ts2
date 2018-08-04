#include <CPath.h>
#include <map>
#include <set>
#include <CJunction.h>
#include <CVehicle.h>

LockId_t s_invalid_lock_id = (LockId_t)0;

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
    assert( EndWayPoint()->pathes_in.count( PathId() ) == 0 );
    assert( BeginWayPoint()->pathes_out.count( PathId() ) == 0 );
    EndWayPoint()->pathes_in.insert( PathId() );
    BeginWayPoint()->pathes_out.insert( PathId() );
}

CPath::~CPath()
{
    assert( EndWayPoint()->pathes_in.count( PathId() ) == 1 );
    assert( BeginWayPoint()->pathes_out.count( PathId() == 1 ) );
    EndWayPoint()->pathes_in.erase( PathId() );
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

size_t CPath::SegmentCount() const
{
    return m_segments.size();
}

Distance_t CPath::SegmentStart( size_t index ) const
{
    assert( index < m_segments.size() );
    return m_segments[index].begin;

}

size_t CPath::PosToSegmentIndex( Distance_t distance ) const
{
    assert( distance >= 0 && distance < this->distance );

    for( size_t i = 1; i < m_segments.size(); ++i )
    {
        if( m_segments[i].begin > distance )
        {
            return i - 1;
        }
    }

    return m_segments.size() - 1;
}

bool CPath::LockSegment( size_t segment_index, VehicleId_t veh, HRTime_t& min_time, HRTime_t max_time )
{
    assert( max_time <= min_time );
    assert( segment_index < m_segments.size() );
    SSegment& segment = m_segments[segment_index];

    if( segment.junc == s_invalid_junc_id )
    {
        return true;
    }

    if( !segment.junc_hi_priority )
    {
        return false;
    }

    CJunction* junc = ToJunction(segment.junc);
    assert( junc->locks.count(veh) == 0 );

    junc->locks[veh] = CJunction::SLock( PathId(), s_hr_time_max, false );
    UpdateLock( veh, segment_index, min_time, max_time );
    return true;
}

void CPath::UpdateLock( VehicleId_t veh, size_t segment_index, HRTime_t& min_time, HRTime_t max_time )
{
    assert( max_time <= min_time );
    assert( segment_index < m_segments.size() );
    SSegment& segment = m_segments[segment_index];

    if( segment.junc == s_invalid_junc_id )
    {
        return;
    }

    assert( segment.junc != s_invalid_junc_id );
    assert( segment.junc_hi_priority );
    CJunction* junc = ToJunction(segment.junc);
    auto lock_it = junc->locks.find(veh);
    assert( lock_it != junc->locks.end() );
    CJunction::SLock& lock = lock_it->second;
    assert( lock.sender == PathId() );
    
    for( auto it = junc->requests.begin(); it != junc->requests.end(); )
    {
        const CJunction::SRequest& request = it->second;
        if( request.time < min_time || request.force )
        {
            ++it;
            continue;
        }
        
        ToPath( request.sender )->MoveThroughWasCanceled( segment.junc, it->first );
        it = junc->requests.erase(it);
    }

    junc->UpdateNextRequestTime();

    if( junc->next_request_time <= max_time )
    {
        min_time = std::max( min_time, junc->next_request_time );
    }
    else
    {
        min_time = max_time;
    }

    lock.time = min_time;
    lock.force = min_time >= max_time;
    junc->UpdateNextLockTime();
}

void CPath::UnlockSegment( VehicleId_t veh, size_t segment_index )
{
    assert( segment_index < m_segments.size() );
    SSegment& segment = m_segments[segment_index];

    if( segment.junc == s_invalid_junc_id )
    {
        return;
    }

    assert( !segment.junc_hi_priority );

    CJunction* junc = ToJunction(segment.junc);
    assert( junc->locks.count(veh) == 0 );
    junc->locks.erase(veh);
    junc->UpdateNextLockTime();
}

bool CPath::RequestMoveThrough( size_t segment_index, VehicleId_t veh, HRTime_t before_time, bool force )
{
    assert( segment_index < m_segments.size() );
    SSegment& segment = m_segments[segment_index];
    assert( segment.junc != s_invalid_junc_id );
    assert( !segment.junc_hi_priority );
    CJunction* junc = ToJunction(segment.junc);
    assert( junc->requests.count(veh) == 0 );

    junc->requests[veh] = CJunction::SRequest( PathId(), s_hr_time_min, false );
    return UpdateMoveThrough( veh, segment_index, before_time, force );
}

bool CPath::UpdateMoveThrough( VehicleId_t veh, size_t segment_index, HRTime_t before_time, bool force )
{
    assert( segment_index < m_segments.size() );
    SSegment& segment = m_segments[segment_index];
    assert( segment.junc != s_invalid_junc_id );
    assert( !segment.junc_hi_priority );
    CJunction* junc = ToJunction(segment.junc);
    auto request_it = junc->requests.find(veh);
    assert( request_it != junc->requests.end() );
    CJunction::SRequest& request = request_it->second;
    
    if( before_time > junc->next_lock_time )
    {
        if( !force )
        {
            junc->requests.erase(request_it);
            junc->UpdateNextRequestTime();
            return false;
        }

        for( auto it = junc->locks.begin(); it != junc->locks.end(); )
        {
            const CJunction::SLock& lock = it->second;
            if( lock.time >= before_time || lock.force )
            {
                ++it;
                continue;
            }

            ToPath( lock.sender )->LockWasCanceled( segment.junc, it->first );
            it = junc->locks.erase(it);
        }

        junc->UpdateNextLockTime();
    }

    request.time = before_time;
    request.force = force;
    junc->UpdateNextRequestTime();
    return true;
}

void CPath::CancelMoveThrough( VehicleId_t veh, size_t segment_index )
{
    assert( segment_index < m_segments.size() );
    SSegment& segment = m_segments[segment_index];
    assert( segment.junc != s_invalid_junc_id );
    assert( !segment.junc_hi_priority );

    CJunction* junc = ToJunction(segment.junc);
    assert( junc->requests.count(veh) == 1 );
    junc->requests.erase(veh);
    junc->UpdateNextLockTime();
}

void CPath::LockWasCanceled( JunctionId_t junc, VehicleId_t veh )
{
    assert( junc != s_invalid_junc_id );
    size_t segment_index = JunctionIdToSegmentIndex(junc);
    ToVehicle(veh)->LockWasCanceled( PathId(), segment_index );
}

void CPath::MoveThroughWasCanceled( JunctionId_t junc, VehicleId_t veh )
{
    assert( junc != s_invalid_junc_id );
    size_t segment_index = JunctionIdToSegmentIndex(junc);
    ToVehicle(veh)->MoveThroughWasCanceled( PathId(), segment_index );
}

size_t CPath::JunctionIdToSegmentIndex( JunctionId_t junc ) const
{
    for( size_t i = 0; i < m_segments.size(); ++i )
    {
        if( m_segments[i].junc == junc )
        {
            return i;
        }
    }

    assert(0);
    return m_segments.size();
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
