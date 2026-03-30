//########################################################################################################################
//#
//########################################################################################################################

#pragma once

//########################################################################################################################
//#
//########################################################################################################################

#include <memory>
#include "KRPTSceneItem.h"
#include <QElapsedTimer>

//########################################################################################################################
//#
//########################################################################################################################

class KRPTSceneItem;
class KRPTSceneView;

//########################################################################################################################
//#
//########################################################################################################################

class KRPTScene
{
    friend class KRPTSceneItem;
    friend class KRPTSceneWidget;
    using CompFn = const std::function<bool(KRPTSceneItem*)>&;
    using Items  = std::list<KRPTSceneItem*>;
public:
    KRPTScene(KRPTSceneView *view)                                                    noexcept;
    virtual ~KRPTScene()                                                              noexcept;
public:
    template<typename T, typename ... Args>
    auto addItem(Args&& ... arg)                                                      noexcept 
    {
        return _item->addChild<T>(std::forward<Args>(arg) ...);
    }
    template<typename T, typename ... Args> 
    auto createRootItem(Args&& ... arg)                                               noexcept
    {
        if(_item)delete _item;
        _item = new T(this, nullptr, std::forward<Args>(arg) ...);
        return _item;
    }
public:
    const QRectF     & geometry              ()                                 const noexcept;
    const QRectF     & rect                  ()                                 const noexcept;
    QPointF            pos                   ()                                 const noexcept;
    QSizeF             size                  ()                                 const noexcept;
    double             x                     ()                                 const noexcept;
    double             y                     ()                                 const noexcept;
    double             width                 ()                                 const noexcept;
    double             height                ()                                 const noexcept;
    double             scale                 ()                                 const noexcept;
    QPointF            center                ()                                 const noexcept;
    const QTransform & transform             ()                                 const noexcept;
    const QTransform & sceneTransform        ()                                 const noexcept;
    KRPTSceneItem::Ptr mousePressedItem      ()                                 const noexcept;
    QColor             borderColor           ()                                 const noexcept;
    QColor             backgroundColor       ()                                 const noexcept;

    void               setGeometry           (const QRectF &geometry)                 noexcept;
    void               setPos                (const QPointF &pos)                     noexcept;
    void               setPos                (double x, double y)                     noexcept;
    void               setSize               (const QSizeF &size)                     noexcept;
    void               setSize               (double width, double height)            noexcept;
    void               setX                  (double x)                               noexcept;
    void               setY                  (double y)                               noexcept;
    void               setWidth              (double width)                           noexcept;
    void               setHeight             (double height)                          noexcept;
    void               setScale              (double scale)                           noexcept;
    void               setDeviceScale        (double scale)                           noexcept;

    void               setBorderColor        (const QColor &color)                    noexcept;
    void               setBackgroundColor    (const QColor &color)                    noexcept;

public:
    KRPTSceneItem::Ptr itemFromPos           (const QPointF &pos, CompFn comp)        noexcept;
    Items              itemsFromPos          (const QPointF &pos, CompFn comp, 
                                              bool one)                               noexcept;
public:
    virtual void       update                ()                                       noexcept;
public:
    virtual void       transformEvent        (SceneTransformEvent *e)                 noexcept {};
    virtual void       childTransformEvent   (KRPTSceneItem::Ptr item, 
                                              SceneTransformEvent *e)                 noexcept {};
    virtual void       childMousePressEvent  (KRPTSceneItem::Ptr item, 
                                              SceneMouseEvent *e)                     noexcept {};
    virtual void       childMouseReleaseEvent(KRPTSceneItem::Ptr item, 
                                              SceneMouseEvent *e)                     noexcept {};
    virtual void       childMouseMoveEvent   (KRPTSceneItem::Ptr item, 
                                              SceneMouseEvent *e)                     noexcept {};
    virtual void       childWhellEvent       (KRPTSceneItem::Ptr item, 
                                              SceneMouseEvent *e)                     noexcept {};
    virtual void       mousePressEvent       (SceneMouseEvent *e)                     noexcept;
    virtual void       mouseReleaseEvent     (SceneMouseEvent *e)                     noexcept;
    virtual void       mouseMoveEvent        (SceneMouseEvent *e)                     noexcept;
    virtual void       whellEvent            (SceneMouseEvent *e)                     noexcept;
    virtual void       deviceScaleEvent      (double scale)                           noexcept;
    virtual void       paintEvent            (QPainter &painte)                       noexcept;

protected:
    KRPTSceneItem::Ptr itemFromPosImpl       (const QPointF &pos, CompFn comp, 
                                              KRPTSceneItem *item)                    noexcept;
    Items              itemsFromPosImpl      (const QPointF &pos, CompFn comp, 
                                              KRPTSceneItem *item, bool one, 
                                              uint32_t level = 0)                     noexcept;
    void               paintImpl             (QPainter &painter, KRPTSceneItem *item, 
                                              uint32_t stage = 0)                     noexcept;
private:
    KRPTSceneView      * _view               ;
    KRPTSceneItem::Ptr   _item               ;
    double               _deviceScale        ;
    KRPTSceneItem::Ptr   _mousePressedItem   ;
    QPointF              _mousePressedItemPos;
    QPointF              _lastMousePos       ;
};