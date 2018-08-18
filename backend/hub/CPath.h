#pragma once

#include <map>
#include <vector>
#include <list>
#include <Defs.h>
#include <CWayPoint.h>

typedef intptr_t LockId_t;
extern LockId_t s_invalid_lock_id;

class CPath
{
public:
    WayPointId_t begin_wp;
    WayPointId_t end_wp;

    Distance_t distance;

    typedef std::list<std::pair<Distance_t, VehicleId_t>> CVehicleList;
    CVehicleList vehicles;

    CVehicleList::iterator RegisterVehicle( VehicleId_t veh, Distance_t pos )
    {
        assert( pos >= 0 && pos < distance );
        CVehicleList::iterator it;
        for( auto it = vehicles.begin(); it != vehicles.end(); ++it )
        {
            if( pos < it->first )
            {
                break;
            }
        }

        return vehicles.insert( it, CVehicleList::value_type( pos, veh ) );
    }

    void MoveVehicle( CVehicleList::iterator veh_it, Distance_t new_pos )
    {
        veh_it->first = new_pos;

        while( veh_it != vehicles.begin() )
        {
            auto prev_it = std::prev(veh_it);
            if( veh_it->first > prev_it->first )
            {
                break;
            }

            vehicles.splice( prev_it, vehicles, veh_it );
        }

        while(true)
        {
            auto next_it = std::next(veh_it);
            if( next_it == vehicles.end() || next_it->first > veh_it->first )
            {
                break;
            }

            vehicles.splice( next_it, vehicles, veh_it );
        }
    }

    VehicleId_t GetVehicleAhead( CVehicleList::const_iterator veh_it ) const
    {
        auto next_it = std::next(veh_it);
        return ( next_it == vehicles.end() ) ? s_invalid_veh_id : next_it->second;
    }

    VehicleId_t GetLastVehicle() const
    {
        return vehicles.empty() ? s_invalid_veh_id : vehicles.front().second;
    }

    void RemoveVehicle( CVehicleList::iterator veh_it )
    {
        vehicles.erase(veh_it);
    }

    CPath( WayPointId_t begin_wp_, WayPointId_t end_wp_, Distance_t distance_ );

    ~CPath();

    PathId_t PathId() const;
    
    CWayPoint* BeginWayPoint() const;
    CWayPoint* EndWayPoint() const;

    void AddJunction( JunctionId_t junc, Distance_t pos_begin, Distance_t pos_end );
    void RemoveJunction( JunctionId_t junc );

    size_t SegmentCount() const;
    Distance_t SegmentStart( size_t index ) const;
    size_t PosToSegmentIndex( Distance_t distance ) const;
    
    // reserve segment for possible route
    // min_time - lower time when veh would be able to reach segment with full accel
    LockId_t LockSegment( size_t segment_index, VehicleId_t veh, HRTime_t min_time );

    void UpdateLock( LockId_t lock, HRTime_t min_time );

    void UnlockSegment( LockId_t lock );

    bool IsStopAvailable( size_t segment_index ) const;
    bool IsOutrunAvailable( size_t segment_index, HRTime_t leave_before_time ) const;

private:
    struct SLock
    {
        size_t segment_index;
        VehicleId_t veh;
        HRTime_t min_time;

        explicit SLock( size_t segment_index_, VehicleId_t veh_, HRTime_t min_time_ )
            :   segment_index(segment_index_),
                veh(veh_),
                min_time(min_time_)
        {    
        }
    };

    struct SSegment
    {
        Distance_t begin;
        Distance_t end;
        std::set<JunctionId_t> juncs;
        std::set<LockId_t> locks;

        explicit SSegment( Distance_t begin_, Distance_t end_ )
            :   begin(begin_),
                end(end_)
        {
        }
    };

    CPath( const CPath& ) = delete;
    CPath& operator=( const CPath& ) = delete;

    void UpdateSegmentLock( size_t segment_index );

    std::vector<SSegment> m_segments;

    size_t PositionToSegmentIndex( Distance_t pos ) const;

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
