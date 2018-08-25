#include "QMapWidget.h"

#include <cmath>
#include <QGestureEvent>
#include <QPaintEvent>
#include <QPainter>
#include <QDebug>

QMapWidget::QMapWidget(QWidget *parent) : QWidget(parent), m_map(nullptr), m_current_pinch_zoom(1.0)
{
    this->grabGesture( Qt::PinchGesture );
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
    double zoom = m_points_per_px / m_current_pinch_zoom;
    p.scale( 1.0 / zoom, 1.0 / zoom );
    {
        int left = m_center_x - width() / 2 * zoom;
        int top = m_center_y - height() / 2 * zoom;
        p.translate( -left, -top );
    }

    p.setClipRect( QRect( 0, 0, m_map->Width(), m_map->Height() ) );

    p.setPen( QPen( Qt::red, 10000.0 ) );
    p.drawRect( QRect( 0, 0, m_map->Width(), m_map->Height() ) );

    p.setPen( QPen( Qt::black, 100 ) );

    for( const CMap::SRoad* road: m_map->Roads() )
    {
        for( const CMap::SLane* lane: road->straight_lanes )
        {
            QRect r;
            r.setCoords( lane->rect.x1, lane->rect.y1, lane->rect.x2, lane->rect.y2 );
            p.drawRect( r );
        }

        for( const CMap::SLane* lane: road->opposite_lanes )
        {
            QRect r;
            r.setCoords( lane->rect.x1, lane->rect.y1, lane->rect.x2, lane->rect.y2 );
            p.drawRect( r );
        }
    }

    p.setPen( QPen( Qt::blue, 50, Qt::DotLine ) );

    for( const CMap::SRoad* road: m_map->Roads() )
    {
        for( const CMap::SLane* lane: road->straight_lanes )
        {
            p.drawLine( lane->from_x, lane->from_y, lane->to_x, lane->to_y );
        }

        for( const CMap::SLane* lane: road->opposite_lanes )
        {
            p.drawLine( lane->from_x, lane->from_y, lane->to_x, lane->to_y );
        }
    }

    for( const CMap::SCross* cross: m_map->Crosses() )
    {
        for( const CMap::SConnection* con: cross->connections )
        {
            p.drawLine( con->in->to_x, con->in->to_y, con->out->from_x, con->out->from_y );
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
        m_center_x -= s_movement_px * m_points_per_px;
        this->update();
        break;
    case Qt::Key_D:
    case Qt::Key_Right:
        m_center_x += s_movement_px * m_points_per_px;
        this->update();
        break;
    case Qt::Key_W:
    case Qt::Key_Up:
        m_center_y -= s_movement_px * m_points_per_px;
        this->update();
        break;
    case Qt::Key_S:
    case Qt::Key_Down:
        m_center_y += s_movement_px * m_points_per_px;
        this->update();
        break;

    }
}


void QMapWidget::wheelEvent(QWheelEvent *event)
{
    event->accept();

    if( !event->pixelDelta().isNull() )
    {
        m_center_x -= event->pixelDelta().x() * m_points_per_px;
        m_center_y -= event->pixelDelta().y() * m_points_per_px;
        this->update();
        return;
    }
}


bool QMapWidget::event(QEvent *event)
{
    if( event->type() == QEvent::Gesture )
    {
        QGestureEvent* gesture_ev = static_cast<QGestureEvent*>(event);
        if( QPinchGesture* g = static_cast<QPinchGesture*>( gesture_ev->gesture(Qt::PinchGesture) ) )
        {
            if( g->changeFlags() & QPinchGesture::ScaleFactorChanged )
            {
                m_current_pinch_zoom = g->totalScaleFactor();
                this->update();
            }
            if( g->state() == Qt::GestureFinished )
            {
                m_points_per_px /= m_current_pinch_zoom;
                m_current_pinch_zoom = 1.0;
                this->update();
            }
        }

        return true;
    }

    return QWidget::event(event);
}
