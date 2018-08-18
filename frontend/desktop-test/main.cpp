#include <iostream>

#include <CMap.h>


int main()
{
    CMap m( PointFromM( 4000 ), PointFromM(4000) );
    m.AddRoad( PointFromM( 2000 ), PointFromM(0), PointFromM(4000), CMap::Direction_Right, 2, 2 );
    m.AddRoad( PointFromM( 0 ), PointFromM(2000), PointFromM(4000), CMap::Direction_Bottom, 2, 2 );
}
