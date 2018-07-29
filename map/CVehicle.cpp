#include <CVehicle.h>
#include <CPath.h>

CVehicle::SPathSegment::SPathSegment( PathId_t path_, Distance_t pos_in_, Distance_t pos_out_, 
                                        Speed_t speed_in_, Speed_t speed_out_, Acceleration_t accel_, JunctionId_t junc_ )
    :   path(path_),
        pos_in(pos_in_),
        pos_out(pos_out_),
        speed_in(speed_in_),
        speed_out(speed_out_),
        accel(accel_),
        junc(junc_)
{
}

CVehicle::CVehicle( EVehicleClass veh_class_, PathId_t path, Distance_t path_pos )
    :   veh_class(veh_class_),
        dim( s_vehicle_dimensions[veh_class] ),
        total_length( std::max( dim.length, dim.trail_joint_point + dim.trail_length ) )
{
    assert( path_pos < ToPath(path)->distance );
    assert( total_length <= path_pos );

    route.push_back(path);
    path_segments.push_back( SPathSegment( path, path_pos-total_length, path_pos, 0, 0, 0 ) );
    path_segments_indexes[0] = path_segments_indexes[1] = 0;
    path_segment_progress[0] = path_segment_progress[1] = 0;
}

CVehicle::~CVehicle()
{

}

VehicleId_t CVehicle::VehicleId() const
{
    return ToVehicleId(this);
}

void CVehicle::GetMinimalRoute( std::vector<PathId_t>& min_route ) const
{
    size_t current_path_segment_index = path_segments_indexes[0];
    const SPathSegment& current_path_segment = path_segments[current_path_segment_index];
    PathId_t current_path = current_path_segment.path;
    auto route_it = std::find( route.begin(), route.end(), current_path );
    assert( route_it != route.end() );
    min_route.reserve( route_it - route.begin() );
    min_route.insert( min_route.end(), route.begin(), route_it + 1 );
}

void CVehicle::ReplaceRoute( std::vector<PathId_t>&& new_route )
{
    route.swap(new_route);
}

void CVehicle::LockWasCanceled( PathId_t path, size_t segment_index )
{

}

void CVehicle::MoveThroughWasCanceled( PathId_t path, size_t segment_index )
{
    
}
