#pragma once

#include <Defs.h>
#include <CPath.h>

class CJunction
{
public:
    PathId_t hi_path;
    PathId_t lo_path;

    struct SLock
    {
        PathId_t sender;
        HRTime_t time;
        bool force;

        SLock() {}
        explicit SLock( PathId_t sender_, HRTime_t time_, bool force_ = false )
            :   sender(sender_),
                time(time_),
                force(force_)
        {
        }
    };

    HRTime_t next_lock_time;
    std::map<VehicleId_t, SLock> locks;

    struct SRequest
    {
        PathId_t sender;
        HRTime_t time;
        bool force;

        SRequest() {}
        explicit SRequest( PathId_t sender_, HRTime_t time_, bool force_ = false )
            :   sender(sender_),
                time(time_),
                force(force_)
        {
        }
    };

    HRTime_t next_request_time;
    std::map<VehicleId_t, SRequest> requests;

    explicit CJunction( PathId_t hi_path_, Distance_t hi_path_begin, Distance_t hi_path_end,
                        PathId_t lo_path_, Distance_t lo_path_begin, Distance_t lo_path_end );
    ~CJunction();
    
    JunctionId_t JunctionId() const;

    CPath* HiPath() const;
    CPath* LoPath() const;

    void UpdateNextLockTime();
    void UpdateNextRequestTime();
};

inline JunctionId_t ToJunctionId( const CJunction* junction )
{
    return (JunctionId_t)junction;
}

inline CJunction* ToJunction( JunctionId_t junction_id )
{
    return (CJunction*)junction_id;
}
