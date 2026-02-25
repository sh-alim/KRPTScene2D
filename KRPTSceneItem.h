//####################################################################################################
//#
//####################################################################################################

#pragma once

#include <cstdint>
#include <list>
#include <QRectF>
#include <QTransform>
#include <QPainter>
#include <QColor>
#include "KRPTFlag.h"

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
friend class KRPTScene;
    enum class Dirty : uint8_t
    {
        No                      = 0x00,
        Transform               = 0x01,
        TransformInv            = 0x02,
        SceneTransformInv       = 0x04,
        BBox                    = 0x08,
        BBoxMapToParent         = 0x10,
        VisibleChildItems       = 0x20,
        All                     = 0xFF,
    };
    enum class State : uint8_t
    {
        No                      = 0x00,
        VisibledInView          = 0x01,
        NeedPaint               = 0x02,
        NeedChildPaint          = 0x04,
        All                     = 0xFF
    };
public:
    enum class Must : uint16_t
    {
        No                      = 0x0000,
        NoPaint                 = 0x0001,
        NoClipChilds            = 0x0002,
        NoScale                 = 0x0004,
        MousePressEvent         = 0x0008,
        MouseReleaseEvent       = 0x0010,
        MouseMoveEvent          = 0x0020,
        TransformEvent          = 0x0040,
        WhellEvent              = 0x0080,
        ChildTransformEvent     = 0x0100,
        All                     = 0xFFFF
    };
    enum class TransSrc : uint8_t{Self, Parent, Scene};
public:
    using Ptr       = KRPTSceneItem*;
    using ItemsList = std::list<KRPTSceneItem::Ptr>;
    using IndexMap  = std::map<KRPTSceneItem*, ItemsList::iterator>;
public:
    KRPTSceneItem(KRPTScene *scene, KRPTSceneItem *parent)                          noexcept;
    virtual ~KRPTSceneItem()                                                        noexcept;
public:
    template<typename T, typename ... Args>
    auto addChild(Args&& ... arg)                                                   noexcept
    {
        auto item = new T(_scene, this, 
            std::forward<Args>(arg) ...);
        addChildImpl(item, this);
        return item;
    }
    bool delChild(KRPTSceneItem *item)                                              noexcept;
    template<typename ... Args> 
    inline bool must(Args&& ... args)                                         const noexcept
    {
        return _must.upped(std::forward<Args>(args)...);
    }
    template<typename ... Args> 
    inline void addMust(Args&& ... args)                                            noexcept
    {
        _must.up(std::forward<Args>(args)...);
    }
public:
    KRPTFlag<Must>     must                ()                                 const noexcept;
    KRPTScene        * scene               ()                                 const noexcept;
    KRPTSceneItem    * parent              ()                                 const noexcept;
    const ItemsList  & childItems          ()                                 const noexcept;
    const ItemsList  & visibleChildItems   ()                                       noexcept;
    bool               visible             ()                                 const noexcept;
    const QRectF     & geometry            ()                                 const noexcept;
    const QRectF     & rect                ()                                 const noexcept;
    QPointF            pos                 ()                                 const noexcept;
    QSizeF             size                ()                                 const noexcept;
    double             x                   ()                                 const noexcept;
    double             y                   ()                                 const noexcept;
    double             width               ()                                 const noexcept;
    double             height              ()                                 const noexcept;
    double             right               ()                                 const noexcept;
    double             bottom              ()                                 const noexcept;
    QPointF            center              ()                                 const noexcept;
    double             angle               ()                                 const noexcept;
    double             scale               ()                                 const noexcept;
    const QTransform & transform           ()                                       noexcept;
    const QTransform & transformInv        ()                                       noexcept;
    const QTransform & sceneTransform      ()                                       noexcept;
    const QTransform & sceneTransformInv   ()                                       noexcept;
    QRectF             bBox                ()                                       noexcept;
    QRectF             bBoxMapToParent     ()                                       noexcept;
    QColor             borderColor         ()                                 const noexcept {return _borderColor       ;}
    QColor             backgroundColor     ()                                 const noexcept {return _backgroundColor   ;}

    void               setVisible          (bool visible)                           noexcept;
    bool               setGeometry         (const QRectF &geometry)                 noexcept;
    bool               setGeometry         (const QPointF &pos, const QSizeF &size) noexcept;
    bool               setGeometry         (double x, double y, 
                                            double w, double h)                     noexcept;
    void               setPos              (const QPointF &pos)                     noexcept;
    void               setPos              (double x, double y)                     noexcept;
    void               setSize             (const QSizeF &size)                     noexcept;
    void               setSize             (double w, double h)                     noexcept;
    void               setX                (double x)                               noexcept;
    void               setY                (double y)                               noexcept;
    void               setWidth            (double w)                               noexcept;
    void               setHeight           (double h)                               noexcept;
    void               setAngle            (double angle)                           noexcept;
    void               setScale            (double scale)                           noexcept;
    void               translate           (const QPointF &pos)                     noexcept;
    void               translate           (double dx, double dy)                   noexcept;
    void               rotate              (double angle)                           noexcept;
    void               rotateAround        (double angle, const QPointF &pt, 
                                            TransSrc src)                           noexcept;
    void               scaleMul            (double scale)                           noexcept;
    void               scaleFromPoint      (double scale, const QPointF &pt, 
                                            TransSrc src)                           noexcept;
    void               setBorderColor      (const QColor &color)                    noexcept;
    void               setBackgroundColor  (const QColor &color)                    noexcept;
    void               lockUpdate          (bool lock)                              noexcept;
    QPointF            mapToParent         (const QPointF   &p)                     noexcept;
    QPolygonF          mapToParent         (const QRectF    &r)                     noexcept;
    QPolygonF          mapToParent         (const QPolygonF &p)                     noexcept;
    QPointF            mapFromParent       (const QPointF   &p)                     noexcept;
    QPolygonF          mapFromParent       (const QRectF    &r)                     noexcept;
    QPolygonF          mapFromParent       (const QPolygonF &p)                     noexcept;
    QPointF            mapToScene          (const QPointF   &p)                     noexcept;
    QPolygonF          mapToScene          (const QRectF    &r)                     noexcept;
    QPolygonF          mapToScene          (const QPolygonF &p)                     noexcept;
    QPointF            mapFromScene        (const QPointF   &p)                     noexcept;
    QPolygonF          mapFromScene        (const QRectF    &r)                     noexcept;
    QPolygonF          mapFromScene        (const QPolygonF &p)                     noexcept;
    bool               needPaint           ()                                 const noexcept;
    bool               needChildPaint      ()                                 const noexcept;
protected:
    virtual void       addChildEvent       (KRPTSceneItem       *item)              noexcept;
    virtual void       delChildEvent       (KRPTSceneItem       *item)              noexcept;
    virtual void       transformEvent      (SceneTransformEvent *e   )              noexcept;
    virtual void       mousePressEvent     (SceneMouseEvent     *e   )              noexcept;
    virtual void       mouseReleaseEvent   (SceneMouseEvent     *e   )              noexcept;
    virtual void       mouseMoveEvent      (SceneMouseEvent     *e   )              noexcept;
    virtual void       whellEvent          (SceneMouseEvent     *e   )              noexcept;
    virtual void       childTransformEvent (KRPTSceneItem       *item,
                                            SceneTransformEvent *e   )              noexcept;
protected:
    virtual void       update              ()                                       noexcept;
    virtual void       addChildImpl        (KRPTSceneItem::Ptr item,
                                            KRPTSceneItem *parent    )              noexcept;
    virtual bool       delChildImpl        (KRPTSceneItem *item,
                                            KRPTSceneItem *parent    )              noexcept;
    virtual void       transformImpl       (SceneTransformEvent    *e)              noexcept;
    virtual void       mousePressImpl      (SceneMouseEvent        *e)              noexcept;
    virtual void       mouseReleaseImpl    (SceneMouseEvent        *e)              noexcept;
    virtual void       mouseMoveImpl       (SceneMouseEvent        *e)              noexcept;
    virtual void       whellImpl           (SceneMouseEvent        *e)              noexcept;
    virtual void       paintBackground     (QPainter &painter        )              noexcept;
    virtual void       paintForeground     (QPainter &painter        )              noexcept;
    virtual void       updateGeometry      ()                                       noexcept;
protected:
    void               transform           (const QRectF &rect, double angle, 
                                            double scale, QTransform &transform)    noexcept;
    QPointF            transformShift      (QTransform &transform, 
                                            TransSrc src, const QPointF &pt)        noexcept;
    void               bBox                (const QTransform &transform, 
                                            const QRectF &rect, QRectF &bBox)       noexcept;
    QRectF             bBox                (const QTransform &transform, 
                                            const QRectF &rect)                     noexcept;
    bool               updateCache         (bool visible = false)                   noexcept;
    bool               dirtyTransform      ()                                       noexcept;
    bool               dirtyVisibleChilds  ()                                       noexcept;
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
    bool               _updateLocked     ;
    bool               _visible          ;
    QRectF             _geometry         ;
    QRectF             _rect             ;
    double             _angle            ;
    double             _scale            ;
    QTransform         _transform        ;
    QTransform         _transformInv     ;
    QTransform         _sceneTransform   ;
    QTransform         _sceneTransformInv;
    QRectF             _bBox             ;
    QRectF             _bBoxMapToParent  ;
    QColor             _borderColor      ;
    QColor             _backgroundColor  ;
};

