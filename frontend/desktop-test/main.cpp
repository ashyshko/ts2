#include <iostream>

#include <CMap.h>
#include <QApplication>
#include <QMapWidget.h>


int main( int argc, char** argv )
{
    CMap m( PointFromM( 4000 ), PointFromM(4000) );
    m.AddRoad( PointFromM( 2000 ), PointFromM(0), PointFromM(4000), CMap::Direction_Bottom, 3, 3 );
    m.AddRoad( PointFromM( 0 ), PointFromM(2000), PointFromM(4000), CMap::Direction_Right, 3, 3 );
    m.AddRoad( PointFromM( 1000 ), PointFromM(1000), PointFromM(2000), CMap::Direction_Right, 2, 2 );
    m.AddRoad( PointFromM( 1000 ), PointFromM(1000), PointFromM(2000), CMap::Direction_Bottom, 2, 2 );


    QApplication app( argc, argv );
    QMapWidget widget;
    widget.show();
    widget.SetMap(&m);
    return app.exec();
}
