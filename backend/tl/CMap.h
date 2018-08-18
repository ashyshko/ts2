#pragma once

#include <hub/CHub.h>

class CMap
{
public:
    explicit CMap( Point_t width, Point_t height );

    static Point_t const s_lane_width;
    static Point_t const s_cross_size;

    enum EDirection
    {
        Direction_Left,
        Direction_Top,
        Direction_Right,
        Direction_Bottom
    };

    bool AddRoad( Point_t x, Point_t y, Point_t length, EDirection direction, size_t straight_lane_count, size_t opposite_lane_count );
    void AddVehicle( EVehicleClass veh_class, bool direction_tl, double pos );

// temporary public:

    struct SRect
    {
        Point_t x1, y1, x2, y2;

        bool IsValid() const;

        static SRect FromLTRB( Point_t x1_, Point_t y1_, Point_t x2_, Point_t y2_ );
        bool Intersects( const SRect& other, SRect& intersect ) const;
        bool Intersects( const SRect& other ) const;
    };

    struct SRoad;

    struct SLane
    {
        SRoad* road;
        size_t index;
        bool straight;
        SRect rect;
        int from_x, from_y, to_x, to_y;
        WayPointId_t in;
        WayPointId_t out;
        PathId_t path;
    };

    struct SCross;

    struct SRoad
    {
        Point_t x, y;
        Point_t length;
        bool horizontal;
        std::vector<SLane*> straight_lanes;
        std::vector<SLane*> opposite_lanes;
        SCross* cross_in;
        SCross* cross_out;

        size_t LaneCount() const;

        static Point_t TotalLanesWidth( size_t lane_count );
        Point_t TotalLengthWidth() const;

        static SRect BoundRect( Point_t x, Point_t y, Point_t length, size_t total_lanes, bool horizontal, Point_t reserve = 0 );
        static SRect BoundRectForCross( Point_t x, Point_t y, Point_t length, size_t total_lanes, bool horizontal );

        SRect BoundRect() const;
        SRect BoundRectForCross() const;
    };

    enum EConnectionType
    {
        ConnectionType_LeftTurn,
        ConnectionType_Straight,
        ConnectionType_RightTurn,
        ConnectionType_TurnAround,
        ConnectionTypeCount
    };

    struct SConnection
    {
        SLane* in;
        SLane* out;
        PathId_t path;
        EConnectionType type;
        std::map<SConnection*, JunctionId_t> juncs;
    };

    struct SCross
    {
        SRoad* road_left;
        SRoad* road_top;
        SRoad* road_right;
        SRoad* road_bottom;

        std::set<SConnection*> connections;

        SCross();
        SRect BoundRect() const;
    };

    const std::set<SRoad*>& Roads() const { return m_roads; }
    const std::set<SCross*>& Crosses() const { return m_crosses; }

private:
    SRoad* AddRoad0( Point_t x, Point_t y, Point_t length, bool horizontal, size_t straight_lane_count, size_t opposite_lane_count, SCross* cross_in, SCross* cross_out );
    void RemoveRoad0( SRoad* road );
    void RegisterLane0( SLane* lane );
    void UnregisterLane0( SLane* lane );
    void SplitRoad0( SRoad* road, SCross* cross, Point_t p1, Point_t p2 );
    std::set<SConnection*>::iterator RemoveCrossConnection0( SCross* cross, SConnection* connection );

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

    SConnection* UpdateCrossConnection0( SCross* cross, SLane* lane_in, SLane* lane_out, EConnectionType type );
    void UpdateCrossTurns0( SCross* cross );
    std::set<SConnection*> UpdateCrossTurns0( SCross* cross, SRoad* road, bool straight, bool left0,
                            SRoad* road_left, bool left_straight,
                            SRoad* road_straight,
                            SRoad* road_right, bool right_straight );
    void UpdateCrossLeft0( SCross* cross, SRoad* road );
    void UpdateCrossTop0( SCross* cross, SRoad* road );
    void UpdateCrossRight0( SCross* cross, SRoad* road );
    void UpdateCrossBottom0( SCross* cross, SRoad* road );

private:
    CHub m_hub;
    Point_t m_width;
    Point_t m_height;
    std::set<SRoad*> m_roads;
    std::set<SCross*> m_crosses;
};
