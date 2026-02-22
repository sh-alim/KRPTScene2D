//####################################################################################################
//#
//####################################################################################################

#pragma once

//####################################################################################################
//#
//####################################################################################################

#include <memory>
#include "KRPTSceneItem.h"
#include <QWidget>

#include <QElapsedTimer>

//####################################################################################################
//#
//####################################################################################################

class KRPTSceneItem;

//####################################################################################################
//#
//####################################################################################################

class SceneMouseEvent
{
public:
    using Ptr = std::unique_ptr<SceneMouseEvent>;
    enum class Btn : uint8_t {No = 0x00, Left = 0x01, Right = 0x02, Middle = 0x04};
    using Btns = KRPTFlag<Btn>;
public:
    SceneMouseEvent(const QPointF &pos, Btns btns, int32_t delta)
        : _pos(pos), _btns(btns), _delta(delta) {}
    static SceneMouseEvent::Ptr get(const QPointF &pos, Btns btns, int32_t delta = 0)
    {
        return std::make_unique<SceneMouseEvent>(pos, btns, delta);
    }
    Btns    btns () const noexcept {return _btns ;}
    QPointF pos  () const noexcept {return _pos  ;}
    int32_t delta() const noexcept {return _delta;}
private:
    Btns    _btns ;
    QPointF _pos  ;
    int32_t _delta;
};

class SceneTransformEvent
{
public:
    using Ptr = std::unique_ptr<SceneTransformEvent>;
public:
    SceneTransformEvent(const QRectF &geometry, const QRectF &oldGeometry, 
        double angle, double oldAngle, double scale, double oldScale,
        bool moved, bool resized, bool rotated, bool scaled)
        : _geometry(geometry), _oldGeometry(oldGeometry), _angle(angle), _oldAngle(oldAngle), 
          _scale(scale), _oldScale(oldScale), _moved(moved), _resized(resized), 
          _rotated(rotated), _scaled(scaled){}
    static SceneTransformEvent::Ptr get(const QRectF &geometry, const QRectF &oldGeometry, 
        double angle, double _oldAngle, double _scale, double _oldScale,
        bool moved, bool resized, bool rotated, bool scaled)
    {
        return std::make_unique<SceneTransformEvent>(geometry, oldGeometry, angle, _oldAngle, 
            _scale, _oldScale, moved, resized, rotated, scaled);
    }
    QRectF geometry   () const noexcept {return _geometry   ;}
    QRectF oldGeometry() const noexcept {return _oldGeometry;}
    double angle      () const noexcept {return _angle      ;}
    double oldAngle   () const noexcept {return _oldAngle   ;}
    double scale      () const noexcept {return _scale      ;}
    double oldScale   () const noexcept {return _oldScale   ;}
    bool   moved      () const noexcept {return _moved      ;}
    bool   resized    () const noexcept {return _resized    ;}
    bool   rotated    () const noexcept {return _rotated    ;}
    bool   scaled     () const noexcept {return _scaled     ;}
private:
    QRectF _geometry   ;
    QRectF _oldGeometry;
    double _angle      ;
    double _oldAngle   ;
    double _scale      ;
    double _oldScale   ;
    bool   _moved      ;
    bool   _resized    ;
    bool   _rotated    ;
    bool   _scaled     ;
};

//####################################################################################################
//#
//####################################################################################################

class KRPTScene
{
    friend class KRPTSceneItem;
    friend class KRPTSceneWidget;
    using CompFn = const std::function<bool(KRPTSceneItem*)>&;
    using Items  = std::list<KRPTSceneItem*>;
public:
    KRPTScene(QWidget *canvas) noexcept;
    virtual ~KRPTScene()       noexcept;
public:
    template<typename T, typename ... Args>
    auto addItem(Args&& ... arg)
    {
        return _item->addChild<T>(std::forward<Args>(arg) ...);
    }
    template<typename T, typename ... Args>
    auto createRootItem(Args&& ... arg)
    {
        if(_item)delete _item;
        _item = new T(this, nullptr, std::forward<Args>(arg) ...);
        return _item;
    }
public:
    const QRectF     & geometry           ()                                 const noexcept;
    const QRectF     & rect               ()                                 const noexcept;
    QPointF            pos                ()                                 const noexcept;
    QSizeF             size               ()                                 const noexcept;
    double             x                  ()                                 const noexcept;
    double             y                  ()                                 const noexcept;
    double             width              ()                                 const noexcept;
    double             height             ()                                 const noexcept;
    const QTransform & transform          ()                                 const noexcept;
    const QTransform & sceneTransform     ()                                 const noexcept;
    QColor             borderColor        ()                                 const noexcept;
    QColor             backgroundColor    ()                                 const noexcept;

    void               setGeometry        (const QRectF &geometry)                 noexcept;
    void               setPos             (const QPointF &pos)                     noexcept;
    void               setPos             (double x, double y)                     noexcept;
    void               setSize            (const QSizeF &size)                     noexcept;
    void               setSize            (double width, double height)            noexcept;
    void               setX               (double x)                               noexcept;
    void               setY               (double y)                               noexcept;
    void               setWidth           (double width)                           noexcept;
    void               setHeight          (double height)                          noexcept;

    void               setBorderColor    (const QColor &color)                     noexcept;
    void               setBackgroundColor(const QColor &color)                     noexcept;

public:
    KRPTSceneItem    * itemFromPos        (const QPointF &pos, CompFn comp)        noexcept;
    Items              itemsFromPos       (const QPointF &, CompFn comp, bool one) noexcept;
public:
    virtual void       update             ()                                       noexcept;
protected:
    KRPTSceneItem    * itemFromPosImpl    (const QPointF &pos, CompFn comp, 
                                           KRPTSceneItem *item)                    noexcept;
    Items              itemsFromPosImpl   (const QPointF &pos, CompFn comp, 
                                           KRPTSceneItem *item, bool one, 
                                           uint32_t level = 0)                     noexcept;
    void               paintImpl          (QPainter &painter, KRPTSceneItem *item) noexcept;
private:
    void               transformEvent     (SceneTransformEvent *e)                 noexcept;
    void               mousePressEvent    (SceneMouseEvent     *e)                 noexcept;
    void               mouseReleaseEvent  (SceneMouseEvent     *e)                 noexcept;
    void               mouseMoveEvent     (SceneMouseEvent     *e)                 noexcept;
    void               whellEvent         (SceneMouseEvent     *e)                 noexcept;
    void               paintEvent         (QPainter &painter     )                 noexcept;
private:
    QWidget       * _canvas            ;
    KRPTSceneItem * _item              ;
    KRPTSceneItem *_mousePressedItem   ;
    QPointF        _mousePressedItemPos;
    QPointF        _lastMousePos       ;
};