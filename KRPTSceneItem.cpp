//####################################################################################################
//#
//####################################################################################################

#include "KRPTSceneItem.h"
#include "KRPTScene.h"

//####################################################################################################
//#
//####################################################################################################

KRPTSceneItem::KRPTSceneItem(KRPTScene *scene, KRPTSceneItem *parent) noexcept
    : _scene(scene), _parent(parent), /*_clipParent(nullptr),*/ _dirty(Dirty::All), _updateLocked(false), 
      _visible(true), _angle(0), _scale(1), _borderColor(255, 255, 255), _backgroundColor(100, 100, 100)
{
}

KRPTSceneItem::~KRPTSceneItem() noexcept
{
    for(auto &child : _childItems)
        delete child;
}

//****************************************************************************************************
//*
//****************************************************************************************************

bool KRPTSceneItem::delChild(KRPTSceneItem *item) noexcept
{
    return delChildImpl(item, this);
}

//****************************************************************************************************
//*
//****************************************************************************************************

const QTransform& KRPTSceneItem::transform() noexcept 
{
    if(!_dirty[Dirty::Transform])return _transform;
    _dirty -= Dirty::Transform;
    transform(_geometry, _angle, _scale, _transform);
    return _transform;
}

const QTransform& KRPTSceneItem::transformInv() noexcept 
{
    if(!_dirty[Dirty::TransformInv])return _transformInv;
    _dirty -= Dirty::TransformInv;
    _transformInv = transform().inverted();
    return _transformInv;
}

QTransform KRPTSceneItem::foolTransform() noexcept 
{
    return _parent ? transform() * _parent->foolTransform() : transform();
}

QTransform KRPTSceneItem::foolTransformInv() noexcept 
{
    return _parent ? _parent->foolTransformInv() * transformInv() : transformInv();
}

QRectF KRPTSceneItem::bBox() noexcept
{
    if(!_dirty[Dirty::BBox])return _bBox;
    _dirty -= Dirty::BBox;
    QPointF p[4];
    QTransform t;
    transform(_rect, _angle, _scale, t);
    p[0] = t.map(_rect.topLeft    ());
    p[1] = t.map(_rect.topRight   ());
    p[2] = t.map(_rect.bottomRight());
    p[3] = t.map(_rect.bottomLeft ());
    QPointF pMin(std::numeric_limits<double>::max(), std::numeric_limits<double>::max());
    QPointF pMax;
    for(int i = 0; i < 4; ++i)
    {
        if(p[i].x() < pMin.x())pMin.setX(p[i].x());
        if(p[i].y() < pMin.y())pMin.setY(p[i].y());
        if(p[i].x() > pMax.x())pMax.setX(p[i].x());
        if(p[i].y() > pMax.y())pMax.setY(p[i].y());
    }
    _bBox.setRect(pMin.x(), pMin.y(), pMax.x() - pMin.x(), pMax.y() - pMin.y());
    return _bBox;
}

QRectF KRPTSceneItem::bBoxMapToParent() noexcept
{
    if(!_dirty[Dirty::BBoxMapToParent])return _bBoxMapToParent;
    _dirty -= Dirty::BBoxMapToParent;
    _bBoxMapToParent = bBox().translated(pos());
    return _bBoxMapToParent;
}

bool KRPTSceneItem::setGeometry(const QRectF &geometry) noexcept
{
    bool isMove   = !qFuzzyCompare(_geometry.topLeft(), geometry.topLeft());
    bool isResize = !qFuzzyCompare(_geometry.size(), geometry.size());
    if(!isMove && ! isResize)return false;
    QRectF oldGeometry = _geometry;
    _geometry = geometry;
    _rect.setSize(_geometry.size());
    _dirty += Dirty::Transform      ;
    _dirty += Dirty::TransformInv   ;
    _dirty += Dirty::BBoxMapToParent;
    _dirty += Dirty::Visibled       ;
    if(isResize)_dirty += Dirty::BBox;
    SceneTransformEvent::Ptr e = SceneTransformEvent::get(geometry, oldGeometry, 
        _angle, _angle, _scale, _scale, isMove, isResize, false, false);
    if(must(Must::TransformEvent))transformImpl(e.get());
    if(_parent && _parent->must(Must::ChildTransformEvent))
       _parent->childTransformEvent(this, e.get());
    return true;
}

bool KRPTSceneItem::setGeometry(const QPointF &pos, const QSizeF &size) noexcept
{
    return setGeometry(QRectF(pos, size));
}

bool KRPTSceneItem::setGeometry(double x, double y, double w, double h) noexcept
{
    return setGeometry(QPointF(x, y), QSizeF(w, h));
}

void KRPTSceneItem::setPos(const QPointF &pos) noexcept
{
    setGeometry(QRectF(pos, _geometry.size()));
}

void KRPTSceneItem::setPos(double x, double y) noexcept
{
    setPos(QPointF(x, y));
}

void KRPTSceneItem::setSize(const QSizeF &size) noexcept
{
    setGeometry(QRectF(_geometry.topLeft(), size));
}

void KRPTSceneItem::setSize(double w, double h) noexcept
{
    setSize(QSize(w, h));
}

void KRPTSceneItem::setX(double x) noexcept
{
    setPos(QPointF(x, _geometry.y()));
}

void KRPTSceneItem::setY(double y) noexcept
{
    setPos(QPointF(_geometry.x(), y));
}

void KRPTSceneItem::setWidth(double w) noexcept
{
    setSize(QSizeF(w, _geometry.height()));
}

void KRPTSceneItem::setHeight(double h) noexcept
{
    setSize(QSizeF(_geometry.width(), h));
}

void KRPTSceneItem::setAngle(double angle) noexcept
{
    if(qFuzzyCompare(_angle, angle))return;
    double oldAngle = _angle;
    _dirty += Dirty::Transform      ;
    _dirty += Dirty::TransformInv   ;
    _dirty += Dirty::BBox           ;
    _dirty += Dirty::BBoxMapToParent;
//    _dirty += Dirty::BBoxMapToClip  ;
    _dirty += Dirty::Visibled       ;
    _angle = angle;
    SceneTransformEvent::Ptr e = SceneTransformEvent::get(_geometry, _geometry, 
        _angle, oldAngle, _scale, _scale, false, false, true, false);
    if(must(Must::TransformEvent))transformImpl(e.get());
}

void KRPTSceneItem::setScale(double scale) noexcept
{
    if(qFuzzyCompare(_scale, scale))return;
    double oldScale = _scale;
    _dirty += Dirty::Transform      ;
    _dirty += Dirty::TransformInv   ;
    _dirty += Dirty::BBox           ;
    _dirty += Dirty::BBoxMapToParent;
    _dirty += Dirty::Visibled       ;
    _scale = scale;
    SceneTransformEvent::Ptr e = SceneTransformEvent::get(_geometry, _geometry, 
    _angle, _angle, _scale, oldScale, false, false, false, true);
    if(must(Must::TransformEvent))transformImpl(e.get());
}

void KRPTSceneItem::translate(const QPointF &pos) noexcept
{
    setPos(_geometry.topLeft() + pos);
}

void KRPTSceneItem::translate(double x, double y) noexcept
{
    translate(QPointF(x, y));
}

void KRPTSceneItem::rotate(double angle) noexcept
{
    setAngle(_angle + angle);
}

void KRPTSceneItem::rotateAround(double angle, const QPointF &pt, TransSrc src) noexcept
{
    QTransform t;
    transform(_geometry, angle + _angle, _scale, t);
    _geometry.translate(transformShift(t, src, pt));
    rotate(angle);
}

void KRPTSceneItem::scaleMul(double scale) noexcept
{
    setScale(_scale * scale);
}

void KRPTSceneItem::scaleFromPoint(double scale, const QPointF &pt, TransSrc src) noexcept
{
    QTransform t;
    transform(_geometry, _angle, _scale * scale, t);
    _geometry.translate(transformShift(t, src, pt));
    scaleMul(scale);
}

void KRPTSceneItem::setBorderColor(const QColor &color) noexcept 
{
    _borderColor = color;
}

void KRPTSceneItem::setBackgroundColor(const QColor &color) noexcept 
{
    _backgroundColor = color;
}

void KRPTSceneItem::lockUpdate(bool lock) noexcept 
{
    if(_updateLocked == lock)return;
    _updateLocked = lock;
    if(!_updateLocked)update();
}

QPointF KRPTSceneItem::mapToParent(const QPointF &p) noexcept 
{
    return transform().map(p);
}

QPolygonF KRPTSceneItem::mapToParent(const QRectF &r) noexcept 
{
    return transform().map(r);
}

QPolygonF KRPTSceneItem::mapToParent(const QPolygonF &p) noexcept 
{
    return transform().map(p);
}

QPointF KRPTSceneItem::mapFromParent(const QPointF &p) noexcept 
{
    return transformInv().map(p);
}

QPolygonF KRPTSceneItem::mapFromParent(const QRectF &r) noexcept 
{
    return transformInv().map(r);
}

QPolygonF KRPTSceneItem::mapFromParent(const QPolygonF &p) noexcept 
{
    return transformInv().map(p);
}

QPointF KRPTSceneItem::mapToScene(const QPointF &p) noexcept 
{
    return foolTransform().map(p);
}

QPolygonF KRPTSceneItem::mapToScene(const QRectF &r) noexcept 
{
    return foolTransform().map(r);
}

QPolygonF KRPTSceneItem::mapToScene(const QPolygonF &p) noexcept 
{
    return foolTransform().map(p);
}

QPointF KRPTSceneItem::mapFromScene(const QPointF &p) noexcept 
{
    return foolTransformInv().map(p);
}

QPolygonF KRPTSceneItem::mapFromScene(const QRectF &r) noexcept 
{
    return foolTransformInv().map(r);
}

QPolygonF KRPTSceneItem::mapFromScene(const QPolygonF &p) noexcept 
{
    return foolTransformInv().map(p);
}

//****************************************************************************************************
//*
//****************************************************************************************************

void KRPTSceneItem::addChildEvent(KRPTSceneItem *item) noexcept 
{
};

void KRPTSceneItem::delChildEvent(KRPTSceneItem *item) noexcept 
{
};

void KRPTSceneItem::transformEvent(SceneTransformEvent *e) noexcept
{
}

void KRPTSceneItem::mousePressEvent(SceneMouseEvent *e) noexcept
{
}

void KRPTSceneItem::mouseReleaseEvent(SceneMouseEvent *e) noexcept
{
}

void KRPTSceneItem::mouseMoveEvent(SceneMouseEvent *e) noexcept
{
}

void KRPTSceneItem::whellEvent(SceneMouseEvent *e) noexcept
{
}

void KRPTSceneItem::childTransformEvent(KRPTSceneItem *item, SceneTransformEvent *e) noexcept
{
}

//****************************************************************************************************
//*
//****************************************************************************************************

void KRPTSceneItem::update() noexcept
{
    if(_updateLocked)return;
    _scene->update();
}

void KRPTSceneItem::addChildImpl(KRPTSceneItem::Ptr item, KRPTSceneItem *parent) noexcept
{
    item->_parent = parent;
    ItemsList::iterator it = parent->_childItems.emplace(parent->_childItems.end(), item);
    parent->_index.emplace(item, it);
    addChildEvent(item);
    updateGeometry();
}

bool KRPTSceneItem::delChildImpl(KRPTSceneItem *item, KRPTSceneItem *parent) noexcept
{
    if(!item)return false;
    auto it = parent->_index.find(item);
    if(it == parent->_index.end())return false;
    parent->_childItems.erase(it->second);
    parent->_index.erase(it);
    delChildEvent(item);
    delete item;
    updateGeometry();
    return true;
}

void KRPTSceneItem::transformImpl(SceneTransformEvent *e) noexcept
{
    transformEvent(e);
    update();
}

void KRPTSceneItem::mousePressImpl(SceneMouseEvent *e) noexcept
{
    mousePressEvent(e);
}

void KRPTSceneItem::mouseReleaseImpl(SceneMouseEvent *e) noexcept
{
    mouseReleaseEvent(e);
}

void KRPTSceneItem::mouseMoveImpl(SceneMouseEvent *e) noexcept
{
    mouseMoveEvent(e);
}

void KRPTSceneItem::whellImpl(SceneMouseEvent *e) noexcept
{
    whellEvent(e);
}

void KRPTSceneItem::paintBackground(QPainter &painter) noexcept
{
    painter.fillRect(_rect, _backgroundColor);
}

void KRPTSceneItem::paintForeground(QPainter &painter) noexcept
{
//    , _borderColor);
    painter.drawRect(_rect);
}

void KRPTSceneItem::updateGeometry() noexcept
{
    update();
}

//****************************************************************************************************
//*
//****************************************************************************************************

void KRPTSceneItem::transform(const QRectF &rect, double angle, double scale, QTransform &transform) noexcept 
{
    transform.reset();
    transform.translate(rect.x(), rect.y());
    if(!qFuzzyIsNull(angle) || !qFuzzyCompare(scale, 1))
    {
        double wd2 = rect.width () / 2.0;
        double hd2 = rect.height() / 2.0;
        transform.translate(wd2, hd2);
        if(!qFuzzyCompare(scale, 1))transform.scale (scale, scale);
        if(!qFuzzyIsNull(angle    ))transform.rotate(angle       );
        transform.translate(-wd2, -hd2);
    }
}

QPointF KRPTSceneItem::transformShift(QTransform &t, TransSrc src, const QPointF &pt) noexcept
{
    QPointF p0 = pt;
    QPointF p1 = pt;
    switch(src)
    {
        case TransSrc::Parent :
            p1 = mapFromParent(pt);
            break;
        case TransSrc::Scene  :
            if(_parent)t *= _parent->foolTransform();
            p1 = mapFromScene(pt);
            break;
        case TransSrc::Self :
            p0 = mapToParent(pt);
            break;
    }
    return p0 - t.map(p1);
}
