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
    KRPTSceneScrolledAreaItem(KRPTScene *scene, KRPTSceneItem *parent)                            noexcept;
    ~KRPTSceneScrolledAreaItem()                                                                  noexcept;
public:
    QPointF areaPos         ()                                                              const noexcept;
    double  areaX           ()                                                              const noexcept;
    double  areaY           ()                                                              const noexcept;
    QSizeF  areaSize        ()                                                              const noexcept;
    double  areaWidth       ()                                                              const noexcept;
    double  areaHeight      ()                                                              const noexcept;

    bool    setAreaGeometry (const QRectF &geometry, 
                             uint32_t time = 0, QEasingCurve curve = QEasingCurve::OutExpo)       noexcept;
    bool    setAreaGeometry (const QPointF &p, const QSizeF &size, 
                             uint32_t time = 0, QEasingCurve curve = QEasingCurve::OutExpo)       noexcept;
    bool    setAreaGeometry (double x, double y, double w, double h, 
                             uint32_t time = 0, QEasingCurve curve = QEasingCurve::OutExpo)       noexcept;


    void    setAreaPos      (const QPointF &p, 
                             uint32_t time = 0, QEasingCurve curve = QEasingCurve::OutExpo)       noexcept;
    void    setAreaPos      (double x, double y, 
                             uint32_t time = 0, QEasingCurve curve = QEasingCurve::OutExpo)       noexcept;
    void    setAreaX        (double x, 
                             uint32_t time = 0, QEasingCurve curve = QEasingCurve::OutExpo)       noexcept;
    void    setAreaY        (double y, 
                             uint32_t time = 0, QEasingCurve curve = QEasingCurve::OutExpo)       noexcept;
    void    setAreaSize     (const QSizeF &size, 
                             uint32_t time = 0, QEasingCurve curve = QEasingCurve::OutExpo)       noexcept;
    void    setAreaSize     (double w, double h, 
                             uint32_t time = 0, QEasingCurve curve = QEasingCurve::OutExpo)       noexcept;
    void    setAreaWidth    (double w, 
                             uint32_t time = 0, QEasingCurve curve = QEasingCurve::OutExpo)       noexcept;
    void    setAreaHeight   (double h, 
                             uint32_t time = 0, QEasingCurve curve = QEasingCurve::OutExpo)       noexcept;
    void    translateArea   (const QPointF &p, 
                             uint32_t time = 0, QEasingCurve curve = QEasingCurve::OutExpo)       noexcept;
    void    translateArea   (double x, double y, 
                             uint32_t time = 0, QEasingCurve curve = QEasingCurve::OutExpo)       noexcept;

public:
    QPointF mapToArea       (const QPointF &point)                                                noexcept;
    QPointF mapFromArea     (const QPointF &point)                                                noexcept;
protected:
    void paintBackground    (QPainter &painter, uint32_t stage)                                   noexcept override;
    void paintForeground    (QPainter &painter, uint32_t stage)                                   noexcept override;
protected:
    void outlineImpl        ()                                                                    noexcept override;
    void transformImpl      (SceneTransformEvent *e)                                              noexcept override;
    void childTransformEvent(KRPTSceneItem::Ptr item, 
                             SceneTransformEvent *e)                                              noexcept override;
    void addChildImpl       (KRPTSceneItem::Ptr item, 
                             KRPTSceneItem::Ptr parent)                                           noexcept override;
    void mousePressImpl     (SceneMouseEvent *e)                                                  noexcept override;
    void mouseReleaseImpl   (SceneMouseEvent *e)                                                  noexcept override;
    void mouseMoveImpl      (SceneMouseEvent *e)                                                  noexcept override;
    void whellImpl          (SceneMouseEvent *e)                                                  noexcept override;

private:
    struct
    {
        QRectF rect;
        double sin       = 0;
        double cos       = 0;
        bool   dirtyArea = true;
        bool   dirtySize = true;
    }_trans;
    bool checkAreaRect(const QRectF &src, QRectF &dst, QPointF &dp) noexcept;
    void updateAreaRect() noexcept;
protected:
    KRPTSceneAreaItem *_area               ;
    QRectF             _areaRect           ;
    QPointF            _areaMousePos;

    double  _radius = 20;

    QRectF             _r0;
    QRectF             _r1;
    QPolygonF _p0;
};
