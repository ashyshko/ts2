#include <CJunction.h>

CJunction::CJunction( PathId_t hi_path_, Distance_t hi_path_begin, Distance_t hi_path_end,
                        PathId_t lo_path_, Distance_t lo_path_begin, Distance_t lo_path_end )
    :   hi_path(hi_path_),
        lo_path(lo_path_),
        next_lock_time(s_hr_time_max),
        next_request_time(s_hr_time_min)
{
    HiPath()->AddJunctionData( CPath::SJunctionData( JunctionId(), lo_path, hi_path_begin, hi_path_end, true ) );
    LoPath()->AddJunctionData( CPath::SJunctionData( JunctionId(), hi_path, lo_path_begin, lo_path_end, false ) );
}

CJunction::~CJunction()
{
    HiPath()->RemoveJunctionData( JunctionId() );
    LoPath()->RemoveJunctionData( JunctionId() );
}

JunctionId_t CJunction::JunctionId() const
{
    return ToJunctionId(this);
}

CPath* CJunction::HiPath() const
{
    return ToPath(hi_path);
}

CPath* CJunction::LoPath() const
{
    return ToPath(lo_path);
}

void CJunction::UpdateNextLockTime()
{
    next_lock_time = s_hr_time_max;

    for( const auto& it: locks )
    {
        next_lock_time = std::min( next_lock_time, it.second.time );
    }
}

void CJunction::UpdateNextRequestTime()
{
    next_request_time = s_hr_time_min;

    for( const auto& it: requests )
    {
        next_request_time = std::max( next_request_time, it.second.time );
    }
}

