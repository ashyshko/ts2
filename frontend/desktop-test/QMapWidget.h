#ifndef QMAPWIDGET_H
#define QMAPWIDGET_H

#include <QWidget>
#include <tl/CMap.h>

class QMapWidget : public QWidget
{
    Q_OBJECT
public:
    explicit QMapWidget(QWidget *parent = nullptr);

    void SetMap( CMap* map );

signals:

public slots:

private:
    static int const s_border_px = 10;
    CMap* m_map;
    double m_points_per_px;
    double m_current_pinch_zoom;
    double m_default_points_per_px;
    Point_t m_center_x;
    Point_t m_center_y;

    // QWidget interface
protected:
    virtual void paintEvent(QPaintEvent *event) override;
    virtual void keyPressEvent(QKeyEvent *event) override;
    virtual void wheelEvent(QWheelEvent *event) override;
    virtual bool event(QEvent *event) override;
};

#endif // QMAPWIDGET_H
