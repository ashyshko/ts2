#pragma once

#include <algorithm>
#include <assert.h>

typedef int Time_t; // one unit -> 0.5s (2 ticks per second)
typedef long long HRTime_t; // one unit - ~0.5ms (2048 ticks per second)
typedef int Point_t; // one unit - 1 mm
typedef int Distance_t; // one unit - 1 um
typedef int Speed_t; // one unit - um per hr tick
typedef int Acceleration_t; // one unit - um per hr tick^2

typedef intptr_t WayPointId_t;
typedef intptr_t PathId_t;
typedef intptr_t JunctionId_t;
typedef intptr_t VehicleId_t;

extern WayPointId_t s_invalid_wp_id;
extern PathId_t s_invalid_path_id;
extern JunctionId_t s_invalid_junc_id;
extern VehicleId_t s_invalid_veh_id;

extern Time_t s_time_max;
extern Time_t s_time_min;
extern HRTime_t s_hr_time_max;
extern HRTime_t s_hr_time_min;

inline int TicksPerSecond()
{
    return 2;
}

inline int HRTicksPerSecond()
{
    return 2048;
}

inline HRTicksPerTick()
{
    return 1024;
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
};

SVehicleDimension VehicleDimensionOf( int width, int length, int front_wheel_offset, int rear_wheel_offset,
                                        bool has_trailer = false, int trail_joint_point = 0, int trail_width = 0, int trail_offset = 0,
                                        int trail_length = 0, int trail_wheel_offset = 0 );

extern SVehicleDimension s_vehicle_dimensions[VehicleClassCount];
extern Speed_t MaxSpeed;

int MaxVehicleLength();
Speed_t VehicleMaxSpeed( EVehicleClass veh_class );
Acceleration_t VehicleMaxDeceleration( EVehicleClass veh_class );
Acceleration_t VehicleNormalDeceleration( EVehicleClass veh_class );
Acceleration_t VehicleMaxAcceleration( EVehicleClass veh_class, Speed_t speed, Speed_t& max_speed_for_this_acc );
