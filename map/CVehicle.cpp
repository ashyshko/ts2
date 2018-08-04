#include <CVehicle.h>
#include <CPath.h>

CVehicle::CVehicle( EVehicleClass veh_class_, PathId_t path, Distance_t path_pos, const std::vector<Distance_t>& tracking_points_, Distance_t length )
    :   veh_class(veh_class_),
        tracking_points( tracking_points_.size() + 2 ),
        pos( tracking_points.size() ),
        speed(0),
        accel(0)
{
    {
        tracking_points[0] = 0;
        std::copy( tracking_points_.begin(), tracking_points_.end(), tracking_points.begin() + 1 );
        tracking_points[ tracking_points.size() - 1 ] = length;
    }

    assert( path_pos < ToPath(path)->distance );
    assert( length <= path_pos );
    current_location.push_back( SLocation( path, ToPath(path)->RegisterVehicle( ToVehicleId(), path_pos ) ) );

    assert( tracking_points.size() == pos.size() );
    for( size_t i = 0; i < tracking_points.size(); ++i )
    {
        pos[i] = SPosition( path, path_pos - tracking_points[i] );
    }
}

CVehicle::~CVehicle()
{
    while( !current_location.empty() )
    {
        ToPath( current_location.front() )->RemoveVehicle( ToVehicleId() );
        current_location.pop_front();
    }

}

VehicleId_t CVehicle::VehicleId() const
{
    return ToVehicleId(this);
}

void CVehicle::Calculate()
{
    speed = FromKmphF(30.0);
    accel = 0;
}

void CVehicle::Move()
{
    Distance_t dist = (long long)speed * HRTicksPerTick() + (long long)accel * HRTicksPerTick() * HRTicksPerTick() / 2;
    speed = speed + (long long)accel * HRTicksPerTick();

    Move(dist);
}

PathId_t CVehicle::GetCurrentPath() const
{
    return current_location.front().path;
}

void CVehicle::ReplaceRoute( std::vector<PathId_t>&& new_route )
{
    route.swap(new_route);
    assert( ToPath( route.front() )->from_wp == ToPath( GetCurrentPath() )->to_wp );
}

void CVehicle::Move( Distance_t distance )
{
    Distance_t moved = 0;
    assert( !current_location.empty() );
    while( distance > moved )
    {
        SLocation& loc = current_location.front();
        CPath::CVehicleList::iterator it = loc.it;
        CPath* path = ToPath( loc.path );
        Distance_t path_pos = it->second;

        if( path_pos < path->distance - 1 - moved )
        {
            Distance_t max_move = path->distance - 1 - path_pos - moved;
            max_move = std::min( max_move, distance );
            path->MoveVehicle( it, path_pos + max_move );
            moved += max_move;
        }
        else
        {
            if( route.size() == 1 )
            {
                break;
            }

            route.pop_front();

            CPath* new_path = ToPath( route.front() );
            current_location.push_front( SLocation( ToPathId(new_path), new_path->RegisterVehicle( ToVehicleId(), 0 ) ) );
        }
    }

    auto loc_it = current_location.begin();
    size_t pos_index = 0;
    moved = 0;

    while( moved < distance )
    {
        assert( loc_it != current_location.end() );
        SLocation& loc = *loc_it;
        CPath* path = ToPath( loc.path );
        
        while( pos_index < tracking_points.size() && moved + path->distance > tracking_points[pos_index] )
        {
            pos[pos_index].path = loc.path;
            pos[pos_index].pos = tracking_points[pos_index] - moved;
            ++pos_index;
        }

        moved += path->distance;
        ++loc_it;
    }

    assert( pos_index == tracking_points.size() );

    while( loc_it != current_location.end() )
    {
        SLocation& loc = *loc_it;
        CPath* path = ToPath( loc.path );
        path->RemoveVehicle( loc.it );
        loc_it = current_location.erase(loc_it);
    }
}
