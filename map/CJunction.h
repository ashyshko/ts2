#pragma once

#include <Defs.h>
#include <CPath.h>

class CJunction
{
public:
    PathId_t hi_path;
    PathId_t lo_path;

    explicit CJunction( PathId_t hi_path_, Distance_t hi_path_begin, Distance_t hi_path_end,
                        PathId_t lo_path_, Distance_t lo_path_begin, Distance_t lo_path_end )
        :   hi_path(hi_path_),
            lo_path(lo_path_),
            m_next_lock_id(1)
    {
        HiPath()->AddJunctionData( CPath::SJunctionData( JunctionId(), lo_path, hi_path_begin, hi_path_end, true ) );
        LoPath()->AddJunctionData( CPath::SJunctionData( JunctionId(), hi_path, lo_path_begin, lo_path_end, false ) );
    }

    ~CJunction()
    {
        HiPath()->RemoveJunctionData( JunctionId() );
        LoPath()->RemoveJunctionData( JunctionId() );
    }

    JunctionId_t JunctionId() const
    {
        return ToJunctionId(this);
    }

    CPath* HiPath() const
    {
        return ToPath(hi_path);
    }

    CPath* LoPath() const
    {
        return ToPath(lo_path);
    }
};

inline JunctionId_t ToJunctionId( const CJunction* junction )
{
    return (JunctionId_t)junction;
}

inline CJunction* ToJunction( JunctionId_t junction_id )
{
    return (CJunction*)junction_id;
}
