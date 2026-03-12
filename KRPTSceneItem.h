//####################################################################################################
//#
//####################################################################################################

#pragma once

#include <cstdint>
#include <list>
#include <QRectF>
#include <QPainterPath>
#include <QTransform>
#include <QPainter>
#include <QColor>
#include <QEasingCurve>
#include "KRPTSceneEvent.h"

//####################################################################################################
//#
//####################################################################################################

class KRPTScene          ;
class SceneMouseEvent    ;
class SceneTransformEvent;
class KRPTSceneItemData  ;

//####################################################################################################
//#
//####################################################################################################

class KRPTSceneItem
{
friend class KRPTScene        ;
friend class KRPTSceneItemData;
protected:
    enum class Dirty : uint16_t
    {
        No                      = 0x0000,
        Transform               = 0x0001,
        TransformTrans          = 0x0002,
        TransformScale          = 0x0004,
        TransformRotate         = 0x0008,
        TransformInv            = 0x0010,
        SceneTransformInv       = 0x0020,
        BBox                    = 0x0040,
        BBoxMapToParent         = 0x0080,
        VisibleChildItems       = 0x0100,
        Outline                 = 0x0200,
        All                     = 0xFFFF,
    };
    enum class State : uint8_t
    {
        No                      = 0x00,
        VisibledInView          = 0x01,
        NeedPaint               = 0x02,
        NeedChildPaint          = 0x04,
        All                     = 0xFF
    };
    enum AnimDst : uint8_t
    {
        Geometry = 0,
        Angle    = 1,
        Scale    = 2,
        Opaq     = 3,
        User     = 4
    };
public:
    enum class Must : uint32_t
    {
        No                      = 0x000000,
        NoPaint                 = 0x000001,
        NoClipChilds            = 0x000002,
        NoClipPainter           = 0x000004,
        NoSceneScale            = 0x000008,
        NoSceneRotate           = 0x000010,
        NoMouseEventTranslate   = 0x000020,
        NoCheckChildVisibled    = 0x000040,
        MousePressEvent         = 0x000080,
        MouseReleaseEvent       = 0x000100,
        MouseMoveEvent          = 0x000200,
        TransformEvent          = 0x000400,
        WhellEvent              = 0x000800,
        ChildTransformEvent     = 0x001000,
        Anim                    = 0x002000,
        AccuracyCheckContains   = 0x004000,
        AccuracyClip            = 0x008000,
        MouseMoveble            = 0x010000,
        All                     = 0xFFFFFF
    };
    enum class TransSrc : uint8_t{Self, Parent, Scene};
public:
    using Ptr       = KRPTSceneItem*;
    using ItemsList = std::list<KRPTSceneItem::Ptr>;
    using IndexMap  = std::map<KRPTSceneItem*, ItemsList::iterator>;
public:
    KRPTSceneItem(KRPTScene *scene, KRPTSceneItem *parent)                                                         noexcept;
    virtual ~KRPTSceneItem()                                                                                       noexcept;
public:
    template<typename T, typename ... Args> inline auto addChild(Args&& ... arg)                                   noexcept
    {
        static_assert(std::is_base_of_v<KRPTSceneItem, T>, "is not scene item");
        auto item = new T(_scene, this, std::forward<Args>(arg) ...);
        addChildImpl(item, this);
        return item;
    }
    bool delChild(KRPTSceneItem *item)                                                                             noexcept;
    template<typename ... Args> inline bool must(Args&& ... args)                                            const noexcept
    {
        return _must.upped(std::forward<Args>(args)...);
    }
    template<typename ... Args> inline void addMust(Args&& ... args)                                               noexcept
    {
        _must.up(std::forward<Args>(args)...);
    }
public:
    KRPTFlag<Must>       must                ()                                                              const noexcept;
    KRPTScene          * scene               ()                                                              const noexcept;
    KRPTSceneItem      * parent              ()                                                              const noexcept;
    const ItemsList    & childItems          ()                                                              const noexcept;
    const ItemsList    & visibleChildItems   ()                                                                    noexcept;
    bool                 visible             ()                                                              const noexcept;
    const QRectF       & geometry            ()                                                              const noexcept;
    const QRectF       & rect                ()                                                              const noexcept;
    QPointF              pos                 ()                                                              const noexcept;
    QSizeF               size                ()                                                              const noexcept;
    double               x                   ()                                                              const noexcept;
    double               y                   ()                                                              const noexcept;
    double               width               ()                                                              const noexcept;
    double               height              ()                                                              const noexcept;
    double               right               ()                                                              const noexcept;
    double               bottom              ()                                                              const noexcept;
    QPointF              center              ()                                                              const noexcept;
    double               angle               ()                                                              const noexcept;
    double               scale               ()                                                              const noexcept;
    double               opaq                ()                                                              const noexcept;
    const QTransform   & transform           ()                                                                    noexcept;
    const QTransform   & transformInv        ()                                                                    noexcept;
    const QTransform   & sceneTransform      ()                                                                    noexcept;
    const QTransform   & sceneTransformInv   ()                                                                    noexcept;
    const QPainterPath & outline             ()                                                                    noexcept;
    QRectF               bBox                ()                                                                    noexcept;
    QRectF               bBoxMapToParent     ()                                                                    noexcept;
    bool                 contains            (const QPointF &point)                                                noexcept;
    uint32_t             tag                 ()                                                              const noexcept;

    QColor               borderColor         ()                                                              const noexcept {return _borderColor       ;}
    QColor               backgroundColor     ()                                                              const noexcept {return _backgroundColor   ;}

    void                 setVisible          (bool visible)                                                        noexcept;
    bool                 setGeometry         (const QRectF &geometry, 
                                              uint32_t time = 0, QEasingCurve curve = QEasingCurve::OutExpo)       noexcept;
    bool                 setGeometry         (const QPointF &pos, const QSizeF &size, 
                                              uint32_t time = 0, QEasingCurve curve = QEasingCurve::OutExpo)       noexcept;
    bool                 setGeometry         (double x, double y, double w, double h, 
                                              uint32_t time = 0, QEasingCurve curve = QEasingCurve::OutExpo)       noexcept;
    void                 setPos              (const QPointF &pos, 
                                              uint32_t time = 0, QEasingCurve curve = QEasingCurve::OutExpo)       noexcept;
    void                 setPos              (double x, double y, 
                                              uint32_t time = 0, QEasingCurve curve = QEasingCurve::OutExpo)       noexcept;
    void                 setSize             (const QSizeF &size, 
                                              uint32_t time = 0, QEasingCurve curve = QEasingCurve::OutExpo)       noexcept;
    void                 setSize             (double w, double h, 
                                              uint32_t time = 0, QEasingCurve curve = QEasingCurve::OutExpo)       noexcept;
    void                 setX                (double x, 
                                              uint32_t time = 0, QEasingCurve curve = QEasingCurve::OutExpo)       noexcept;
    void                 setY                (double y, 
                                              uint32_t time = 0, QEasingCurve curve = QEasingCurve::OutExpo)       noexcept;
    void                 setWidth            (double w, 
                                              uint32_t time = 0, QEasingCurve curve = QEasingCurve::OutExpo)       noexcept;
    void                 setHeight           (double h, 
                                              uint32_t time = 0, QEasingCurve curve = QEasingCurve::OutExpo)       noexcept;
    bool                 setAngle            (double angle, 
                                              uint32_t time = 0, QEasingCurve curve = QEasingCurve::OutExpo)       noexcept;
    bool                 setScale            (double scale, 
                                              uint32_t time = 0, QEasingCurve curve = QEasingCurve::OutExpo)       noexcept;
    bool                 setOpaq             (double opaq, 
                                              uint32_t time = 0, QEasingCurve curve = QEasingCurve::OutExpo)       noexcept;
    void                 translate           (const QPointF &pos, 
                                              uint32_t time = 0, QEasingCurve curve = QEasingCurve::OutExpo)       noexcept;
    void                 translate           (double dx, double dy, 
                                              uint32_t time = 0, QEasingCurve curve = QEasingCurve::OutExpo)       noexcept;
    void                 rotate              (double angle, 
                                              uint32_t time = 0, QEasingCurve curve = QEasingCurve::OutExpo)       noexcept;
    void                 rotateAround        (double angle, const QPointF &pt, TransSrc src, 
                                              uint32_t time = 0, QEasingCurve curve = QEasingCurve::OutExpo)       noexcept;
    void                 scaleMul            (double scale, 
                                              uint32_t time = 0, QEasingCurve curve = QEasingCurve::OutExpo)       noexcept;
    void                 scaleFromPoint      (double scale, const QPointF &pt, TransSrc src, 
                                              uint32_t time = 0, QEasingCurve curve = QEasingCurve::OutExpo)       noexcept;
    void                 setTag              (uint32_t tag)                                                        noexcept;
    void                 setBorderColor      (const QColor &color)                                                 noexcept;
    void                 setBackgroundColor  (const QColor &color)                                                 noexcept;
    void                 lockUpdate          (bool lock)                                                           noexcept;
    QPointF              mapToParent         (const QPointF &point)                                                noexcept;
    QPolygonF            mapToParent         (const QRectF &rect)                                                  noexcept;
    QPolygonF            mapToParent         (const QPolygonF &polygon)                                            noexcept;
    QPointF              mapFromParent       (const QPointF &point)                                                noexcept;
    QPolygonF            mapFromParent       (const QRectF &rect)                                                  noexcept;
    QPolygonF            mapFromParent       (const QPolygonF &polygon)                                            noexcept;
    QPointF              mapToScene          (const QPointF &point)                                                noexcept;
    QPolygonF            mapToScene          (const QRectF &rect)                                                  noexcept;
    QPolygonF            mapToScene          (const QPolygonF &polygon)                                            noexcept;
    QPointF              mapFromScene        (const QPointF &point)                                                noexcept;
    QPolygonF            mapFromScene        (const QRectF &rect)                                                  noexcept;
    QPolygonF            mapFromScene        (const QPolygonF &polygon)                                            noexcept;
    QPointF              mapToItem           (KRPTSceneItem *item, const QPointF &point)                           noexcept;
    QPolygonF            mapToItem           (KRPTSceneItem *item, const QRectF &rect)                             noexcept;
    QPolygonF            mapToItem           (KRPTSceneItem *item, const QPolygonF &polygon)                       noexcept;
    QPointF              mapFromItem         (KRPTSceneItem *item, const QPointF &point)                           noexcept;
    QPolygonF            mapFromItem         (KRPTSceneItem *item, const QRectF &rect)                             noexcept;
    QPolygonF            mapFromItem         (KRPTSceneItem *item, const QPolygonF &polygon)                       noexcept;
    bool                 needPaint           ()                                                              const noexcept;
    bool                 needChildPaint      ()                                                              const noexcept;
protected:
    virtual void         addChildEvent       (KRPTSceneItem *item)                                                 noexcept;
    virtual void         delChildEvent       (KRPTSceneItem *item)                                                 noexcept;
    virtual void         transformEvent      (SceneTransformEvent *e)                                              noexcept;
    virtual void         mousePressEvent     (SceneMouseEvent *e)                                                  noexcept;
    virtual void         mouseReleaseEvent   (SceneMouseEvent *e)                                                  noexcept;
    virtual void         mouseMoveEvent      (SceneMouseEvent *e)                                                  noexcept;
    virtual void         whellEvent          (SceneMouseEvent *e)                                                  noexcept;
    virtual void         childTransformEvent (KRPTSceneItem *item, SceneTransformEvent *e)                         noexcept;
protected:
    virtual void         update              ()                                                                    noexcept;
    virtual void         addChildImpl        (KRPTSceneItem::Ptr item, KRPTSceneItem *parent)                      noexcept;
    virtual bool         delChildImpl        (KRPTSceneItem *item, KRPTSceneItem *parent)                          noexcept;
    virtual bool         setGeometryImpl     (const QRectF &geometry)                                              noexcept;
    virtual bool         setAngleImpl        (double angle)                                                        noexcept;
    virtual bool         setScaleImpl        (double scale)                                                        noexcept;
    virtual bool         setOpaqImpl         (double opaq)                                                         noexcept;
    virtual void         transformImpl       (SceneTransformEvent *e)                                              noexcept;
    virtual void         outlineImpl         ()                                                                    noexcept;
    virtual void         mousePressImpl      (SceneMouseEvent *e)                                                  noexcept;
    virtual void         mouseReleaseImpl    (SceneMouseEvent *e)                                                  noexcept;
    virtual void         mouseMoveImpl       (SceneMouseEvent *e)                                                  noexcept;
    virtual void         whellImpl           (SceneMouseEvent *e)                                                  noexcept;
    virtual void         animImpl            (uint32_t id, const std::vector<double> &value, 
                                              uint32_t time, bool completed, int loop)                             noexcept;
    virtual void         paintBackground     (QPainter &painter)                                                   noexcept;
    virtual void         paintForeground     (QPainter &painter)                                                   noexcept;
    virtual void         updateGeometry      ()                                                                    noexcept;
protected:
    void                 transform           (const QRectF &rect, double angle, 
                                              double scale, QTransform &transform)                                 noexcept;
    QPointF              transformShift      (QTransform &transform, TransSrc src, const QPointF &pt)              noexcept;
    void                 bBox                (const QTransform &transform, const QRectF &rect, QRectF &bBox)       noexcept;
    QRectF               bBox                (const QTransform &transform, const QRectF &rect)                     noexcept;
    bool                 updateCache         (bool visible = false)                                                noexcept;
    bool                 dirtyTransform      ()                                                                    noexcept;
    bool                 dirtyVisibleChilds  ()                                                                    noexcept;
    bool                 mustAnim            (uint32_t time)                                                 const noexcept;
protected:
    void                 startAnimImpl       (uint32_t id, 
                                              const std::vector<double> &start, const std::vector<double> &end, 
                                              uint32_t time, QEasingCurve curve, int loopCount = 1)                noexcept;
    void                 startAnimImpl       (uint32_t id, double start, double end, 
                                              uint32_t time, QEasingCurve curve, int loopCount = 1)                noexcept;
    void                 startAnimImpl       (uint32_t id, const QPointF &start, const QPointF &end, 
                                              uint32_t time, QEasingCurve curve, int loopCount = 1)                noexcept;
    void                 startAnimImpl       (uint32_t id, const QRectF &start, const QRectF &end, 
                                              uint32_t time, QEasingCurve curve, int loopCount = 1)                noexcept;
    void                 startAnimImpl       (uint32_t id, const QSizeF &start, const QSizeF &end, 
                                              uint32_t time, QEasingCurve curve, int loopCount = 1)                noexcept;
    void                 startAnimImpl       (uint32_t id, const QColor &start, const QColor &end, 
                                              uint32_t time, QEasingCurve curve, int loopCount = 1)                noexcept;
protected:
    KRPTSceneItemData *_data;
    KRPTFlag<Dirty>    _dirty            ;
    KRPTFlag<Must>     _must             ;
    KRPTFlag<State>    _state            ;
    KRPTScene         *_scene            ;
    KRPTSceneItem     *_parent           ;
    ItemsList          _childItems       ;
    ItemsList          _visibleChildItems;
    IndexMap           _index            ;
    uint32_t           _updateLocked     ; 
    bool               _visible          ;
    QRectF             _geometry         ;
    QRectF             _rect             ;
    double             _angle            ;
    double             _scale            ;
    double             _opaq             ;
    QTransform         _transform        ;
    QTransform         _transTransform   ;
    QTransform         _scaleTransform   ;
    QTransform         _rotateTransform  ;
    QTransform         _transformInv     ;
    QTransform         _sceneTransform   ;
    QTransform         _sceneTransformInv;
    QPainterPath       _outline          ;
    QRectF             _bBox             ;
    QRectF             _bBoxMapToParent  ;
    QColor             _borderColor      ;
    QColor             _backgroundColor  ;
    uint32_t           _tag              ; 
};

