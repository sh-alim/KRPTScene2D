//########################################################################################################################
//#
//########################################################################################################################

#pragma once

//########################################################################################################################
//#
//########################################################################################################################

#include "KRPTSceneItem.h"
#include <QElapsedTimer>

//########################################################################################################################
//#
//########################################################################################################################

class KRPTSceneScrolledAreaItem : public KRPTSceneItem
{
friend class KRPTSceneScrolledAreaCanvasItem;
public:
    enum class AreaSizePolicy
    {
        None,
        AutoSize,
        AutoPosSize,
    };
    enum class ScrollPolicy : uint8_t
    {
        No             = 0x00,
        Vertical       = 0x01,
        Horisontal     = 0x02,
        Mouse          = 0x04,
        Whell          = 0x08,
        MouseChildOver = 0x10,
    };
    using FScrollPolicy = KRPTFlag<ScrollPolicy>;
    template<typename ... Args> inline void upScrollPolicy(Args&& ... args)                                    noexcept
    {
        _scrollPolicy.up(std::forward<Args>(args)...);
    }
    template<typename ... Args> inline void downScrollPolicy(Args&& ... args)                                  noexcept
    {
        _scrollPolicy.down(std::forward<Args>(args)...);
    }
public:
    KRPTSceneScrolledAreaItem             (KRPTScene *scene, KRPTSceneItem *parent, 
                                           const QRectF &geometry = QRectF(0, 0, 100, 100))                     noexcept;
    ~KRPTSceneScrolledAreaItem            ()                                                                    noexcept;
public:
    const List   & areaChildItems         ()                                                              const noexcept;
    QPointF        areaPos                ()                                                              const noexcept;
    double         areaX                  ()                                                              const noexcept;
    double         areaY                  ()                                                              const noexcept;
    QSizeF         areaSize               ()                                                              const noexcept;
    double         areaWidth              ()                                                              const noexcept;
    double         areaHeight             ()                                                              const noexcept;
    AreaSizePolicy areaSizePolicy         ()                                                              const noexcept;
    QPointF        margin                 ()                                                              const noexcept;

    bool           setAreaGeometry        (const QRectF &geometry, 
                                           uint32_t time = 0, QEasingCurve curve = QEasingCurve::OutExpo)       noexcept;
    bool           setAreaGeometry        (const QPointF &p, const QSizeF &size, 
                                           uint32_t time = 0, QEasingCurve curve = QEasingCurve::OutExpo)       noexcept;
    bool           setAreaGeometry        (double x, double y, double w, double h, 
                                           uint32_t time = 0, QEasingCurve curve = QEasingCurve::OutExpo)       noexcept;

    void           setAreaPos             (const QPointF &p, 
                                           uint32_t time = 0, QEasingCurve curve = QEasingCurve::OutExpo)       noexcept;
    void           setAreaPos             (double x, double y, 
                                           uint32_t time = 0, QEasingCurve curve = QEasingCurve::OutExpo)       noexcept;
    void           setAreaX               (double x, 
                                           uint32_t time = 0, QEasingCurve curve = QEasingCurve::OutExpo)       noexcept;
    void           setAreaY               (double y, 
                                           uint32_t time = 0, QEasingCurve curve = QEasingCurve::OutExpo)       noexcept;
    void           setAreaSize            (const QSizeF &size, 
                                           uint32_t time = 0, QEasingCurve curve = QEasingCurve::OutExpo)       noexcept;
    void           setAreaSize            (double w, double h, 
                                           uint32_t time = 0, QEasingCurve curve = QEasingCurve::OutExpo)       noexcept;
    void           setAreaWidth           (double w, 
                                           uint32_t time = 0, QEasingCurve curve = QEasingCurve::OutExpo)       noexcept;
    void           setAreaHeight          (double h, 
                                           uint32_t time = 0, QEasingCurve curve = QEasingCurve::OutExpo)       noexcept;
    void           translateArea          (const QPointF &p, 
                                           uint32_t time = 0, QEasingCurve curve = QEasingCurve::OutExpo)       noexcept;
    void           translateArea          (double x, double y, 
                                           uint32_t time = 0, QEasingCurve curve = QEasingCurve::OutExpo)       noexcept;
    void           setAreaSizePolicy      (AreaSizePolicy policy)                                               noexcept;
    void           setMargin              (const QPointF &margin)                                               noexcept;
public:
    QPointF        mapToArea              (const QPointF &point)                                                noexcept;
    QPointF        mapFromArea            (const QPointF &point)                                                noexcept;
protected:
    void           paintBackground        (QPainter &painter, uint32_t stage)                                   noexcept override;
    void           paintForeground        (QPainter &painter, uint32_t stage)                                   noexcept override;
protected:
    void           outlineImpl            ()                                                                    noexcept override;
    void           transformImpl          (SceneTransformEvent *e)                                              noexcept override;
    void           childTransformEvent    (KRPTSceneItem::Ptr item, 
                                           SceneTransformEvent *e)                                              noexcept override;
    void           addChildImpl           (KRPTSceneItem::Ptr item, KRPTSceneItem::Ptr parent)                  noexcept override;
    void           mousePressImpl         (SceneMouseEvent *e)                                                  noexcept override;
    void           mouseReleaseImpl       (SceneMouseEvent *e)                                                  noexcept override;
    void           mouseMoveImpl          (SceneMouseEvent *e)                                                  noexcept override;
    void           whellImpl              (SceneMouseEvent *e)                                                  noexcept override;
    void           animImpl               (uint32_t id, const std::vector<double> &value, 
                                           uint32_t time, bool completed, int loop)                             noexcept override;
protected:
    virtual void   areaChildTransformEvent   (KRPTSceneItem::Ptr item, SceneTransformEvent *e)                  noexcept; 
    virtual void   areaChildMousePressEvent  (KRPTSceneItem::Ptr item, SceneMouseEvent *e)                      noexcept;
    virtual void   areaChildMouseReleaseEvent(KRPTSceneItem::Ptr item, SceneMouseEvent *e)                      noexcept;
    virtual void   areaChildMouseMoveEvent   (KRPTSceneItem::Ptr item, SceneMouseEvent *e)                      noexcept;
    virtual void   areaChildMouseOutEvent    (KRPTSceneItem::Ptr item, 
                                              KRPTSceneItem::Ptr newItem, SceneMouseEvent *e)                   noexcept;
private:
    void           updateAreaRect         ()                                                                    noexcept;
    void           resetAreaMinMax        ()                                                                    noexcept;
    void           updateAreaMinMax       (KRPTSceneItem::Ptr item = nullptr)                                   noexcept;
private:
    struct
    {
        QRectF rect;
        double sin       = 0;
        double cos       = 0;
        bool   dirtyArea = true;
        bool   dirtySize = true;
    }_trans;
    struct AreaMinMax
    {
        KRPTSceneItem::Ptr item  = nullptr;
        double             value = 0      ;
    };
    std::array<AreaMinMax, 4> _areaMinMax;
protected:
    KRPTSceneScrolledAreaCanvasItem *_area              ;
    QRectF                           _areaRect          ;
    QPointF                          _mousePos          ;
    AreaSizePolicy                   _areaSizePolicy    ;
    FScrollPolicy                    _scrollPolicy      ;
    QElapsedTimer                    _mouseVelocityTimer;
    QPointF                          _mouseVelocity     ;
    double                           _mouseMoveDistance ;
    QPointF                          _margin            ;


    double  _radius = 8;
};
