#pragma once

#include <vector>
#include <Defs.h>

class CVehicle
{
public:
    EVehicleClass veh_class;
    SVehicleDimension dim;
    std::vector<Distance_t> tracking_points;
    Distance_t total_length;

    std::vector<PathId_t> route;
    
    struct SPosition
    {
        PathId_t path;
        Distance_t pos;
    };

    std::vector<SPosition> pos;
    Speed_t speed;
    Acceleration_t accel;

    explicit CVehicle( EVehicleClass veh_class_, PathId_t path, Distance_t path_pos );
    ~CVehicle();
    
    VehicleId_t VehicleId() const;
    
    void GetMinimalRoute( std::vector<PathId_t>& min_route ) const;
    void ReplaceRoute( std::vector<PathId_t>&& new_route );
};

inline VehicleId_t ToVehicleId( const CVehicle* veh )
{
    return (VehicleId_t)veh;
}

inline CVehicle* ToVehicle( VehicleId_t veh_id )
{
    return (CVehicle*)veh_id;
}