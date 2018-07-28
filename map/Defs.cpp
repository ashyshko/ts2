#include <Defs.h>

WayPointId_t s_invalid_point_id = WayPointId_t(0);
PathId_t s_invalid_path_id = PathId_t(0);
JunctionId_t s_invalid_junction_id = JunctionId_t(0);
VehicleId_t s_invalid_vehicle_id = VehicleId_t(0);

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
                        /*front wheel*/ DistanceFromMm(2624, /*rear wheel*/ DistanceFromMm(8124) ),
                        /*has_trailer*/ true, /*trail_joint_point*/ DistanceFromMm(10524), /*trail_width*/ DistanceFromMm(2550),
                        /*trail_offset*/ DistanceFromMm(10524), /*trail_length*/ DistanceFromMm(7466), 
                        /*trail_wheel_offset*/ DistanceFromMm(15524)
};

Speed_t MaxSpeed = FromKmphF(120.0);
int SpeedRangesCount = 6;
