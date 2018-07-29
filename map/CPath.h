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

    size_t SegmentCount() const;
    Distance_t SegmentStart( size_t index ) const;
    size_t PosToSegmentIndex( Distance_t distance ) const;
    
    // reserve segment for possible route
    // min_time - lower time when veh would be able to reach segment with full accel
    // max_time - lower time when veh would be able to reach segment with full deccel 
    //            (possible s_time_max if veh is able to full stop before this segment)
    // returns true if lock is possible (i.e. this segment is not reserved for other veh on path with higher priority )
    //              and modifies min_time in range [min_time, max_time] if this segment was already requested (see RequestMoveThrough)
    // returns false if lock is not possible
    bool LockSegment( size_t segment_index, VehicleId_t veh, Time_t& min_time, Time_t max_time );

    void UpdateLock( VehicleId_t veh, size_t segment_index, Time_t& min_time, Time_t max_time );

    void UnlockSegment( VehicleId_t veh, size_t segment_index );

    // if LockSegment returns false, veh may request fast getting through that segment
    // before_time - minimal time when vehicle is able to leave this segment
    // force - if vehicle can't stop before that start of segment_index
    // returns true if fast getting through is available
    bool RequestMoveThrough( size_t segment_index, VehicleId_t veh, Time_t before_time, bool force );

    // MoveThrough request is automatically canceled if UpdateMoveThrough returns false
    bool UpdateMoveThrough( VehicleId_t veh, size_t segment_index, Time_t before_time, bool force );

    void CancelMoveThrough( VehicleId_t veh, size_t segment_index );

    void LockWasCanceled( JunctionId_t junc, VehicleId_t veh );
    void MoveThroughWasCanceled( JunctionId_t junc, VehicleId_t veh );

private:
    struct SSegment
    {
        DistanceId_t begin;
        JunctionId_t junc;
        PathId_t junc_other_path;
        bool junc_hi_priority;

        explicit SSegment( DistanceId_t begin_,
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