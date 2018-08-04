#pragma once

#include <map>
#include <vector>
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


    void LockWasCanceled( JunctionId_t junc, VehicleId_t veh );
    void MoveThroughWasCanceled( JunctionId_t junc, VehicleId_t veh );

private:
    struct SSegment
    {
        Distance_t begin;
        JunctionId_t junc;
        PathId_t junc_other_path;
        bool junc_hi_priority;

        explicit SSegment( Distance_t begin_,
                            JunctionId_t junc_ = s_invalid_junc_id, PathId_t junc_other_path_ = s_invalid_path_id,
                            bool junc_hi_priority_ = true )
            :   begin(begin_),
                junc(junc_),
                junc_other_path(junc_other_path_),
                junc_hi_priority(junc_hi_priority_)
        {
        }
    };

    std::vector<SSegment> m_segments;
    std::map<VehicleId_t, size_t> m_veh_segments;

    size_t JunctionIdToSegmentIndex( JunctionId_t junc ) const;

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