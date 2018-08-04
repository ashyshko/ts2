#include <CJunction.h>

CJunction::CJunction( PathId_t hi_path_, Distance_t hi_path_begin, Distance_t hi_path_end,
                        PathId_t lo_path_, Distance_t lo_path_begin, Distance_t lo_path_end )
    :   hi_path(hi_path_),
        lo_path(lo_path_),
        m_hi_lock_time(s_hr_time_max)
{
    HiPath()->AddJunction( JunctionId(), hi_path_begin, hi_path_end ) );
    LoPath()->AddJunction( JunctionId(), lo_path_begin, lo_path_end ) );
}

CJunction::~CJunction()
{
    HiPath()->RemoveJunction( JunctionId() );
    LoPath()->RemoveJunction( JunctionId() );
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

void CJunction::Lock( PathId_t path, HRTime_t time )
{
    assert( path == hi_path || path == lo_path );
    if( path == hi_path )
    {
        m_hi_lock_time = time;
    }
}

void CJunction::Unlock( PathId_t path )
{
    assert( path == hi_path || path == lo_path );
    this->Lock( path, s_hr_time_max );
}

bool CJunction::IsStopAvailable( PathId_t path ) const
{
    assert( path == hi_path || path == lo_path );
    return path == hi_path;
}

bool CJunction::IsOutrunAvailable( PathId_t path, HRTime_t leave_before_time ) const
{
    assert( path == hi_path || path == lo_path );
    return ( path == hi_path ) || ( leave_before_time <= m_hi_lock_time );
}