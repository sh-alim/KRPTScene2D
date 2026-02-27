//####################################################################################################
//#
//####################################################################################################

#include "KRPTSceneItem.h"
#include "KRPTScene.h"
#include "KRPTSceneAnim.h"

//####################################################################################################
//#
//####################################################################################################

class KRPTSceneItemData
{
friend class KRPTSceneItem;
public:
    KRPTSceneItemData(KRPTSceneItem *owner)
        : owner(owner), genTransform(0), genScale(0), sceneScale(1) {}
public:
    struct Cache
    {
        Cache(KRPTSceneItem *item, KRPTSceneItem *parent) 
            : item(item), parent(parent), genTransform(0), genParentTransform(0), 
              genVisibleChildItems(0), genScale(0), visible(false), nextDirty(false)
        {}
        KRPTSceneItem *item                ;
        KRPTSceneItem *parent              ;
        QTransform     transform           ;
        uint32_t       genTransform        ;
        uint32_t       genParentTransform  ;
        uint32_t       genVisibleChildItems;
        uint32_t       genScale            ;
        QRectF         bBox                ;
        bool           visible             ;
        bool           nextDirty           ;  
    };
    struct Anim
    {
        using Values = std::vector<double>;
        using Map = std::map<uint32_t, std::unique_ptr<Anim>>;
        Anim (uint32_t id, KRPTSceneAnim::Event &event, 
              int duration, QEasingCurve easingCurve) noexcept 
            : anim(new KRPTSceneAnim(id, event, duration, easingCurve)) {}
        ~Anim() noexcept {if(anim)anim->deleteLater();}
        inline int duration() const {return anim ? anim->duration() : 0;}
        template<typename T>
        inline void update(const T &r0, const T &r1) noexcept
        {
            size_t size = KRPTSceneAnim::valuesFrom(r0, start);
            KRPTSceneAnim::valuesFrom(r1, end);
            if(current.size() < size)current.resize(size);
        }
        KRPTSceneAnim *anim   ;
        Values         start  ;
        Values         end    ;
        Values         current;
    };
    template<typename T>
    void startAnim(uint32_t id, const T &start, const T &end, uint32_t duration, QEasingCurve easingCurve)
    {
        if(qFuzzyCompare(start, end))
        {
            std::vector<double> value;
            size_t size = KRPTSceneAnim::valuesFrom(end, value);
            owner->animImpl(id, value);
            deleteAnim(id);
            return;
        }
        Anim *anim = addAnim(id, duration, easingCurve);
        KRPTSceneAnim *sceneAnim = anim->anim;
        sceneAnim->stop();
        anim->update(start, end);
        sceneAnim->setDuration(duration);
        sceneAnim->start();
    }
    void stopAnim(uint32_t id)
    {
        deleteAnim(id);
    }
private:
    Anim* anim(uint32_t id) noexcept
    {
        auto findAnim = anims.find(id);
        return findAnim != anims.end() ? findAnim->second.get() : nullptr;
    }
    Anim* addAnim(uint32_t id, int duration, QEasingCurve easingCurve) noexcept
    {
        auto findAnim = anims.find(id);
        if(findAnim == anims.end())
        {
            if(!animFunction)animFunction = std::bind(&KRPTSceneItemData::animEvent, this, 
                std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
            findAnim = anims.emplace(id, std::make_unique<KRPTSceneItemData::Anim>
                                    (id, animFunction, duration, easingCurve)).first;
        }
        return findAnim->second.get();
    }
    void deleteAnim(uint32_t id) noexcept
    {
        auto findAnim = anims.find(id);
        if(findAnim == anims.end())return;
        findAnim->second->anim->stop();
        anims.erase(findAnim);
    }
    void animEvent(uint32_t id, int time, double progress) noexcept
    {
        auto anim = this->anim(id);
        if(!anim)return;
        KRPTSceneAnim::interpolate(anim->start, anim->end, progress, anim->current);
        if(anim->duration() == time)
        {
            owner->animImpl(id, anim->end);
            deleteAnim(id);
        }else owner->animImpl(id, anim->current);
    }
private:
    KRPTSceneItem       *owner       ;
    std::list<Cache>     cache       ;
    uint32_t             genTransform;
    uint32_t             genScale    ;
    double               sceneScale  ;
    Anim::Map            anims       ;
    KRPTSceneAnim::Event animFunction;
};

//####################################################################################################
//#
//####################################################################################################

KRPTSceneItem::KRPTSceneItem(KRPTScene *scene, KRPTSceneItem *parent) noexcept
    : _scene(scene), _parent(parent), _data(new KRPTSceneItemData(this)), _dirty(Dirty::All), 
      _updateLocked(false), _visible(true), _angle(0), _scale(1), _opaq(1), 
      _borderColor(255, 255, 255), _backgroundColor(50, 50, 50)
{
}

KRPTSceneItem::~KRPTSceneItem() noexcept
{
    for(auto &child : _childItems)
        delete child;
    delete _data;
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

KRPTFlag<KRPTSceneItem::Must> KRPTSceneItem::must() const noexcept 
{
    return _must;
}

KRPTScene* KRPTSceneItem::scene() const noexcept 
{
    return _scene;
}

KRPTSceneItem* KRPTSceneItem::parent() const noexcept 
{
    return _parent;
}

const KRPTSceneItem::ItemsList& KRPTSceneItem::childItems() const noexcept 
{
    return _childItems;
}

bool KRPTSceneItem::visible() const noexcept 
{
    return _visible;
}

const QRectF & KRPTSceneItem::geometry() const noexcept 
{
    return _geometry;
}

const QRectF & KRPTSceneItem::rect() const noexcept 
{
    return _rect;
}

QPointF KRPTSceneItem::pos() const noexcept 
{
    return _geometry.topLeft();
}

QSizeF KRPTSceneItem::size() const noexcept 
{
    return _geometry.size();
}

double KRPTSceneItem::x() const noexcept 
{
    return _geometry.x();
}

double KRPTSceneItem::y() const noexcept 
{
    return _geometry.y();
}

double KRPTSceneItem::width() const noexcept 
{
    return _geometry.width();
}

double KRPTSceneItem::height() const noexcept 
{
    return _geometry.height();
}

double KRPTSceneItem::right() const noexcept 
{
    return _geometry.right();
}

double KRPTSceneItem::bottom() const noexcept 
{
    return _geometry.bottom();
}

QPointF KRPTSceneItem::center() const noexcept 
{
    return _geometry.center();
}

double KRPTSceneItem::angle() const noexcept 
{
    return _angle;
}

double KRPTSceneItem::scale() const noexcept 
{
    return _scale;
}

double KRPTSceneItem::opaq() const noexcept 
{
    return _opaq;
}

const KRPTSceneItem::ItemsList& KRPTSceneItem::visibleChildItems() noexcept
{
    if(_childItems.empty())return _childItems;
#if 1
    if(!dirtyVisibleChilds())
        return _visibleChildItems;
    _visibleChildItems.clear();
    for(auto &item : _childItems)
    {
        item->updateCache(true);
        bool needPaint = (!item->_parent || 
            item->_state[State::VisibledInView, State::NeedChildPaint]) && _visible;
        if(needPaint)
            _visibleChildItems.emplace_back(item);
    }
    return _visibleChildItems;
#else
    _visibleChildItems.clear();
    for(auto &item : _childItems)
    {
        item->updateCache(true);
        _visibleChildItems.emplace_back(item);
    }
    return _visibleChildItems;
#endif
}

const QTransform& KRPTSceneItem::transform() noexcept 
{
#if 0
    if(!_dirty[Dirty::Transform])return _transform;
    _dirty -= Dirty::Transform;
#else
    if(!dirtyTransform())return _transform;
#endif
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

const QTransform& KRPTSceneItem::sceneTransform() noexcept 
{
    updateCache();
    return _sceneTransform;
}

const QTransform& KRPTSceneItem::sceneTransformInv() noexcept 
{
//    updateCache();
    if(_dirty[Dirty::SceneTransformInv])
    {
        _dirty -= Dirty::SceneTransformInv;
        _sceneTransformInv = sceneTransform().inverted();
    }
    return _sceneTransformInv;
}

QRectF KRPTSceneItem::bBox() noexcept
{
    if(!_dirty[Dirty::BBox])return _bBox;
    _dirty -= Dirty::BBox;
    QTransform t;
    transform(_rect, _angle, _scale, t);
    bBox(t, _rect, _bBox);
    return _bBox;
}

QRectF KRPTSceneItem::bBoxMapToParent() noexcept
{
    if(!_dirty[Dirty::BBoxMapToParent])return _bBoxMapToParent;
    _dirty -= Dirty::BBoxMapToParent;
    _bBoxMapToParent = bBox().translated(pos());
    return _bBoxMapToParent;
}

void KRPTSceneItem::setVisible(bool visible) noexcept
{
    _visible = visible;
}

bool KRPTSceneItem::setGeometry(const QRectF &geometry, 
    uint32_t time, QEasingCurve curve) noexcept
{
    if(mustAnim(time))
    {
        _data->startAnim(AnimDst::Geometry, _geometry, geometry, time, curve);
        return true;
    }
    if(must(KRPTSceneItem::Must::Anim))_data->stopAnim(AnimDst::Geometry);
    return setGeometryImpl(geometry);
}

bool KRPTSceneItem::setGeometry(const QPointF &pos, const QSizeF &size, 
    uint32_t time, QEasingCurve curve) noexcept
{
    return setGeometry(QRectF(pos, size), time, curve);
}

bool KRPTSceneItem::setGeometry(double x, double y, double w, double h, 
    uint32_t time, QEasingCurve curve) noexcept
{
    return setGeometry(QPointF(x, y), QSizeF(w, h), time, curve);
}

void KRPTSceneItem::setPos(const QPointF &pos, uint32_t time, QEasingCurve curve) noexcept
{
    setGeometry(QRectF(pos, _geometry.size()), time, curve);
}

void KRPTSceneItem::setPos(double x, double y, uint32_t time, QEasingCurve curve) noexcept
{
    setPos(QPointF(x, y), time, curve);
}

void KRPTSceneItem::setSize(const QSizeF &size, uint32_t time, QEasingCurve curve) noexcept
{
    setGeometry(QRectF(_geometry.topLeft(), size), time, curve);
}

void KRPTSceneItem::setSize(double w, double h, uint32_t time, QEasingCurve curve) noexcept
{
    setSize(QSize(w, h), time, curve);
}

void KRPTSceneItem::setX(double x, uint32_t time, QEasingCurve curve) noexcept
{
    setPos(QPointF(x, _geometry.y()), time, curve);
}

void KRPTSceneItem::setY(double y, uint32_t time, QEasingCurve curve) noexcept
{
    setPos(QPointF(_geometry.x(), y), time, curve);
}

void KRPTSceneItem::setWidth(double w, uint32_t time, QEasingCurve curve) noexcept
{
    setSize(QSizeF(w, _geometry.height()), time, curve);
}

void KRPTSceneItem::setHeight(double h, uint32_t time, QEasingCurve curve) noexcept
{
    setSize(QSizeF(_geometry.width(), h), time, curve);
}

bool KRPTSceneItem::setAngle(double angle, uint32_t time, QEasingCurve curve) noexcept
{
    if(mustAnim(time))
    {
        _data->startAnim(AnimDst::Angle, _angle, angle, time, curve);
        return true;
    }
    if(must(KRPTSceneItem::Must::Anim))_data->stopAnim(AnimDst::Angle);
    return setAngleImpl(angle);
}

bool KRPTSceneItem::setScale(double scale, uint32_t time, QEasingCurve curve) noexcept
{
    if(mustAnim(time))
    {
        _data->startAnim(AnimDst::Scale, _scale, scale, time, curve);
        return true;
    }
    if(must(KRPTSceneItem::Must::Anim))_data->stopAnim(AnimDst::Scale);
    return setScaleImpl(scale);
}

bool KRPTSceneItem::setOpaq(double opaq, uint32_t time, QEasingCurve curve) noexcept
{
    if(mustAnim(time))
    {
        _data->startAnim(AnimDst::Opaq, _opaq, opaq, time, curve);
        return true;
    }
    if(must(KRPTSceneItem::Must::Anim))_data->stopAnim(AnimDst::Opaq);
    return setOpaqImpl(opaq);
}

void KRPTSceneItem::translate(const QPointF &pos, uint32_t time, QEasingCurve curve) noexcept
{
    setPos(_geometry.topLeft() + pos, time, curve);
}

void KRPTSceneItem::translate(double x, double y, uint32_t time, QEasingCurve curve) noexcept
{
    translate(QPointF(x, y), time, curve);
}

void KRPTSceneItem::rotate(double angle, uint32_t time, QEasingCurve curve) noexcept
{
    setAngle(_angle + angle, time, curve);
}

void KRPTSceneItem::rotateAround(double angle, const QPointF &pt, 
    TransSrc src, uint32_t time, QEasingCurve curve) noexcept
{
    lockUpdate(true);
    QTransform t;
    transform(_geometry, angle + _angle, _scale, t);
    translate(transformShift(t, src, pt), time, curve);
    rotate(angle, time, curve);
    lockUpdate(false);
}

void KRPTSceneItem::scaleMul(double scale, uint32_t time, QEasingCurve curve) noexcept
{
    setScale(_scale * scale, time, curve);
}

void KRPTSceneItem::scaleFromPoint(double scale, const QPointF &pt, 
    TransSrc src, uint32_t time, QEasingCurve curve) noexcept
{
#if 1
    lockUpdate(true);
    QTransform t;
    transform(_geometry, _angle, _scale * scale, t);
    translate(transformShift(t, src, pt), time, curve);
    scaleMul(scale, time, curve);
    lockUpdate(false);
#else
    QTransform t;
    transform(_geometry, _angle, scale, t);
    _geometry.translate(transformShift(t, src, pt));
    setScale(scale);
#endif
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
//    if(!_updateLocked)update();
}

QPointF KRPTSceneItem::mapToParent(const QPointF &point) noexcept 
{
    return transform().map(point);
}

QPolygonF KRPTSceneItem::mapToParent(const QRectF &rect) noexcept 
{
    return transform().map(rect);
}

QPolygonF KRPTSceneItem::mapToParent(const QPolygonF &polygon) noexcept 
{
    return transform().map(polygon);
}

QPointF KRPTSceneItem::mapFromParent(const QPointF &point) noexcept 
{
    return transformInv().map(point);
}

QPolygonF KRPTSceneItem::mapFromParent(const QRectF &rect) noexcept 
{
    return transformInv().map(rect);
}

QPolygonF KRPTSceneItem::mapFromParent(const QPolygonF &polygon) noexcept 
{
    return transformInv().map(polygon);
}

QPointF KRPTSceneItem::mapToScene(const QPointF &point) noexcept 
{
    return sceneTransform().map(point);
}

QPolygonF KRPTSceneItem::mapToScene(const QRectF &rect) noexcept 
{
    return sceneTransform().map(rect);
}

QPolygonF KRPTSceneItem::mapToScene(const QPolygonF &polygon) noexcept 
{
    return sceneTransform().map(polygon);
}

QPointF KRPTSceneItem::mapFromScene(const QPointF &point) noexcept 
{
    return sceneTransformInv().map(point);
}

QPolygonF KRPTSceneItem::mapFromScene(const QRectF &rect) noexcept 
{
    return sceneTransformInv().map(rect);
}

QPolygonF KRPTSceneItem::mapFromScene(const QPolygonF &polygon) noexcept 
{
    return sceneTransformInv().map(polygon);
}

bool KRPTSceneItem::needPaint() const noexcept
{
    bool needPaint = !must(KRPTSceneItem::Must::NoPaint) && (!_parent || _state[State::NeedPaint]) && _visible;
    return needPaint;
}

bool KRPTSceneItem::needChildPaint() const noexcept
{
    bool needPaint = (!_parent || _state[State::NeedChildPaint]) && _visible;
    return needPaint;
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

#if 0
void KRPTSceneItem::parentTransformEvent(SceneTransformEvent *e) noexcept
{
}
#endif

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
    _dirty += Dirty::VisibleChildItems;
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
    _dirty += Dirty::VisibleChildItems;
    updateGeometry();
    return true;
}

bool KRPTSceneItem::setGeometryImpl(const QRectF &geometry) noexcept
{
    bool isMove   = !qFuzzyCompare(_geometry.topLeft(), geometry.topLeft());
    bool isResize = !qFuzzyCompare(_geometry.size   (), geometry.size   ());
    if(!isMove && ! isResize)return false;
    QRectF oldGeometry = _geometry;
    _geometry = geometry;
    _rect.setSize(_geometry.size());
    _dirty += Dirty::Transform        ;
    _dirty += Dirty::TransformInv     ;
    _dirty += Dirty::BBoxMapToParent  ;
    _dirty += Dirty::VisibleChildItems;
    if(_parent)
        _parent->_dirty += Dirty::VisibleChildItems;
    ++_data->genTransform;
    if(isResize)_dirty += Dirty::BBox;
    if(must(Must::TransformEvent) || (_parent && _parent->must(Must::ChildTransformEvent)))
    {
        SceneTransformEvent::Ptr e = SceneTransformEvent::get(geometry, oldGeometry, 
            _angle, _angle, _scale, _scale, isMove, isResize, false, false);
        if(must(Must::TransformEvent))transformImpl(e.get());
        if(_parent && _parent->must(Must::ChildTransformEvent))
            _parent->childTransformEvent(this, e.get());
    }
    return true;
}

bool KRPTSceneItem::setAngleImpl(double angle) noexcept
{
    if(qFuzzyCompare(_angle, angle))return false;
    double oldAngle = _angle;
    _dirty += Dirty::Transform        ;
    _dirty += Dirty::TransformInv     ;
    _dirty += Dirty::BBox             ;
    _dirty += Dirty::BBoxMapToParent  ;
    _dirty += Dirty::VisibleChildItems;
    if(_parent)
        _parent->_dirty += Dirty::VisibleChildItems;
    ++_data->genTransform;
    _angle = angle;
    SceneTransformEvent::Ptr e = SceneTransformEvent::get(_geometry, _geometry, 
        _angle, oldAngle, _scale, _scale, false, false, true, false);
    if(must(Must::TransformEvent))transformImpl(e.get());
    return true;
}

bool KRPTSceneItem::setScaleImpl(double scale) noexcept
{
    if(qFuzzyCompare(_scale, scale))return false;
    double oldScale = _scale;
    _dirty += Dirty::Transform        ;
    _dirty += Dirty::TransformInv     ;
    _dirty += Dirty::BBox             ;
    _dirty += Dirty::BBoxMapToParent  ;
    _dirty += Dirty::VisibleChildItems;
    if(_parent)
        _parent->_dirty += Dirty::VisibleChildItems;
    ++_data->genTransform;
    ++_data->genScale;
    _scale = scale;
    SceneTransformEvent::Ptr e = SceneTransformEvent::get(_geometry, _geometry, 
    _angle, _angle, _scale, oldScale, false, false, false, true);
    if(must(Must::TransformEvent))transformImpl(e.get());
    return true;
}

bool KRPTSceneItem::setOpaqImpl(double opaq) noexcept
{
    if(qFuzzyCompare(_opaq, opaq))return false;
    _opaq = opaq;
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

void KRPTSceneItem::animImpl(uint32_t id, const std::vector<double> &value) noexcept
{
    switch(id)
    {
        case AnimDst::Geometry : setGeometryImpl(KRPTSceneAnim::valuesTo<QRectF>(value)); break;
        case AnimDst::Angle    : setAngleImpl   (KRPTSceneAnim::valuesTo<double>(value)); break;
        case AnimDst::Scale    : setScaleImpl   (KRPTSceneAnim::valuesTo<double>(value)); break;
        case AnimDst::Opaq     : setOpaqImpl    (KRPTSceneAnim::valuesTo<double>(value)); break;
    }
    update();
}

void KRPTSceneItem::paintBackground(QPainter &painter) noexcept
{
    painter.fillRect(_rect, _backgroundColor);
}

void KRPTSceneItem::paintForeground(QPainter &painter) noexcept
{
    QPen pen(_borderColor);
    painter.setPen(pen);
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
    double tscale = !must(KRPTSceneItem::Must::NoScale) ? scale : scale *= 1.0 / _data->sceneScale;
    transform.reset();
    transform.translate(rect.x(), rect.y());
    if(!qFuzzyIsNull(angle) || !qFuzzyCompare(tscale, 1))
    {
        double wd2 = rect.width () / 2.0;
        double hd2 = rect.height() / 2.0;
        transform.translate(wd2, hd2);
        if(!qFuzzyCompare(tscale, 1))transform.scale (tscale, tscale);
        if(!qFuzzyIsNull (angle    ))transform.rotate(angle         );
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
            if(_parent)t *= _parent->sceneTransform();
            p1 = mapFromScene(pt);
            break;
        case TransSrc::Self :
            p0 = mapToParent(pt);
            break;
    }
    return p0 - t.map(p1);
}

void KRPTSceneItem::bBox(const QTransform &transform, const QRectF &rect, QRectF &bBox) noexcept 
{
    QPointF p[4];
    p[0] = transform.map(_rect.topLeft    ());
    p[1] = transform.map(_rect.topRight   ());
    p[2] = transform.map(_rect.bottomRight());
    p[3] = transform.map(_rect.bottomLeft ());
    QPointF pMin(std::numeric_limits<double>::max(), std::numeric_limits<double>::max());
    QPointF pMax;
    for(int i = 0; i < 4; ++i)
    {
        if(p[i].x() < pMin.x())pMin.setX(p[i].x());
        if(p[i].y() < pMin.y())pMin.setY(p[i].y());
        if(p[i].x() > pMax.x())pMax.setX(p[i].x());
        if(p[i].y() > pMax.y())pMax.setY(p[i].y());
    }
    bBox.setRect(pMin.x(), pMin.y(), pMax.x() - pMin.x(), pMax.y() - pMin.y());
}

QRectF KRPTSceneItem::bBox(const QTransform &transform, const QRectF &rect) noexcept 
{
    QRectF ret;
    bBox(transform, rect, ret);
    return ret;
}

bool KRPTSceneItem::updateCache(bool visible) noexcept 
{
    if(_data->cache.empty())
    {
        KRPTSceneItem *item = this;
        while(item)
        {
            _data->cache.emplace_back(item, item->_parent);
            item = item->_parent;
        }
    }
    if(visible)
    {
        _state += State::VisibledInView;
        _state += State::NeedPaint;
        _state += State::NeedChildPaint;
    }
    bool dirty = false;
    uint32_t i = 0;
    QTransform *transform = nullptr;
    for(auto &cache : _data->cache)
    {
        if(cache.item->_dirty[Dirty::Transform] || cache.genTransform != cache.item->_data->genTransform)
        {
            dirty = true;
            cache.genTransform = cache.item->_data->genTransform;
        }
        bool dirtyVisible = false;
        if(dirty)
        {
            cache.transform = transform ? *transform * cache.item->transform() : cache.item->transform();
            bBox(cache.transform, _rect, cache.bBox);
            if(cache.parent)
                cache.genParentTransform = cache.parent->_data->genTransform;
            dirtyVisible = true;
        }else
        {
            if(visible)
            {
                if(cache.parent && cache.genParentTransform != cache.parent->_data->genTransform)
                {
                    cache.genParentTransform = cache.parent->_data->genTransform;
                    dirtyVisible = true;
                }
            }
        }
        if(cache.nextDirty)
        {
            cache.nextDirty = false;
            dirty = true;
        }
        if(visible)
        {
            if(dirtyVisible)
            {
                if(!cache.parent)cache.visible = true; else
                    cache.visible = cache.parent->must(KRPTSceneItem::Must::NoClipChilds) ? true :
                    cache.parent->_rect.intersects(cache.bBox);
            }
            if(!cache.visible)
            {
                _state -= State::VisibledInView;
                _state -= State::NeedPaint;
                if(cache.parent && !must(KRPTSceneItem::Must::NoClipChilds))
                    _state -= State::NeedChildPaint;
                cache.nextDirty = true;
                break;

            }
        }
        transform = &cache.transform;
        ++i;
    }
    if(transform && i == _data->cache.size())
    {
        _sceneTransform = *transform;
        _dirty += Dirty::SceneTransformInv;
    }
    return dirty;
}

bool KRPTSceneItem::dirtyTransform() noexcept
{
    bool dirty = _dirty[Dirty::Transform];
    _dirty -= Dirty::Transform;
    if(!must(KRPTSceneItem::Must::NoScale))return dirty;
    _data->sceneScale = 1;
    for(auto &cache : _data->cache)
    {
        if(cache.parent && cache.genScale != cache.parent->_data->genScale)
        {
            dirty = true;
            cache.genScale = cache.parent->_data->genScale;
        }
        if(cache.item != this)
        _data->sceneScale *= cache.item->_scale;
    }
    return dirty;
}

bool KRPTSceneItem::dirtyVisibleChilds() noexcept
{
    bool dirty = _dirty[Dirty::VisibleChildItems];
    _dirty -= Dirty::VisibleChildItems;
    if(dirty)return dirty;
    for(auto &cache : _data->cache)
    {
        if(cache.parent && cache.genVisibleChildItems != cache.parent->_data->genTransform)
        {
            dirty = true;
            cache.genVisibleChildItems = cache.parent->_data->genTransform;
            break;
        }
    }
    return dirty;
}

bool KRPTSceneItem::mustAnim(uint32_t time) const noexcept
{
    return time > 0 && _visible && must(KRPTSceneItem::Must::Anim);
}


