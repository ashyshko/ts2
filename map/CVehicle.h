#pragma once

#include <vector>
#include <Defs.h>

class CVehicle
{
public:
    EVehicleClass veh_class;
    SVehicleDimension dim;
    Distance_t total_length;

    struct SPathSegment
    {
        PathId_t path;
        Distance_t pos_in, pos_out;
        Speed_t speed_in, speed_out;
        Acceleration_t accel;
        JunctionId_t junc;

        explicit SPathSegment( PathId_t path_, Distance_t pos_in_, Distance_t pos_out_, 
                                Speed_t speed_in_, Speed_t speed_out_, Acceleration_t accel_, JunctionId_t junc_ = s_invalid_junc_id );
    };

    std::vector<PathId_t> route;
    std::vector<SPathSegment> path_segments;
    size_t path_segments_indexes[2]; // 0 for main unit, 1 for trailer
    Distance_t path_segment_progress[2]; // 0 for main unit, 1 for trailer

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