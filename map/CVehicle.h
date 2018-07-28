#pragma once

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
                                Speed_t speed_in_, Speed_t speed_out_, Acceleration_t accel_, JuncId_t junc_ = s_invalid_junc_id )
            :   path(path_),
                pos_in(pos_in_),
                pos_out(pos_out_),
                speed_in(speed_in_),
                speed_out(speed_out_),
                accel(accel_),
                junc(junc_)
        {
        }
    };

    std::vector<PathId_t> route;
    std::vector<SPathSegment> path_segments;
    size_t path_segments_indexes[2]; // 0 for main unit, 1 for trailer
    Distance_t path_segment_progress[2]; // 0 for main unit, 1 for trailer

    explicit CVehicle( EVehicleClass veh_class_, PathId_t path, Distance_t path_pos )
        :   veh_class(veh_class_),
            dim( s_vehicle_dimensions[veh_class] ),
            total_length( std::max( dim.length, dim.joint_pos + dim.trail_length ) )
    {
        assert( path_pos < ToPath(path)->distance );
        assert( total_length <= path_pos );

        route.push_back(path);
        path_segments.push_back( SPathSegment( path, path_pos-total_length, path_pos, 0, 0, 0 ) );
        path_segments_indexes[0] = path_segments_indexes[1] = 0;
        path_segment_progress[0] = path_segment_progress[1] = 0;
    }

    ~CVehicle()
    {

    }

    VehicleId_t VehicleId() const
    {
        return ToVehicleId(this);
    }

    void GetMinimalRoute( std::vector<PathId_t>& min_route ) const
    {
        size_t current_path_segment_index = path_segments_indexes[0];
        const SVehiclePathSegment& current_path_segment = path_segments[current_path_segment_index];
        PathId_t current_path = current_path_segment.path;
        auto route_it = route.find(current_path);
        assert( route_it != route.end() );
        min_route.reserve( route_it - route.begin() );
        min_route.insert( new_route.end(), route.begin(), route_it + 1 );
    }

    void ReplaceRoute( std::vector<PathId_t>&& new_route )
    {
        route.swap(new_route);
    }
};

inline VehicleId_t ToVehicleId( const CVehicle* veh )
{
    return (VehicleId_t)veh;
}

inline CVehicle* ToVehicleContext( VehicleId_t veh_id )
{
    return (CVehicle*)veh_id;
}