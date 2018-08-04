#pragma once

#include <vector>
#include <Defs.h>

class CVehicle
{
public:
    EVehicleClass veh_class;
    std::vector<Distance_t> tracking_points;

    std::vector<PathId_t> route;
    
    struct SPosition
    {
        PathId_t path;
        Distance_t pos;

        SPosition() {}
        explicit SPosition( PathId_t path_, Distance_t pos_ )
            :   path(path_),
                pos(pos_)
        {    
        }
    };

    std::vector<SPosition> pos;
    Speed_t speed;
    Acceleration_t accel;

    struct SLocation
    {
        PathId_t path;
        CPath::CVehicleList::iterator it;

        explicit SLocation( PathId_t path_, CPath::CVehicleList::iterator it_ )
            :   path(path_),
                it(it_)
        {
        }
    };

    std::list<SLocation> current_location;

    explicit CVehicle( EVehicleClass veh_class_, PathId_t path, Distance_t path_pos, const std::vector<Distance_t>& tracking_points, Distance_t length );
    ~CVehicle();
    
    VehicleId_t VehicleId() const;
    
    void Calculate();
    void Move();

    PathId_t GetCurrentPath() const;
    void ReplaceRoute( std::vector<PathId_t>&& new_route );

private:
    void Move( Distance_t distance );
};

inline VehicleId_t ToVehicleId( const CVehicle* veh )
{
    return (VehicleId_t)veh;
}

inline CVehicle* ToVehicle( VehicleId_t veh_id )
{
    return (CVehicle*)veh_id;
}