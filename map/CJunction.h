#pragma once

#include <Defs.h>
#include <CPath.h>

class CJunction
{
public:
    PathId_t hi_path;
    PathId_t lo_path;

    explicit CJunction( PathId_t hi_path_, Distance_t hi_path_begin, Distance_t hi_path_end,
        PathId_t lo_path_, Distance_t lo_path_begin, Distance_t lo_path_end );
    ~CJunction();
    
    JunctionId_t JunctionId() const;

    CPath* HiPath() const;
    CPath* LoPath() const;

    void Lock( PathId_t path, HRTime_t time );
    void Unlock( PathId_t path );

    bool IsStopAvailable( PathId_t path ) const;
    bool IsOutrunAvailable( PathId_t path, HRTime_t leave_before_time ) const;

private:
    HRTime_t m_hi_lock_time;
};

inline JunctionId_t ToJunctionId( const CJunction* junction )
{
    return (JunctionId_t)junction;
}

inline CJunction* ToJunction( JunctionId_t junction_id )
{
    return (CJunction*)junction_id;
}
