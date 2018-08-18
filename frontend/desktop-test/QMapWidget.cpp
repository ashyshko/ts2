#include "QMapWidget.h"

#include <QPaintEvent>
#include <QPainter>

QMapWidget::QMapWidget(QWidget *parent) : QWidget(parent), m_map(nullptr)
{

}

void QMapWidget::SetMap( CMap* map )
{
    m_map = map;

    int w = std::max( this->width() - 2 * s_border_px, 0 );
    int h = std::max( this->height() - 2 * s_border_px, 0 );

    if( (double)map->Width() / map->Height() > (double)w / h )
    {
        m_default_points_per_px = (double)map->Width() / w;
    }
    else
    {
        m_default_points_per_px = (double)map->Height() / h;
    }

    m_points_per_px = m_default_points_per_px;

    m_center_x = map->Width() / 2;
    m_center_y = map->Height() / 2;

    this->update();
}


void QMapWidget::paintEvent(QPaintEvent *event)
{
    if( m_map == nullptr )
    {
        QWidget::paintEvent(event);
        return;
    }

    QPainter p(this);
    p.scale( 1.0 / m_points_per_px, 1.0 / m_points_per_px );
    {
        int left = m_center_x - width() / 2 * m_points_per_px;
        int top = m_center_y - height() / 2 * m_points_per_px;
        p.translate( -left, -top );
    }

    p.setClipRect( QRect( 0, 0, m_map->Width(), m_map->Height() ) );

    p.setPen( QPen( Qt::red, 10000.0 ) );
    p.drawRect( QRect( 0, 0, m_map->Width(), m_map->Height() ) );

    p.setRenderHint( QPainter::HighQualityAntialiasing );

    p.setPen( QPen( Qt::black, 100 ) );

    for( const CMap::SRoad* road: m_map->Roads() )
    {
        for( const CMap::SLane* lane: road->straight_lanes )
        {
            QRect r;
            r.setCoords( lane->rect.x1, lane->rect.y1, lane->rect.x2, lane->rect.y2 );
            p.drawRect( r );
        }
    }
}


void QMapWidget::keyPressEvent(QKeyEvent *event)
{
    static const int s_movement_px = 10;

    switch( event->key() )
    {
    case Qt::Key_0:
        m_points_per_px = m_default_points_per_px;
        this->update();
        break;
    case Qt::Key_Minus:
        m_points_per_px *= 1.2;
        this->update();
        break;
    case Qt::Key_Equal:
        m_points_per_px /= 1.2;
        this->update();
        break;
    case Qt::Key_A:
    case Qt::Key_Left:
        m_center_x += s_movement_px * m_points_per_px;
        this->update();
        break;
    case Qt::Key_D:
    case Qt::Key_Right:
        m_center_x -= s_movement_px * m_points_per_px;
        this->update();
        break;
    case Qt::Key_W:
    case Qt::Key_Up:
        m_center_y += s_movement_px * m_points_per_px;
        this->update();
        break;
    case Qt::Key_S:
    case Qt::Key_Down:
        m_center_y -= s_movement_px * m_points_per_px;
        this->update();
        break;

    }
}
