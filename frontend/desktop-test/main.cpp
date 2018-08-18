#include <iostream>

#include <CMap.h>
#include <QApplication>
#include <QMapWidget.h>


int main( int argc, char** argv )
{
    CMap m( PointFromM( 4000 ), PointFromM(4000) );
    m.AddRoad( PointFromM( 2000 ), PointFromM(0), PointFromM(4000), CMap::Direction_Bottom, 1, 1 );
    m.AddRoad( PointFromM( 0 ), PointFromM(2000), PointFromM(4000), CMap::Direction_Right, 1, 1 );

    QApplication app( argc, argv );
    QMapWidget widget;
    widget.show();
    widget.SetMap(&m);
    return app.exec();
}
