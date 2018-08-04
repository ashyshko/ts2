#include <CPath.h>
#include <map>
#include <set>
#include <CJunction.h>
#include <CVehicle.h>

LockId_t s_invalid_lock_id = (LockId_t)0;

CPath::CPath( WayPointId_t begin_wp_, WayPointId_t end_wp_, Distance_t distance_ )
    :   begin_wp(begin_wp_),
        end_wp(end_wp_),
        distance(distance_)
{
    assert( EndWayPoint()->pathes_in.count( PathId() ) == 0 );
    assert( BeginWayPoint()->pathes_out.count( PathId() ) == 0 );
    EndWayPoint()->pathes_in.insert( PathId() );
    BeginWayPoint()->pathes_out.insert( PathId() );

    m_segments.push_back( SSegment( 0, distance ) );
}

CPath::~CPath()
{
    assert( EndWayPoint()->pathes_in.count( PathId() ) == 1 );
    assert( BeginWayPoint()->pathes_out.count( PathId() == 1 ) );
    EndWayPoint()->pathes_in.erase( PathId() );
    BeginWayPoint()->pathes_out.erase( PathId() );

    assert( m_segments.size() == 1 );
    assert( m_segments[0].juncs.empty() );
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

void CPath::AddJunction( JunctionId_t junc, Distance_t pos_begin, Distance_t pos_end )
{
    assert( pos_begin <= pos_end );
    assert( pos_begin >= 0 );
    assert( pos_end <= distance );

    size_t segment_index = PosToSegmentIndex(pos_begin);
    size_t rebuild_from_segment_index = segment_index;
    
    assert( pos_begin >= m_segments[segment_index].begin && pos_begin < m_segments[segment_index].end );

    if( pos_begin > m_segments[segment_index].begin )
    {
        m_segments.insert( m_segments.begin() + segment_index + 1, SSegment( pos_begin, m_segments[segment_index].end ) );
        m_segments[segment_index].end = pos_begin;
        ++segment_index;
        m_segments[segment_index].juncs = m_segments[segment_index-1].juncs;
        m_segments[segment_index].juncs.insert(junc);
    }

    while( m_segments[segment_index].end < pos_end )
    {
        m_segments[segment_index].juncs.insert(junc);
        ++segment_index;
        assert( segment_index < m_segments.size() );
    }

    if( m_segments[segment_index].end > pos_end )
    {
        m_segments.insert( m_segments.begin() + segment_index + 1, SSegment( pos_end, m_segments[segment_index].end ) );
        m_segments[segment_index+1].juncs = m_segments[segment_index].juncs;
        m_segments[segment_index].juncs.insert(junc);
        m_segments[segment_index].end = pos_end;
        ++segment_index;
    }

    // TODO notify about segments changed!
}

void CPath::RemoveJunction( JunctionId_t junc )
{
    size_t rebuild_from_segment_index = std::numeric_limits<size_t>::max();

    for( size_t segment_index = 0; segment_index < m_segments.size(); ++segment_index )
    {
        if( m_segments[segment_index].juncs.erase(junc) > 0 )
        {
            rebuild_from_segment_index = std::min( rebuild_from_segment_index, segment_index );
        }
    }

    assert( rebuild_from_segment_index != std::numeric_limits<size_t>::max() );

    for( size_t segment_index = 0; segment_index + 1 < m_segments.size(); )
    {
        if( m_segments[segment_index].juncs == m_segments[segment_index+1].juncs )
        {
            m_segments[segment_index].end = m_segments[segment_index+1].end;
            m_segments.erase( m_segments.begin() + segment_index + 1 );
            rebuild_from_segment_index = std::min( rebuild_from_segment_index, segment_index );
        }
        else
        {
            ++segment_index;
        }
    }

    // TODO notify about segments changed!
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

LockId_t CPath::LockSegment( size_t segment_index, VehicleId_t veh, HRTime_t min_time )
{
    assert( segment_index < m_segments.size() );
    SSegment& segment = m_segments[segment_index];
    SLock* lock = new SLock( segment_index, veh, min_time );
    LockId_t lock_id = (LockId_t)lock;
    assert( segment.locks.count(lock_id) == 0 );
    segment.locks.insert(lock_id);
    UpdateSegmentLock(segment_index);
    return lock_id;
}

void CPath::UpdateLock( LockId_t lock, HRTime_t min_time )
{
    SLock* lock_obj = (SLock*)lock;
    lock_obj->min_time = min_time;
    UpdateSegmentLock( lock_obj->segment_index );
}

void CPath::UnlockSegment( LockId_t lock )
{
    SLock* lock_obj = (SLock*)lock;
    assert( lock_obj->segment_index < m_segments.size() );
    SSegment& segment = m_segments[ lock_obj->segment_index ];
    assert( segment.locks.count(lock) == 1 );
    segment.locks.erase(lock);
    UpdateSegmentLock( lock_obj->segment_index );
    delete lock_obj;
}

bool CPath::IsStopAvailable( size_t segment_index ) const
{
    assert( segment_index < m_segments.size() );
    const SSegment& segment = m_segments[segment_index];

    for( auto junc: m_segments[segment_index].juncs )
    {
        if( !ToJunction(junc)->IsStopAvailable( PathId() ) )
        {
            return false;
        }
    }

    return true;
}

bool CPath::IsOutrunAvailable( size_t segment_index, HRTime_t leave_before_time ) const
{
    assert( segment_index < m_segments.size() );
    const SSegment& segment = m_segments[segment_index];
    
    for( auto junc: m_segments[segment_index].juncs )
    {
        if( !ToJunction(junc)->IsOutrunAvailable( PathId(), leave_before_time ) )
        {
            return false;
        }
    }

    return true;
}

void CPath::UpdateSegmentLock( size_t segment_index )
{
    assert( segment_index < m_segments.size() );
    SSegment& segment = m_segments[segment_index];

    if( segment.juncs.empty() )
    {
        return;
    }

    if( segment.locks.empty() )
    {
        for( JunctionId_t junc : segment.juncs )
        {
            ToJunction(junc)->Unlock( PathId() );
        }

        return;
    }

    HRTime_t min_time = s_hr_time_max;
    for( LockId_t lock: segment.locks )
    {
        min_time = std::min( min_time, ((SLock*)lock)->min_time );
    }

    for( JunctionId_t junc : segment.juncs )
    {
        ToJunction( junc )->Lock( PathId(), min_time );
    }
}

size_t CPath::PositionToSegmentIndex( Distance_t pos ) const
{
    assert( pos >= 0 && pos < distance );
    assert( !m_segments.empty() );
    for( size_t i = 1; i < m_segments.size(); ++i )
    {
        if( m_segments[i].begin > pos )
        {
            return i-1;
        }
    }

    return m_segments.size() - 1;
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
