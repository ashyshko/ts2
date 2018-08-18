#include <CMap.h>

//=====================================================================================================================
bool CMap::SRect::IsValid() const
{
    return x2 <= x1 && y2 <= y1;
}

/*static*/ CMap::SRect CMap::SRect::FromLTRB( Point_t x1_, Point_t y1_, Point_t x2_, Point_t y2_ )
{
    SRect res;
    res.x1 = x1_;
    res.y1 = y1_;
    res.x2 = x2_;
    res.y2 = y2_;
    return res;
}

bool CMap::SRect::Intersects( const SRect& other, SRect& intersect ) const
{
    assert( IsValid() && other.IsValid() );
    intersect = FromLTRB( std::max( x1, other.x2 ), std::max( y1, other.y1 ), std::min( x2, other.x2 ), std::min( y2, other.y2 ) );
    return intersect.IsValid();
}

bool CMap::SRect::Intersects( const SRect& other ) const
{
    SRect intersect;
    return this->Intersects( other, intersect );
}

//=====================================================================================================================
size_t CMap::SRoad::LaneCount() const
{
    return straight_lanes.size() + opposite_lanes.size();
}

/*static*/ Point_t CMap::SRoad::TotalLanesWidth( size_t lane_count )
{
    return lane_count * s_lane_width;
}

Point_t CMap::SRoad::TotalLengthWidth() const
{
    return TotalLanesWidth( this->LaneCount() );
}

/*static*/ CMap::SRect CMap::SRoad::BoundRect( Point_t x, Point_t y, Point_t length, size_t total_lanes, bool horizontal, Point_t reserve )
{
    Point_t x1, y1, x2, y2;
    if( horizontal )
    {
        x1 = x - reserve;
        x2 = x + length + reserve;
        y1 = y - TotalLanesWidth(total_lanes) / 2 - reserve;
        y2 = y + TotalLanesWidth(total_lanes) / 2 + reserve;
    }
    else
    {
        x1 = x - TotalLanesWidth(total_lanes) / 2 - reserve;
        x2 = x + TotalLanesWidth(total_lanes) / 2 + reserve;
        y1 = y - reserve;
        y2 = y + length + reserve;
    }
    return SRect::FromLTRB( x1, y1, x2, y2 );
}

/*static*/ CMap::SRect CMap::SRoad::BoundRectForCross( Point_t x, Point_t y, Point_t length, size_t total_lanes, bool horizontal )
{
    return BoundRect( x, y, length, total_lanes, horizontal, s_cross_size );
}

CMap::SRect CMap::SRoad::BoundRect() const
{
    return BoundRect( this->x, this->y, this->length, this->LaneCount(), this->horizontal );            
}

CMap::SRect CMap::SRoad::BoundRectForCross() const
{
    return BoundRect( this->x, this->y, this->length, this->LaneCount(), this->horizontal, s_cross_size );            
}

//=====================================================================================================================
CMap::SCross::SCross()
    :   road_left(nullptr),
        road_top(nullptr),
        road_right(nullptr),
        road_bottom(nullptr)
{

}

CMap::SRect CMap::SCross::BoundRect() const
{
    SRect r = SRect::FromLTRB( s_point_max, s_point_max, s_point_min, s_point_min );

    if( road_left != nullptr )
    {
        SRect bound_rect = road_left->BoundRect();
        SRect bound_rect_for_cross = road_left->BoundRectForCross();
        r.x1 = std::min( r.x1, bound_rect.x2 );
        r.y1 = std::min( r.y1, bound_rect_for_cross.y1 );
        r.x2 = std::max( r.x2, bound_rect_for_cross.x2 );
        r.y2 = std::max( r.y2, bound_rect_for_cross.y2 );
    }

    if( road_top != nullptr )
    {
        SRect bound_rect = road_top->BoundRect();
        SRect bound_rect_for_cross = road_top->BoundRectForCross();
        r.x1 = std::min( r.x1, bound_rect_for_cross.x1 );
        r.y1 = std::min( r.y1, bound_rect.y2 );
        r.x2 = std::max( r.x2, bound_rect_for_cross.x2 );
        r.y2 = std::max( r.y2, bound_rect_for_cross.y2 );
    }

    if( road_right != nullptr )
    {
        SRect bound_rect = road_right->BoundRect();
        SRect bound_rect_for_cross = road_right->BoundRectForCross();
        r.x1 = std::min( r.x1, bound_rect_for_cross.x1 );
        r.y1 = std::min( r.y1, bound_rect_for_cross.y1 );
        r.x2 = std::max( r.x2, bound_rect.x1 );
        r.y2 = std::max( r.y2, bound_rect_for_cross.y2 );
    }

    if( road_bottom != nullptr )
    {
        SRect bound_rect = road_bottom->BoundRect();
        SRect bound_rect_for_cross = road_bottom->BoundRectForCross();
        r.x1 = std::min( r.x1, bound_rect_for_cross.x1 );
        r.y1 = std::min( r.y1, bound_rect_for_cross.y1 );
        r.x2 = std::max( r.x2, bound_rect_for_cross.x2 );
        r.y2 = std::max( r.y2, bound_rect.y1 );
    }

    assert( r.IsValid() );
    return r;
}


//=====================================================================================================================
/*static*/ Point_t const CMap::s_lane_width = PointFromMm(3000);
/*static*/ Point_t const CMap::s_cross_size = PointFromMm(3000);

CMap::CMap( Point_t width, Point_t height )
    :   m_width(width),
        m_height(height)
{

}


bool CMap::AddRoad( Point_t x, Point_t y, Point_t length, EDirection direction, size_t straight_lane_count, size_t opposite_lane_count )
{
    switch(direction)
    {
    case Direction_Left:
        return AddRoad( x + length, y, length, Direction_Right, opposite_lane_count, straight_lane_count );
    case Direction_Top:
        return AddRoad( x, y + length, length, Direction_Bottom, opposite_lane_count, straight_lane_count );
    }

    assert( direction == Direction_Right || direction == Direction_Bottom );
    bool horizontal = direction == Direction_Right;
    size_t total_lanes = straight_lane_count + opposite_lane_count;
    SRect bound_rect = SRoad::BoundRectForCross( x, y, length, total_lanes, horizontal );

    std::vector<std::pair<SRect, SRoad*>> new_cross_rects;
    for( SRoad* other: m_roads )
    {
        SRect intersection;
        if( other->BoundRectForCross().Intersects( bound_rect, intersection ) )
        {
            if( other->horizontal == horizontal )
            {
                return false;
            }

            new_cross_rects.push_back( std::make_pair( intersection, other ) );
        }
    }

    if(horizontal)
    {
        new_cross_rects.push_back( std::make_pair( SRoad::BoundRectForCross( x, y, 0, total_lanes, horizontal ), nullptr ) );
        new_cross_rects.push_back( std::make_pair( SRoad::BoundRectForCross( x+length, y, 0, total_lanes, horizontal ), nullptr ) );
    }
    else
    {
        new_cross_rects.push_back( std::make_pair( SRoad::BoundRectForCross( x, y, 0, total_lanes, horizontal ), nullptr ) );
        new_cross_rects.push_back( std::make_pair( SRoad::BoundRectForCross( x, y+length, 0, total_lanes, horizontal ), nullptr ) );
    }

    std::vector<SCross*> existing_crosses;


    for( SCross* existing_cross: m_crosses )
    {
        if( existing_cross->BoundRect().Intersects(bound_rect) )
        {
            existing_crosses.push_back(existing_cross);
        }
    }

    for( size_t i = 0; i < new_cross_rects.size(); )
    {
        bool remove = false;

        for( SCross* existing_cross: existing_crosses )
        {
            if( existing_cross->BoundRect().Intersects(new_cross_rects[i].first) )
            {
                remove = true;
                break;
            }
        }

        if(remove)
        {
            new_cross_rects.erase( new_cross_rects.begin() + i );
        }
        else
        {
            ++i;
        }
    }

    std::sort( existing_crosses.begin(), existing_crosses.end(),
        [horizontal]( const SCross* l, const SCross* r )
        {
            if(horizontal)
            {
                return l->BoundRect().x1 < r->BoundRect().x1;
            }
            else
            {
                return l->BoundRect().y1 < r->BoundRect().y1;
            }
        } );
    
    std::sort( new_cross_rects.begin(), new_cross_rects.end(),
        [horizontal]( const std::pair<SRect,SRoad*>& l, const std::pair<SRect,SRoad*>& r )
        {
            if(horizontal)
            {
                return l.first.x1 < r.first.x1;
            }
            else
            {
                return l.first.y1 < r.first.y1;
            }
        } );
    
    size_t existing_cross_index = 0;
    size_t new_cross_index = 0;

    SCross* prev_cross = nullptr;
    Point_t prev_cross_end = s_point_max;

    while( existing_cross_index < existing_crosses.size() || new_cross_index < new_cross_rects.size() )
    {
        Point_t existing_pos = s_point_max;
        Point_t new_pos = s_point_max;

        if( existing_cross_index < existing_crosses.size() )
        {
            existing_pos = horizontal ? existing_crosses[existing_cross_index]->BoundRect().x1 : existing_crosses[existing_cross_index]->BoundRect().y1;
        }

        if( new_cross_index < new_cross_rects.size() )
        {
            new_pos = horizontal ? new_cross_rects[new_cross_index].first.x1 : new_cross_rects[new_cross_index].first.y1;
        }

        assert( existing_pos < s_point_max || new_pos < s_point_max );

        SCross* current_cross;

        Point_t cross_begin;
        Point_t cross_end;

        if( existing_pos < new_pos )
        {
            current_cross = existing_crosses[existing_cross_index];

            if(horizontal)
            {
                cross_begin = current_cross->BoundRect().x1;
                cross_end = current_cross->BoundRect().x2;
            }
            else
            {
                cross_begin = current_cross->BoundRect().y1;
                cross_end = current_cross->BoundRect().y2;
            }

            ++existing_cross_index;
        }
        else
        {
            current_cross = new SCross;
            m_crosses.insert(current_cross);

            if(horizontal)
            {
                cross_begin = new_cross_rects[new_cross_index].first.x1;
                cross_end = new_cross_rects[new_cross_index].first.x2;
                SplitRoad0( new_cross_rects[new_cross_index].second, current_cross, bound_rect.y1, bound_rect.y2 );
            }
            else
            {
                cross_begin = new_cross_rects[new_cross_index].first.y1;
                cross_end = new_cross_rects[new_cross_index].first.y2;
                SplitRoad0( new_cross_rects[new_cross_index].second, current_cross, bound_rect.x1, bound_rect.x2 );
            }

            ++new_cross_index;
        }

        if( prev_cross != nullptr )
        {
            if(horizontal)
            {
                AddRoad0( prev_cross_end, y, cross_begin - prev_cross_end, true, straight_lane_count, opposite_lane_count, prev_cross, current_cross );
            }
            else
            {
                AddRoad0( x, prev_cross_end, cross_begin - prev_cross_end, false, straight_lane_count, opposite_lane_count, prev_cross, current_cross );
            }
        }

        prev_cross = current_cross;
        prev_cross_end = cross_end;
    }

    return true;
}

void CMap::AddVehicle( EVehicleClass veh_class, bool direction_tl, double pos )
{

}

CMap::SRoad* CMap::AddRoad0( Point_t x, Point_t y, Point_t length, bool horizontal, size_t straight_lane_count, size_t opposite_lane_count, SCross* cross_in, SCross* cross_out )
{
    assert( cross_in != nullptr && cross_out != nullptr );
    assert( horizontal ? ( cross_in->road_right == nullptr ) : ( cross_in->road_bottom == nullptr ) );
    assert( horizontal ? ( cross_out->road_left == nullptr ) : ( cross_out->road_top == nullptr ) );

    size_t lane_count = straight_lane_count + opposite_lane_count;

    SRoad* road = new SRoad;
    road->x = x;
    road->y = y;
    road->length = length;
    road->horizontal = horizontal;
    road->cross_in = nullptr;
    road->cross_out = nullptr;

    Point_t lane_pos;
    if(horizontal)
    {
        lane_pos = y - ( lane_count - 1 ) * s_lane_width / 2;
    }
    else
    {
        lane_pos = x - ( lane_count - 1 ) * s_lane_width / 2;
    }
    
    for( size_t i = 0; i < lane_count; ++i )
    {
        SLane* lane = new SLane;
        lane->road = road;
        lane->index = i;
        lane->straight = i < straight_lane_count;
        if(horizontal)
        {
            lane->rect = SRect::FromLTRB( x, lane_pos - s_lane_width / 2, x + length, lane_pos + s_lane_width / 2 );
            lane->from_x = lane->straight ? x : x + length;
            lane->from_y = lane_pos;
            lane->to_x = lane->straight ? x + length : x;
            lane->to_y = lane_pos;
        }
        else
        {
            lane->rect = SRect::FromLTRB( lane_pos - s_lane_width / 2, y, lane_pos + s_lane_width / 2, y + length );
            lane->from_x = lane_pos;
            lane->from_y = lane->straight ? y : y + length;
            lane->to_x = lane_pos;
            lane->to_y = lane->straight ? y + length : y;
        }
        
        RegisterLane0(lane);
        if( lane->straight )
        {
            road->straight_lanes.push_back(lane);
        }
        else
        {
            road->opposite_lanes.push_back(lane);
        }
        lane_pos += s_lane_width;
    }

    if(horizontal)
    {
        UpdateCrossRight0( cross_in, road );
        UpdateCrossLeft0( cross_out, road );
    }
    else
    {
        UpdateCrossBottom0( cross_in, road );
        UpdateCrossTop0( cross_out, road );
    }

    m_roads.insert(road);
    return road;
}

void CMap::RemoveRoad0( SRoad* road )
{
    SCross* cross_in = road->cross_in;
    SCross* cross_out = road->cross_out;

    if( cross_in != nullptr )
    {
        if(road->horizontal)
        {
            assert( cross_in->road_right == road );
            UpdateCrossRight0( cross_in, nullptr );
            assert( cross_out->road_left == road );
            UpdateCrossLeft0( cross_out, nullptr );
        }
        else
        {
            assert( cross_in->road_bottom == road );
            UpdateCrossBottom0( cross_in, nullptr );
            assert( cross_out->road_top == road );
            UpdateCrossTop0( cross_out, nullptr );
        }
    }

    for( SLane* lane: road->straight_lanes )
    {
        UnregisterLane0(lane);
    }

    for( SLane* lane: road->opposite_lanes )
    {
        UnregisterLane0(lane);
    }

    m_roads.erase(road);
    delete road;
}

void CMap::RegisterLane0( SLane* lane )
{
    lane->in = m_hub.AddWayPoint();
    lane->out = m_hub.AddWayPoint();
    lane->path = m_hub.AddPath( lane->in, lane->out, DistanceFromPoint( lane->road->length ) );
}

void CMap::UnregisterLane0( SLane* lane )
{
    m_hub.RemovePath(lane->path);
    m_hub.RemoveWayPoint(lane->in);
    m_hub.RemoveWayPoint(lane->out);
}

void CMap::SplitRoad0( SRoad* road, SCross* cross, Point_t p1, Point_t p2 )
{
    Point_t x = road->x;
    Point_t y = road->y;
    Point_t length = road->length;
    SCross* cross_in = road->cross_in;
    SCross* cross_out = road->cross_out;
    size_t straight_lane_count = road->straight_lanes.size();
    size_t opposite_lane_count = road->opposite_lanes.size();
    bool horizontal = road->horizontal;
    RemoveRoad0(road);

    if(horizontal)
    {
        AddRoad0( x, y, (p1-x), true, straight_lane_count, opposite_lane_count, cross_in, cross );
        AddRoad0( p2, y, ( x + length - p2 ), true, straight_lane_count, opposite_lane_count, cross, cross_out );
    }
    else
    {
        AddRoad0( x, y, (p1-y), false, straight_lane_count, opposite_lane_count, cross_in, cross );
        AddRoad0( x, p2, ( y + length - p1 ), false, straight_lane_count, opposite_lane_count, cross, cross_out );
    }
}

std::set<CMap::SConnection*>::iterator CMap::RemoveCrossConnection0( SCross* cross, SConnection* connection )
{
    for( const std::pair<SConnection*, JunctionId_t>& junc: connection->juncs )
    {
        assert( junc.first->juncs.count(connection) == 1 );
        m_hub.RemoveJunction(junc.second);
        junc.first->juncs.erase(connection);
    }

    connection->juncs.clear();
    delete connection;
    auto connection_it = cross->connections.find(connection);
    assert( connection_it != cross->connections.end() );
    return cross->connections.erase(connection_it);
}

/*

            S0  S1  O0  O1
            \/  \/  /\  /\

    O0 <-                   <- O0
    O1 <-                   <- O1
    S0 ->                   -> S0
    S1 ->                   -> S1

            \/  \/  /\  /\
            S0  S1  O0  O1
*/

CMap::SConnection* CMap::UpdateCrossConnection0( SCross* cross, SLane* lane_in, SLane* lane_out, EConnectionType type )
{
    // temporary
    int dx = lane_out->from_x - lane_in->to_x;
    int dy = lane_out->from_y - lane_in->to_y;
    int distance = std::sqrt( dx * dx + dy * dy );

    for( SConnection* con: cross->connections )
    {
        if( con->in != lane_in || con->out != lane_out )
        {
            continue;
        }

        assert( con->type == type );
        return con;
    }


    SConnection* con = new SConnection;
    con->in = lane_in;
    con->out = lane_out;
    con->path = m_hub.AddPath( lane_in->out, lane_out->in, DistanceFromPoint(distance) );
    con->type = type;
    cross->connections.insert(con);
    return con;
}

void CMap::UpdateCrossTurns0( SCross* cross )
{
    std::set<SConnection*> connections;

    {
        std::set<SConnection*> local = this->UpdateCrossTurns0( cross, cross->road_left, true, true, 
            cross->road_top, false, cross->road_right, cross->road_bottom, true );
        connections.insert( local.begin(), local.end() );
    }
    
    {
        std::set<SConnection*> local = this->UpdateCrossTurns0( cross, cross->road_top, true, false,
            cross->road_right, true, cross->road_bottom, cross->road_left, false );
        connections.insert( local.begin(), local.end() );
    }

    {
        std::set<SConnection*> local = this->UpdateCrossTurns0( cross, cross->road_right, false, false, 
            cross->road_bottom, true, cross->road_left, cross->road_top, false );
        connections.insert( local.begin(), local.end() );
    }
    
    {
        std::set<SConnection*> local = this->UpdateCrossTurns0( cross, cross->road_bottom, false, true, 
            cross->road_left, false, cross->road_top, cross->road_right, true );
        connections.insert( local.begin(), local.end() );
    }

    for( auto it = cross->connections.begin(); it != cross->connections.end(); )
    {
        if( connections.count(*it) != 0 )
        {
            ++it;
            continue;
        }

        it = RemoveCrossConnection0( cross, *it );
    }
}

std::set<CMap::SConnection*> CMap::UpdateCrossTurns0( SCross* cross, SRoad* road, bool straight, bool left0,
                        SRoad* road_left, bool left_straight,
                        SRoad* road_straight,
                        SRoad* road_right, bool right_straight )
{
    if( road == nullptr )
    {
        return std::set<SConnection*>();
    }

    std::set<SConnection*> res;

    std::vector<SLane*>& lanes = straight ? road->straight_lanes : road->opposite_lanes;

    for( size_t i = 0; i < lanes.size(); ++i )
    {
        bool can_turn_left = left0 ? ( i == 0 ) : ( i == lanes.size() - 1 );
        bool can_turn_right = left0 ? ( i == lanes.size() - 1 ) : ( i == 0 );

        // u turn
        if( can_turn_left )
        {
            std::vector<SLane*>& turn_lanes = !straight ? road->straight_lanes : road->opposite_lanes;
            
            if( !turn_lanes.empty() )
            {
                size_t turn_lane_index = left0 ? ( turn_lanes.size() - 1 ) : 0;
                res.insert( UpdateCrossConnection0( cross, lanes[i], turn_lanes[turn_lane_index], ConnectionType_TurnAround ) );
            }
        }

        if( can_turn_left && road_left != nullptr )
        {
            std::vector<SLane*>& turn_lanes = left_straight ? road_left->straight_lanes : road_left->opposite_lanes;
            
            if( !turn_lanes.empty() )
            {
                size_t turn_lane_index;
                if( straight == left_straight )
                {
                    turn_lane_index = left0 ? ( turn_lanes.size() - 1 ) : 0;
                }
                else
                {
                    turn_lane_index = left0 ? 0 : ( turn_lanes.size() - 1 );
                }

                res.insert( UpdateCrossConnection0( cross, lanes[i], turn_lanes[turn_lane_index], ConnectionType_LeftTurn ) );
            }
        }

        if( road_straight != nullptr )
        {
            std::vector<SLane*>& other_lanes = straight ? road_straight->straight_lanes : road_straight->opposite_lanes;
            
            if( i < other_lanes.size() )
            {
                res.insert( UpdateCrossConnection0( cross, lanes[i], other_lanes[i], ConnectionType_Straight ) );
            }
        }

        if( can_turn_right && road_right != nullptr )
        {
            std::vector<SLane*>& turn_lanes = right_straight ? road_right->straight_lanes : road_right->opposite_lanes;
            
            if( !turn_lanes.empty() )
            {
                size_t turn_lane_index;
                if( straight == right_straight )
                {
                    turn_lane_index = left0 ? ( turn_lanes.size() - 1 ) : 0;
                }
                else
                {
                    turn_lane_index = left0 ? 0 : ( turn_lanes.size() - 1 );
                }

                res.insert( UpdateCrossConnection0( cross, lanes[i], turn_lanes[turn_lane_index], ConnectionType_RightTurn ) );
            }
        }
    }

    return res;
}

void CMap::UpdateCrossLeft0( SCross* cross, SRoad* road )
{
    cross->road_left = road;
    UpdateCrossTurns0(cross);
}

void CMap::UpdateCrossTop0( SCross* cross, SRoad* road )
{
    cross->road_top = road;
    UpdateCrossTurns0(cross);
}

void CMap::UpdateCrossRight0( SCross* cross, SRoad* road )
{
    cross->road_right = road;
    UpdateCrossTurns0(cross);
}

void CMap::UpdateCrossBottom0( SCross* cross, SRoad* road )
{
    cross->road_bottom = road;
    UpdateCrossTurns0(cross);
}
