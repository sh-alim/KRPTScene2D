//########################################################################################################################
//#
//########################################################################################################################

#pragma once

//########################################################################################################################
//#
//########################################################################################################################

#include "KRPTFlag.h"

//########################################################################################################################
//#
//########################################################################################################################

class SceneMouseEvent
{
public:
    using Ptr = std::unique_ptr<SceneMouseEvent>;
    enum class Btn        : uint8_t {No = 0x00, Left = 0x01, Right = 0x02, Middle = 0x04};
    enum class KeyModifer : uint8_t {No = 0x00, Ctrl = 0x01, Shift = 0x02, Alt    = 0x04};
    using Btns        = KRPTFlag<Btn>;
    using KeyModifers = KRPTFlag<KeyModifer>;
public:
    SceneMouseEvent(const QPointF &pos, Btns btns, const QPointF &scenePos, 
        KeyModifers keyModifers, const QPointF &delta)
        : _pos(pos), _scenePos(scenePos), _btns(btns), _keyModifers(keyModifers), _delta(delta), _doubleClick(false) {}

    static SceneMouseEvent::Ptr get(const QPointF &pos, Btns btns, const QPointF &scenePos, 
        KeyModifers keyModifers, const QPointF &delta = QPointF())
    {
        return std::make_unique<SceneMouseEvent>(pos, btns, scenePos, keyModifers, delta);
    }
    Btns        btns          ()                     const noexcept {return _btns              ;}
    KeyModifers keyModifers   ()                     const noexcept {return _keyModifers       ;}
    QPointF     pos           ()                     const noexcept {return _pos               ;}
    QPointF     scenePos      ()                     const noexcept {return _scenePos          ;}
    QPointF     delta         ()                     const noexcept {return _delta             ;}
    bool        doubleClick   ()                     const noexcept {return _doubleClick       ;}
    void        setDelta      (const QPointF &delta)       noexcept {_delta = delta            ;}
    void        setDoubleClick(bool doubleClick)           noexcept {_doubleClick = doubleClick;}
private:
    QPointF     _pos        ;
    QPointF     _scenePos   ;
    Btns        _btns       ;
    KeyModifers _keyModifers;
    QPointF     _delta      ;
    bool        _doubleClick;
};

//########################################################################################################################
//#
//########################################################################################################################

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
        double angle, double oldAngle, double scale, double oldScale,
        bool moved, bool resized, bool rotated, bool scaled)
    {
        return std::make_unique<SceneTransformEvent>(geometry, oldGeometry, angle, oldAngle, 
            scale, oldScale, moved, resized, rotated, scaled);
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

