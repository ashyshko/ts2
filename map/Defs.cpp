#include <Defs.h>

WayPointId_t s_invalid_wp_id = WayPointId_t(0);
PathId_t s_invalid_path_id = PathId_t(0);
JunctionId_t s_invalid_junc_id = JunctionId_t(0);
VehicleId_t s_invalid_veh_id = VehicleId_t(0);
Time_t s_time_max = std::numeric_limits<Time_t>::max();
Time_t s_time_min = std::numeric_limits<Time_t>::min();

SVehicleDimension VehicleDimensionOf( int width, int length, int front_wheel_offset, int rear_wheel_offset,
                                        bool has_trailer, int trail_joint_point, int trail_width, int trail_offset,
                                        int trail_length, int trail_wheel_offset )
{
    SVehicleDimension res;
    res.width = width;
    res.length = length;
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

SVehicleDimension s_vehicle_dimensions[VehicleClassCount] = {
    // VehicleClass_Micro - smart fortwo
    VehicleDimensionOf( /*width*/ DistanceFromMm(1888), /*length*/ DistanceFromMm(2695), 
                        /*front wheel*/ DistanceFromMm(424), /*rear wheel*/ DistanceFromMm(2297) ),

    // VehicleClass_Mini - volkswagen golf
    VehicleDimensionOf( /*width*/ DistanceFromMm(1759), /*length*/ DistanceFromMm(4204), 
                        /*front wheel*/ DistanceFromMm(880), /*rear wheel*/ DistanceFromMm(3458) ),
    
    //VehicleClass_Sedan - toyota corolla
    VehicleDimensionOf( /*width*/ DistanceFromMm(1763), /*length*/ DistanceFromMm(4572), 
                        /*front wheel*/ DistanceFromMm(986), /*rear wheel*/ DistanceFromMm(3586) ),   

    // VehicleClass_Limosine - bmw 6 series
    VehicleDimensionOf( /*width*/ DistanceFromMm(1894), /*length*/ DistanceFromMm(5007), 
                        /*front wheel*/ DistanceFromMm(941), /*rear wheel*/ DistanceFromMm(3909) ),

    // VehicleClass_Suv - mercedes gelenwagen
    VehicleDimensionOf( /*width*/ DistanceFromMm(1760), /*length*/ DistanceFromMm(4662), 
                        /*front wheel*/ DistanceFromMm(783), /*rear wheel*/ DistanceFromMm(3633) ),
    
    // VehicleClass_Minivan - volkswagen crafter
    VehicleDimensionOf( /*width*/ DistanceFromMm(1993), /*length*/ DistanceFromMm(5905), 
                        /*front wheel*/ DistanceFromMm(1000), /*rear wheel*/ DistanceFromMm(4665) ),
    
    // VehicleClass_Van - mack granit
    VehicleDimensionOf( /*width*/ DistanceFromMm(2576), /*length*/ DistanceFromMm(8216), 
                        /*front wheel*/ DistanceFromMm(1295), /*rear wheel*/ DistanceFromMm(6271) ),

    // VehicleClass_CityBus - man leon city
    VehicleDimensionOf( /*width*/ DistanceFromMm(2500), /*length*/ DistanceFromMm(10500), 
                        /*front wheel*/ DistanceFromMm(2700), /*rear wheel*/ DistanceFromMm(7095) ),
    
    // VehicleClass_Truck - volvo vnl
    VehicleDimensionOf( /*width*/ DistanceFromMm(3015), /*length*/ DistanceFromMm(8730), 
                        /*front wheel*/ DistanceFromMm(1430), /*rear wheel*/ DistanceFromMm(5730),
                        /*has_trailer*/ true, /*trail_joint_point*/ DistanceFromMm(5020), /*trail_width*/ DistanceFromMm(2591),
                        /*trail_offset*/ DistanceFromMm(4012), /*trail_length*/ DistanceFromMm(16154),
                        /*trail_wheel_offset*/ DistanceFromMm(20173) ),

    // VehicleClass_DoubleDeckBus - volvo 8500
    VehicleDimensionOf( /*width*/ DistanceFromMm(2550), /*length*/ DistanceFromMm(10524), 
                        /*front wheel*/ DistanceFromMm(2624), /*rear wheel*/ DistanceFromMm(8124),
                        /*has_trailer*/ true, /*trail_joint_point*/ DistanceFromMm(10524), /*trail_width*/ DistanceFromMm(2550),
                        /*trail_offset*/ DistanceFromMm(10524), /*trail_length*/ DistanceFromMm(7466), 
                        /*trail_wheel_offset*/ DistanceFromMm(15524) )
};

Speed_t MaxSpeed = FromKmphF(120.0);

int MaxVehicleLength()
{
    int res = 0;
    for( const auto& dimension : s_vehicle_dimensions )
    {
        int length = dimension.has_trailer ? (dimension.trail_offset + dimension.trail_length) : dimension.length;
        res = std::max( res, length );
    }
    return res;
}

Speed_t VehicleMaxSpeed( EVehicleClass veh_class )
{
    static Speed_t s_max_speed[VehicleClassCount] = {
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

    return s_max_speed[veh_class];
}

Acceleration_t VehicleMaxDeceleration( EVehicleClass veh_class )
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

Acceleration_t VehicleNormalDeceleration( EVehicleClass veh_class )
{
    return VehicleMaxDeceleration(veh_class) / 2;
}

Acceleration_t VehicleMaxAcceleration( EVehicleClass veh_class, Speed_t speed, Speed_t& max_speed_for_this_acc )
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
        /*60-80*/ FromMps2F(0.3), /*80-100*/ FromMps2F(0.2), /*100-120*/ FromMps2F(0.1) 
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