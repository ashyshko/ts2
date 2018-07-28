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
};

inline JunctionId_t ToJunctionId( const CJunction* junction )
{
    return (JunctionId_t)junction;
}

inline CJunction* ToJunction( JunctionId_t junction_id )
{
    return (CJunction*)junction_id;
}
