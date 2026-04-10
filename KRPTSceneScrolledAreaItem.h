//########################################################################################################################
//#
//########################################################################################################################

#pragma once

//########################################################################################################################
//#
//########################################################################################################################

#include "KRPTSceneItem.h"

//########################################################################################################################
//#
//########################################################################################################################

class KRPTSceneScrolledAreaItem : public KRPTSceneItem
{
friend class KRPTSceneAreaItem;
public:
    KRPTSceneScrolledAreaItem(KRPTScene *scene, KRPTSceneItem *parent) noexcept;
    ~KRPTSceneScrolledAreaItem()                                       noexcept;
public:
    QPointF areaPos         ()                                  const noexcept;
    double  areaX           ()                                  const noexcept;
    double  areaY           ()                                  const noexcept;
    QSizeF  areaSize        ()                                  const noexcept;
    double  areaWidth       ()                                  const noexcept;
    double  areaHeight      ()                                  const noexcept;

    void setAreaGeometry(const QRectF &geometry, 
                         uint32_t time = 0, QEasingCurve curve = QEasingCurve::OutExpo) noexcept;

    void    setAreaPos      (const QPointF &p)                        noexcept;
    void    setAreaPos      (double x, double y)                      noexcept;
    void    setAreaX        (double x)                                noexcept;
    void    setAreaY        (double y)                                noexcept;
    void    setAreaSize     (const QSizeF &s)                         noexcept;
    void    setAreaSize     (double w, double h)                      noexcept;
    void    setAreaWidth    (double w)                                noexcept;
    void    setAreaHeight   (double h)                                noexcept;


    void translateArea(const QPointF &p) noexcept;
    void translateArea(double x, double y) noexcept;

public:
    QPointF mapToArea       (const QPointF &point)                    noexcept;
    QPointF mapFromArea     (const QPointF &point)                    noexcept;
protected:
    void paintBackground    (QPainter &painter, uint32_t stage)       noexcept override;
    void paintForeground    (QPainter &painter, uint32_t stage)       noexcept override;
protected:
    void outlineImpl        ()                                        noexcept override;
    void transformImpl      (SceneTransformEvent *e)                  noexcept override;
    void childTransformEvent(KRPTSceneItem::Ptr item, 
                             SceneTransformEvent *e)                  noexcept override;
    void addChildImpl       (KRPTSceneItem::Ptr item, 
                             KRPTSceneItem::Ptr parent)               noexcept override;
    void mousePressImpl     (SceneMouseEvent *e)                      noexcept override;
    void mouseReleaseImpl   (SceneMouseEvent *e)                      noexcept override;
    void mouseMoveImpl      (SceneMouseEvent *e)                      noexcept override;

private:
    struct
    {
//        QRectF tr[2];
        double sin       = 0;
        double cos       = 0;
        QRectF rect;

        bool   dirtyAreaTrans = true;
        bool   dirtyAreaSize  = true;
        bool   dirtySize      = true;

    }_trans;
//    QRectF checkAreaRect(const QRectF &rect) noexcept;
    bool checkAreaRect(const QRectF &src, QRectF &dst, QPointF &dp) noexcept;


    void updateAreaRect() noexcept;
protected:
    KRPTSceneAreaItem *_area               ;
    QRectF             _areaRect           ;
    QPointF            _areaMousePressedPos;


    QRectF             _r0;
    QRectF             _r1;
    QPolygonF _p0;
};
