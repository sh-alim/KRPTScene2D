//########################################################################################################################
//#
//########################################################################################################################

#pragma once

//########################################################################################################################
//#
//########################################################################################################################

#include <cstdint>
#include <list>
#include <QRectF>
#include <QPainterPath>
#include <QTransform>
#include <QPainter>
#include <QColor>
#include <QEasingCurve>
#include "KRPTSceneEvent.h"

#include <QDebug>

//########################################################################################################################
//#
//########################################################################################################################

class KRPTScene           ;
class SceneMouseEvent     ;
class SceneTransformEvent ;
class KRPTSceneItemData   ;

//########################################################################################################################
//#
//########################################################################################################################

class KRPTSceneItem
{
friend class KRPTScene        ;
friend class KRPTSceneItemData;
friend class KRPTSceneColors  ;
protected:
    enum class Dirty : uint16_t
    {
        No                      = 0x0000,
        Transform               = 0x0001,
        TransformInv            = 0x0002,
        TransformTrans          = 0x0004,
        TransformSize           = 0x0008,
        TransformScale          = 0x0010,
        TransformRotate         = 0x0020,
        SceneTransform          = 0x0040,
        SceneTransformInv       = 0x0080,
        SceneScale              = 0x0100,
        SceneRotate             = 0x0200,
        BBox                    = 0x0400,
        BBoxMapToParent         = 0x0800,
        VisibleChildItems       = 0x1000,
        Outline                 = 0x2000,
        All                     = 0xFFFF
    };
    enum AnimDst : uint8_t
    {
        Geometry                = 0,
        Angle                   = 1,
        Scale                   = 2,
        Opaq                    = 3,
        Color                   = 4,
        User                    = 5
    };
public:
    enum class Must : uint64_t
    {
        No                      = 0x0000000000,
        NoPaintBackground       = 0x0000000001,
        NoPaintForeground       = 0x0000000002,
        NoPaint                 = NoPaintBackground | NoPaintForeground,
        NoClipChilds            = 0x0000000004,
        NoClipPainter           = 0x0000000008,
        NoCheckChildVisibled    = 0x0000000010,
        NoSceneScale            = 0x0000000020,
        NoSceneRotate           = 0x0000000040,
        TransformAnim           = 0x0000000080,
        ColorAnim               = 0x0000000100,
        AccuracyCheckContains   = 0x0000000200,
        AccuracyClip            = 0x0000000400,
        NoMouseEventTranslate   = 0x0000000800,
        MouseTracking           = 0x0000001000,
        StateChangeEvent        = 0x0000002000,
        MousePressEvent         = 0x0000004000,
        MouseMoveEvent          = 0x0000008000,
        MouseEnterEvent         = 0x0000010000,
        WhellEvent              = 0x0000020000,
        TransformEvent          = 0x0000040000,
        SceneTransformEvent     = 0x0000080000,
        SceneScaleEvent         = 0x0000100000,
        SceneRotateEvent        = 0x0000200000,
        TransformToParentEvent  = 0x0000400000,
        TransformToSceneEvent   = 0x0000800000,
        MousePressToParentEvent = 0x0001000000,
        MousePressToSceneEvent  = 0x0002000000,
        MouseMoveToParentEvent  = 0x0004000000,
        MouseMoveToSceneEvent   = 0x0008000000,
        WhellToParentEvent      = 0x0010000000,
        WhellToSceneEvent       = 0x0020000000,
        CheckedToParentEvent    = 0x0040000000,
        CheckedToSceneEvent     = 0x0080000000,
        Checked                 = 0x0100000000,
        MouseMoved              = 0x0200000000,
        MouseChecked            = 0x0400000000,
        All                     = 0xFFFFFFFFFF
    };
    enum class State : uint8_t
    {
        No                      = 0x00,
        VisibledInView          = 0x01,
        NeedPaint               = 0x02,
        NeedChildPaint          = 0x04,
        MouseOver               = 0x08,
        MousePressed            = 0x10,
        Checked                 = 0x20,
        ChildMouseOver          = 0x40,
        All                     = 0xFF
    };
    enum class TransSrc : uint8_t{Self, Parent, Scene};
    enum class TransformAnchor
    {
        Center      ,
        LeftTop     ,
        RightTop    ,
        LeftBottom  ,
        RightBottom ,
        LeftCenter  ,
        TopCenter   ,
        RightCenter ,
        BottomCenter
    };
public:
    using Ptr    = KRPTSceneItem*;
    using FDirty = KRPTFlag<Dirty>;
    using FMust  = KRPTFlag<Must>;
    using FState = KRPTFlag<State>;
    using List   = std::list<KRPTSceneItem::Ptr>;
    using CList  = const List;
    using Index  = std::unordered_map<KRPTSceneItem::Ptr, List::iterator>;
protected:
    KRPTSceneItem()                     = delete;
    KRPTSceneItem(const KRPTSceneItem&) = delete;
    KRPTSceneItem(KRPTSceneItem&&)      = delete;
    KRPTSceneItem(KRPTScene *scene, KRPTSceneItem *parent, 
                  const QRectF &geometry = QRectF(0, 0, 100, 100), FMust must = Must::No)                            noexcept;
    virtual ~KRPTSceneItem()                                                                                         noexcept;
public:
    template<typename T, typename ... Args> inline auto addChild(Args&& ... arg)                                     noexcept
    {
        static_assert(std::is_base_of_v<KRPTSceneItem, T>, "is not scene item");
        auto item = new T(_scene, this, std::forward<Args>(arg) ...);
        addChildImpl(item, this);
        return item;
    }
    template<typename T, typename ... Args> inline auto insertChild(KRPTSceneItem::Ptr before, Args&& ... arg)       noexcept
    {
        static_assert(std::is_base_of_v<KRPTSceneItem, T>, "is not scene item");
        auto item = new T(_scene, this, std::forward<Args>(arg) ...);
        insertChildImpl(item, this, before);
        return item;
    }
    bool delChild(KRPTSceneItem *item)                                                                               noexcept;
    template<typename ... Args> inline bool mustAny(Args&& ... args)                                           const noexcept
    {
        return _must.any(std::forward<Args>(args)...);
    }
    template<typename ... Args> inline bool mustAll(Args&& ... args)                                           const noexcept
    {
        return _must.all(std::forward<Args>(args)...);
    }
    template<typename ... Args> inline void upMust(Args&& ... args)                                                  noexcept
    {
        FMust old = _must;
        _must.up(std::forward<Args>(args)...);
        if(_must != old)mustChangeImpl(_must, old);
    }
    template<typename ... Args> inline void downMust(Args&& ... args)                                                noexcept
    {
        FMust old = _must;
        _must.down(std::forward<Args>(args)...);
        if(_must != old)mustChangeImpl(_must, old);
    }
public:
    KRPTScene          * scene                 ()                                                              const noexcept;
    KRPTSceneItem::Ptr   parent                ()                                                              const noexcept;
    const List         & childItems            ()                                                              const noexcept;
    const List         & visibleChildItems     ()                                                                    noexcept;
    bool                 visible               ()                                                              const noexcept;
    const QRectF       & geometry              ()                                                              const noexcept;
    const QRectF       & rect                  ()                                                              const noexcept;
    QPointF              pos                   ()                                                              const noexcept;
    QSizeF               size                  ()                                                              const noexcept;
    double               x                     ()                                                              const noexcept;
    double               y                     ()                                                              const noexcept;
    double               width                 ()                                                              const noexcept;
    double               height                ()                                                              const noexcept;
    double               right                 ()                                                              const noexcept;
    double               bottom                ()                                                              const noexcept;
    QPointF              center                ()                                                              const noexcept;
    double               angle                 ()                                                              const noexcept;
    double               scale                 ()                                                              const noexcept;
    double               opaq                  ()                                                              const noexcept;
    TransformAnchor      transformAnchor       ()                                                              const noexcept;
    TransformAnchor      posAnchor             ()                                                              const noexcept;
    QPointF              transformAnchorPoint  ()                                                              const noexcept;
    QPointF              posAnchorPoint        ()                                                              const noexcept;
    const QTransform   & transform             ()                                                                    noexcept;
    const QTransform   & transformInv          ()                                                                    noexcept;
    const QTransform   & sceneTransform        ()                                                                    noexcept;
    const QTransform   & sceneTransformInv     ()                                                                    noexcept;
    double               sceneScale            ()                                                                    noexcept;
    double               sceneAngle            ()                                                                    noexcept;
    QRectF               bBox                  ()                                                                    noexcept;
    QRectF               bBoxMapToParent       ()                                                                    noexcept;
    bool                 contains              (const QPointF &point)                                                noexcept;
    const QPainterPath & outline               ()                                                                    noexcept;
    bool                 eventLocked           ()                                                              const noexcept;
    uint32_t             id                    ()                                                              const noexcept;
    const QColor       & color                 (uint32_t id)                                                         noexcept;
    bool                 checkable             ()                                                              const noexcept;
    bool                 checked               ()                                                              const noexcept;

    void                 setParent             (KRPTSceneItem::Ptr parent)                                           noexcept;
    void                 setVisible            (bool visible)                                                        noexcept;
    bool                 setGeometry           (const QRectF &geometry, 
                                                uint32_t time = 0, QEasingCurve curve = QEasingCurve::OutExpo)       noexcept;
    bool                 setGeometry           (const QPointF &pos, const QSizeF &size, 
                                                uint32_t time = 0, QEasingCurve curve = QEasingCurve::OutExpo)       noexcept;
    bool                 setGeometry           (double x, double y, double w, double h, 
                                                uint32_t time = 0, QEasingCurve curve = QEasingCurve::OutExpo)       noexcept;
    void                 setPos                (const QPointF &pos, 
                                                uint32_t time = 0, QEasingCurve curve = QEasingCurve::OutExpo)       noexcept;
    void                 setPos                (double x, double y, 
                                                uint32_t time = 0, QEasingCurve curve = QEasingCurve::OutExpo)       noexcept;
    void                 setSize               (const QSizeF &size, 
                                                uint32_t time = 0, QEasingCurve curve = QEasingCurve::OutExpo)       noexcept;
    void                 setSize               (double w, double h, 
                                                uint32_t time = 0, QEasingCurve curve = QEasingCurve::OutExpo)       noexcept;
    void                 setX                  (double x, 
                                                uint32_t time = 0, QEasingCurve curve = QEasingCurve::OutExpo)       noexcept;
    void                 setY                  (double y, 
                                                uint32_t time = 0, QEasingCurve curve = QEasingCurve::OutExpo)       noexcept;
    void                 setWidth              (double w, 
                                                uint32_t time = 0, QEasingCurve curve = QEasingCurve::OutExpo)       noexcept;
    void                 setHeight             (double h, 
                                                uint32_t time = 0, QEasingCurve curve = QEasingCurve::OutExpo)       noexcept;
    bool                 setAngle              (double angle, 
                                                uint32_t time = 0, QEasingCurve curve = QEasingCurve::OutExpo)       noexcept;
    void                 setAngle              (double angle, const QPointF &pt, TransSrc src, 
                                                uint32_t time = 0, QEasingCurve curve = QEasingCurve::OutExpo)       noexcept;
    bool                 setScale              (double scale, 
                                                uint32_t time = 0, QEasingCurve curve = QEasingCurve::OutExpo)       noexcept;
    void                 setScale              (double scale, const QPointF &pt, TransSrc src, 
                                                uint32_t time = 0, QEasingCurve curve = QEasingCurve::OutExpo)       noexcept;
    bool                 setOpaq               (double opaq, 
                                                uint32_t time = 0, QEasingCurve curve = QEasingCurve::OutExpo)       noexcept;
    void                 setTransformAnchor    (TransformAnchor anchor)                                              noexcept;
    void                 setPosAnchor          (TransformAnchor anchor)                                              noexcept;
    void                 translate             (const QPointF &pos, 
                                                uint32_t time = 0, QEasingCurve curve = QEasingCurve::OutExpo)       noexcept;
    void                 translate             (double dx, double dy, 
                                                uint32_t time = 0, QEasingCurve curve = QEasingCurve::OutExpo)       noexcept;
    void                 rotate                (double angle, 
                                                uint32_t time = 0, QEasingCurve curve = QEasingCurve::OutExpo)       noexcept;
    void                 rotate                (double angle, const QPointF &pt, TransSrc src, 
                                                uint32_t time, QEasingCurve curve)                                   noexcept;
    void                 scale                 (double scale, 
                                                uint32_t time = 0, QEasingCurve curve = QEasingCurve::OutExpo)       noexcept;
    void                 scale                 (double scale, const QPointF &pt, TransSrc src, 
                                                uint32_t time, QEasingCurve curve)                                   noexcept;
    void                 setId                 (uint32_t id)                                                         noexcept;
    void                 setColor              (uint32_t id, const QColor &color, FState state = State::No)          noexcept;
    void                 clearColors           ()                                                                    noexcept;
    void                 delColor              (uint32_t id, KRPTSceneItem::FState state)                            noexcept;
    void                 delColor              (uint32_t id)                                                         noexcept;
    void                 setCheckable          (bool checkable)                                                      noexcept;
    void                 setChecked            (bool checked)                                                        noexcept;

    void                 lockUpdate            (bool lock)                                                           noexcept;
    void                 lockEvents            (bool lock)                                                           noexcept;
    QPointF              mapToParent           (const QPointF &point)                                                noexcept;
    QPolygonF            mapToParent           (const QRectF &rect)                                                  noexcept;
    QPolygonF            mapToParent           (const QPolygonF &polygon)                                            noexcept;
    QPointF              mapFromParent         (const QPointF &point)                                                noexcept;
    QPolygonF            mapFromParent         (const QRectF &rect)                                                  noexcept;
    QPolygonF            mapFromParent         (const QPolygonF &polygon)                                            noexcept;
    QPointF              mapToScene            (const QPointF &point)                                                noexcept;
    QPolygonF            mapToScene            (const QRectF &rect)                                                  noexcept;
    QPolygonF            mapToScene            (const QPolygonF &polygon)                                            noexcept;
    QPointF              mapFromScene          (const QPointF &point)                                                noexcept;
    QPolygonF            mapFromScene          (const QRectF &rect)                                                  noexcept;
    QPolygonF            mapFromScene          (const QPolygonF &polygon)                                            noexcept;
    QPointF              mapToItem             (KRPTSceneItem::Ptr item, const QPointF &point)                       noexcept;
    QPolygonF            mapToItem             (KRPTSceneItem::Ptr item, const QRectF &rect)                         noexcept;
    QPolygonF            mapToItem             (KRPTSceneItem::Ptr item, const QPolygonF &polygon)                   noexcept;
    QPointF              mapFromItem           (KRPTSceneItem::Ptr item, const QPointF &point)                       noexcept;
    QPolygonF            mapFromItem           (KRPTSceneItem::Ptr item, const QRectF &rect)                         noexcept;
    QPolygonF            mapFromItem           (KRPTSceneItem::Ptr item, const QPolygonF &polygon)                   noexcept;
    bool                 isParent              (KRPTSceneItem::Ptr item)                                       const noexcept;
    KRPTSceneItem::Ptr   commonParent          (KRPTSceneItem::Ptr item)                                       const noexcept;
    bool                 needPaint             ()                                                              const noexcept;
    bool                 canBeUpdated          ()                                                              const noexcept;
    bool                 needChildPaint        ()                                                              const noexcept;
protected:
    virtual void         addChildEvent         (KRPTSceneItem::Ptr item)                                             noexcept {(void)item                        ;}
    virtual void         delChildEvent         (KRPTSceneItem::Ptr item)                                             noexcept {(void)item                        ;}
    virtual void         transformEvent        (SceneTransformEvent *e)                                              noexcept {(void)e                           ;}
    virtual void         stateChangeEvent      (const FState &newState, const FState &oldState)                      noexcept {(void)newState; (void)oldState    ;}
    virtual void         mousePressEvent       (SceneMouseEvent *e)                                                  noexcept {(void)e                           ;}
    virtual void         mouseReleaseEvent     (SceneMouseEvent *e)                                                  noexcept {(void)e                           ;}
    virtual void         mouseMoveEvent        (SceneMouseEvent *e)                                                  noexcept {(void)e                           ;}
    virtual void         mouseEnterEvent       (bool enter)                                                          noexcept {(void)enter                       ;}
    virtual void         mouseOutEvent         (KRPTSceneItem::Ptr newItem, SceneMouseEvent *e)                      noexcept {(void)newItem; (void)e            ;}
    virtual void         whellEvent            (SceneMouseEvent *e)                                                  noexcept {(void)e                           ;}
    virtual void         checkedEvent          (bool checked)                                                        noexcept {(void)checked                     ;}
    virtual void         childTransformEvent   (KRPTSceneItem::Ptr item, SceneTransformEvent *e)                     noexcept {(void)item; (void)e               ;}
    virtual void         childMousePressEvent  (KRPTSceneItem::Ptr item, SceneMouseEvent *e)                         noexcept {(void)item; (void)e               ;}
    virtual void         childMouseReleaseEvent(KRPTSceneItem::Ptr item, SceneMouseEvent *e)                         noexcept {(void)item; (void)e               ;}
    virtual void         childMouseMoveEvent   (KRPTSceneItem::Ptr item, SceneMouseEvent *e)                         noexcept {(void)item; (void)e               ;}
    virtual void         childMouseOutEvent    (KRPTSceneItem::Ptr item, 
                                                KRPTSceneItem::Ptr newItem, SceneMouseEvent *e)                      noexcept {(void)item; (void)newItem; (void)e;}
    virtual void         childWhellEvent       (KRPTSceneItem::Ptr item, SceneMouseEvent *e)                         noexcept {(void)item; (void)e               ;}
    virtual void         childCheckedEvent     (KRPTSceneItem::Ptr item, 
                                                bool checked)                                                        noexcept {(void)item; (void)checked         ;}
    virtual void         sceneTransformEvent   (const QTransform &transform)                                         noexcept {(void)transform                   ;}
    virtual void         sceneScaleEvent       (double scale, double oldScale)                                       noexcept {(void)scale; (void)oldScale       ;}
    virtual void         sceneRotateEvent      (double angle, double oldAngle)                                       noexcept {(void)angle; (void)oldAngle       ;}
protected:
    virtual void         update                ()                                                                    noexcept;
    virtual CList      & filterChildItems      ()                                                                    noexcept;
    virtual void         mustChangeImpl        (const FMust &cur, const FMust &old)                                  noexcept;
    virtual void         setParentImpl         (KRPTSceneItem::Ptr parent)                                           noexcept;
    virtual void         addChildImpl          (KRPTSceneItem::Ptr item, KRPTSceneItem::Ptr parent)                  noexcept;
    virtual void         insertChildImpl       (KRPTSceneItem::Ptr item, KRPTSceneItem::Ptr parent, 
                                                KRPTSceneItem::Ptr before)                                           noexcept;
    virtual bool         delChildImpl          (KRPTSceneItem::Ptr item, KRPTSceneItem::Ptr parent)                  noexcept;
    virtual bool         setGeometryImpl       (const QRectF &geometry)                                              noexcept;
    virtual bool         setAngleImpl          (double angle)                                                        noexcept;
    virtual bool         setScaleImpl          (double scale)                                                        noexcept;
    virtual bool         setOpaqImpl           (double opaq)                                                         noexcept;
    virtual bool         setCheckedImpl        (bool checked)                                                        noexcept;
    virtual void         transformImpl         (SceneTransformEvent *e)                                              noexcept;
    virtual void         outlineImpl           ()                                                                    noexcept;
    virtual bool         stateChangeImpl       (const FState &cur, const FState &old)                                noexcept;
    virtual void         mousePressImpl        (SceneMouseEvent *e)                                                  noexcept;
    virtual void         mouseReleaseImpl      (SceneMouseEvent *e)                                                  noexcept;
    virtual void         mouseMoveImpl         (SceneMouseEvent *e)                                                  noexcept;
    virtual void         mouseOutImpl          (KRPTSceneItem::Ptr newItem, SceneMouseEvent *e)                      noexcept;
    virtual void         whellImpl             (SceneMouseEvent *e)                                                  noexcept;
    virtual void         animImpl              (uint32_t id, const std::vector<double> &value, 
                                                uint32_t time, bool completed, int loop)                             noexcept;
    virtual void         paintBackground       (QPainter &painter, uint32_t stage)                                   noexcept;
    virtual void         paintForeground       (QPainter &painter, uint32_t stage)                                   noexcept;
protected:
    void                 transform             (const QRectF &rect, double angle, 
                                                double scale, QTransform &transform)                                 noexcept;
    void                 mappedRectPoints      (const QTransform &transform, const QRectF &rect, 
                                                std::array<QPointF, 5> &p)                                           noexcept;
    void                 bBox                  (const std::array<QPointF, 5> &p, QRectF &bBox)                       noexcept;
    void                 bBox                  (const QTransform &transform, const QRectF &rect, QRectF &bBox)       noexcept;
    QRectF               bBox                  (const QTransform &transform, const QRectF &rect)                     noexcept;
    bool                 updateCache           (bool visible = false)                                                noexcept;
    bool                 dirtyTransform        ()                                                                    noexcept;
    bool                 dirtyVisibleChilds    ()                                                                    noexcept;
    bool                 mustAnim              (uint32_t time)                                                 const noexcept;
protected:
    void                 stopAnimImpl          (uint32_t id)                                                         noexcept;
    void                 startAnimImpl         (uint32_t id, 
                                                const std::vector<double> &start, 
                                                const std::vector<double> &end, 
                                                uint32_t time, QEasingCurve curve, int loopCount = 1)                noexcept;
    void                 startAnimImpl         (uint32_t id, double start, double end, 
                                                uint32_t time, QEasingCurve curve, int loopCount = 1)                noexcept;
    void                 startAnimImpl         (uint32_t id, const QPointF &start, const QPointF &end, 
                                                uint32_t time, QEasingCurve curve, int loopCount = 1)                noexcept;
    void                 startAnimImpl         (uint32_t id, const QRectF &start, const QRectF &end, 
                                                uint32_t time, QEasingCurve curve, int loopCount = 1)                noexcept;
    void                 startAnimImpl         (uint32_t id, const QSizeF &start, const QSizeF &end, 
                                                uint32_t time, QEasingCurve curve, int loopCount = 1)                noexcept;
    void                 startAnimImpl         (uint32_t id, const QColor &start, const QColor &end, 
                                                uint32_t time, QEasingCurve curve, int loopCount = 1)                noexcept;
private:
    void                 anchorPoint           (TransformAnchor anchor, const QSizeF &size, 
                                                double &dx, double &dy)                                        const noexcept;
    void                 sendTransformEvent    (const QRectF &geometry, const QRectF &oldGeometry, 
                                                double angle, double oldAngle, double scale, double oldScale,
                                                bool moved, bool resized, bool rotated, bool scaled)                 noexcept;
protected:
    KRPTSceneItemData   *_data             ;
    KRPTScene           *_scene            ;
    KRPTSceneItem       *_parent           ;
    FDirty               _dirty            ;
    FMust                _must             ;
    FState               _state            ;
    List                 _childItems       ;
    List                 _visibleChildItems;
    Index                _index            ;
    uint32_t             _updateLocked     ;
    uint32_t             _eventLocked      ;
    bool                 _visible          ;
    QRectF               _geometry         ;
    QRectF               _rect             ;
    double               _angle            ;
    double               _scale            ;
    double               _opaq             ;
    TransformAnchor      _transformAnchor  ;
    TransformAnchor      _posAnchor        ;
    QTransform           _transform        ;
    QTransform           _transformInv     ;
    QTransform           _sceneTransform   ;
    QTransform           _sceneTransformInv;
    QPainterPath         _outline          ;
    uint32_t             _paintStageCount  ;
    QRectF               _bBox             ;
    QRectF               _bBoxMapToParent  ;
    uint32_t             _id               ;
};

