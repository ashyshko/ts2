#pragma once

bool AxisRectanglesIntersect( int l0, int t0, int r0, int b0, int l1, int t1, int r1, int b1 )
{
    assert( r0 >= l0 && b0 >= t0 );
    assert( r1 >= l1 && b1 >= t1 );
    return std::max( l0, l1 ) <= std::max( r0, r1 ) && std::max( t0, t1 ) <= std::max( b0, b1 );
}

bool RectanglesIntersect( int x0, int y0, int x1, int y1, int x2, int y2, int x3, int y3,
    int x4, int y4, int x5, int y5, int x6, int y6, int x7, int y7 )
{
    return AxisRectanglesIntersect( std::min( std::min( x0, x1 ), std::min( x2, x3 ) ), std::min( std::min( y0, y1 ), std::min( y2, y3 ) ),
        std::max( std::max( x0, x1 ), std::max( x2, x3 ) ), std::max( std::max( y0, y1 ), std::max( y2, y3 ) ),
        std::min( std::min( x4, x5 ), std::min( x6, x7 ) ), std::min( std::min( y4, y5 ), std::min( y6, y7 ) ),
        std::max( std::max( x4, x5 ), std::max( x6, x7 ) ), std::max( std::max( y4, y5 ), std::max( y6, y7 ) ) );
}