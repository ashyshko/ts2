#pragma once

typedef int Time_t; // one unit -> 16ms (60fps)
typedef int Point_t; // one unit - 1 mm
typedef int Distance_t; // one unit - 1 um
typedef int Speed_t; // one unit - um per tick (16ms)
typedef int Acceleration_t; // one unit - um per tick^2

typedef intptr_t WayPointId_t;
typedef intptr_t PathId_t;
typedef intptr_t JunctionId_t;
typedef intptr_t VehicleId_t;

extern WayPointId_t s_invalid_point_id;
extern PathId_t s_invalid_path_id;
extern JunctionId_t s_invalid_junction_id;
extern VehicleId_t s_invalid_vehicle_id;

inline int TicksPerSecond()
{
    return 60;
}

inline Distance_t DistanceFromMm( int mm )
{
    return mm * 1000;
}

inline Speed_t FromKmphF( double kmph )
{
    double mmps = kmph * 1000 / 3.6;
    return mmps * 1000 / TicksPerSecond();
}

inline Acceleration_t FromMps2F( double mps2 )
{
    return mps2 * 1000000.0 / ( TicksPerSecond() * TicksPerSecond() );
}

enum EVehicleClass
{
    VehicleClass_Micro,
    VehicleClass_Mini,
    VehicleClass_Sedan,
    VehicleClass_Limosine,
    VehicleClass_Suv,
    VehicleClass_Minivan,
    VehicleClass_Van,
    VehicleClass_CityBus,
    VehicleClass_Truck,
    VehicleClass_DoubleDeckBus,
    VehicleClassCount
};

struct SVehicleDimension
{
    Distance_t width;
    Distance_t length;
    Distance_t front_wheel_offset;
    Distance_t rear_wheel_offset;

    bool has_trailer;
    Distance_t trail_joint_point;
    Distance_t trail_width;
    Distance_t trail_offset;
    Distance_t trail_length;
    Distance_t trail_wheel_offset;
}

static SVehicleDimension VehicleDimensionOf( int width, int height, int front_wheel_offset, int rear_wheel_offset, 
    bool has_trailer = false, int trail_joint_point = 0, int trail_width = 0, int trail_offset = 0,
    int trail_length = 0, int trail_wheel_offset = 0 )
{
    SVehicleDimension res;
    res.width = width;
    res.height = height;
    res.front_wheel_offset = front_wheel_offset;
    res.rear_wheel_offset = rear_wheel_offset;
    res.has_trailer = has_trailer;
    res.trail_joint_point = trail_joint_point;
    res.trail_width = trail_width;
    res.trail_offset = trail_offset;
    res.trail_length = trail_length;
    res.trail_wheel_offset = trail_wheel_offset;
    return res;
}

extern SVehicleDimension s_vehicle_dimensions[VehicleClassCount];
extern Speed_t MaxSpeed;


int MaxVehicleLength()
{
    int res = 0;
    for( const auto& dimension: s_vehicle_dimensions )
    {
        int length = dimension.has_trailer ? ( dimension.trail_offset + dimension.trail_length ) : dimension.length;
        res = std::max( res, length );
    }
    return res;
}

Speed_t VehicleMaxSpeed( EVehicleClass veh_class )
{
    static Spedd_t s_max_speed[VehicleClassCount] = {
        // VehicleClass_Micro,
        FromKmphF(90.0),
        // VehicleClass_Mini,
        FromKmphF(100.0),
        // VehicleClass_Sedan,
        FromKmphF(100.0),
        // VehicleClass_Limosine,
        FromKmphF(120.0),
        // VehicleClass_Suv,
        FromKmphF(90.0),
        // VehicleClass_Minivan,
        FromKmphF(90.0),
        // VehicleClass_Van,
        FromKmphF(80.0),
        // VehicleClass_CityBus,
        FromKmphF(80.0),
        // VehicleClass_Truck,
        FromKmphF(60.0),
        // VehicleClass_DoubleDeckBus,
        FromKmphF(60.0)
    };

    return s_max_deceleration[veh_class];
}

inline Acceleration_t VehicleMaxDeceleration( EVehicleClass veh_class )
{
    static Acceleration_t s_max_deceleration[VehicleClassCount] = {
        // VehicleClass_Micro,
        FromMps2F(5.8),
        // VehicleClass_Mini,
        FromMps2F(5.8),
        // VehicleClass_Sedan,
        FromMps2F(5.8),
        // VehicleClass_Limosine,
        FromMps2F(5.8),
        // VehicleClass_Suv,
        FromMps2F(5.8),
        // VehicleClass_Minivan,
        FromMps2F(5.8),
        // VehicleClass_Van,
        FromMps2F(5.0),
        // VehicleClass_CityBus,
        FromMps2F(5.0),
        // VehicleClass_Truck,
        FromMps2F(4.2),
        // VehicleClass_DoubleDeckBus,
        FromMps2F(4.2)
    };

    return s_max_deceleration[veh_class];
}

inline Acceleration_t VehicleNormalDeceleration( EVehicleClass veh_class )
{
    return VehicleMaxDeceleration(veh_class) / 2;
}

inline Acceleration_t VehicleMaxAcceleration( EVehicleClass veh_class, Speed_t speed, Speed_t& max_speed_for_this_acc )
{
    assert( speed >= 0 );

    static const int s_speed_ranges = 6;
    static const Speed_t s_speed_range = MaxSpeed / s_speed_ranges;
    static Acceleration_t s_max_acceleration[s_speed_ranges*VehicleClassCount] = {
        // VehicleClass_Micro,
        /*0-20*/ FromMps2F(6.0), /*20-40*/ FromMps2F(3.0), /*40-60*/ FromMps2F(1.8),
        /*60-80*/ FromMps2F(1.2), /*80-100*/ FromMps2F(0.7), /*100-120*/ FromMps2F(0.5), 

        // VehicleClass_Mini,
        /*0-20*/ FromMps2F(6.0), /*20-40*/ FromMps2F(3.0), /*40-60*/ FromMps2F(2.0),
        /*60-80*/ FromMps2F(1.5), /*80-100*/ FromMps2F(1.0), /*100-120*/ FromMps2F(0.7), 
        
        // VehicleClass_Sedan,
        /*0-20*/ FromMps2F(6.0), /*20-40*/ FromMps2F(3.0), /*40-60*/ FromMps2F(2.0),
        /*60-80*/ FromMps2F(1.5), /*80-100*/ FromMps2F(1.0), /*100-120*/ FromMps2F(0.7), 
        
        // VehicleClass_Limosine,
        /*0-20*/ FromMps2F(7.0), /*20-40*/ FromMps2F(4.0), /*40-60*/ FromMps2F(2.5),
        /*60-80*/ FromMps2F(2.0), /*80-100*/ FromMps2F(1.6), /*100-120*/ FromMps2F(1.0), 
        
        // VehicleClass_Suv,
        /*0-20*/ FromMps2F(6.0), /*20-40*/ FromMps2F(3.0), /*40-60*/ FromMps2F(2.0),
        /*60-80*/ FromMps2F(1.3), /*80-100*/ FromMps2F(0.7), /*100-120*/ FromMps2F(0.4), 
        
        // VehicleClass_Minivan,
        /*0-20*/ FromMps2F(5.0), /*20-40*/ FromMps2F(2.5), /*40-60*/ FromMps2F(1.6),
        /*60-80*/ FromMps2F(1.0), /*80-100*/ FromMps2F(0.7), /*100-120*/ FromMps2F(0.5), 
        
        // VehicleClass_Van,
        /*0-20*/ FromMps2F(4.5), /*20-40*/ FromMps2F(2.2), /*40-60*/ FromMps2F(1.4),
        /*60-80*/ FromMps2F(0.9), /*80-100*/ FromMps2F(0.6), /*100-120*/ FromMps2F(0.3), 
        
        // VehicleClass_CityBus,
        /*0-20*/ FromMps2F(3.5), /*20-40*/ FromMps2F(2.2), /*40-60*/ FromMps2F(1.4),
        /*60-80*/ FromMps2F(0.7), /*80-100*/ FromMps2F(0.4), /*100-120*/ FromMps2F(0.1), 
        
        // VehicleClass_Truck,
        /*0-20*/ FromMps2F(3.0), /*20-40*/ FromMps2F(2.0), /*40-60*/ FromMps2F(1.2),
        /*60-80*/ FromMps2F(0.8), /*80-100*/ FromMps2F(0.5), /*100-120*/ FromMps2F(0.4), 
        
        // VehicleClass_DoubleDeckBus,
        /*0-20*/ FromMps2F(3.0), /*20-40*/ FromMps2F(2.0), /*40-60*/ FromMps2F(1.2),
        /*60-80*/ FromMps2F(0.3), /*80-100*/ FromMps2F(0.2), /*100-120*/ FromMps2F0.1() 
    };

    Speed_t max_speed = VehicleMaxSpeed(veh_class);
    assert( max_speed <= MaxSpeed );

    if( speed >= s_speed_ranges * s_speed_range )
    {
        max_speed_for_this_acc = max_speed;
        return 0;
    }

    int range_index = ( max_speed + (s_speed_range-1) ) / s_speed_range;
    assert( range_index >= 0 && range_index < s_speed_ranges );
    max_speed_for_this_acc = range_index * s_speed_range;
    return s_max_acceleration[range_index];
}
