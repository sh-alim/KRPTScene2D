//####################################################################################################
//#
//####################################################################################################

#include "KRPTSceneItem.h"
#include "KRPTScene.h"
#include "KRPTSceneAnim.h"

//####################################################################################################
//#
//####################################################################################################

static bool qFuzzyCompare(const std::vector<double> &v1, const std::vector<double> &v2) noexcept
{
    if(v1.size() != v2.size())return false;
    for(size_t i = 0; i < v1.size(); ++i)
        if(!qFuzzyCompare(v1.at(i), v2.at(i)))return false;
    return true;
}

static bool qFuzzyCompare(const QColor &s1, const QColor &s2) noexcept
{
    return s1 == s2;
}

//####################################################################################################
//#
//####################################################################################################

class KRPTSceneItemData
{
friend class KRPTSceneItem;
public:
    KRPTSceneItemData(KRPTSceneItem *owner)
        : owner(owner), genTransform(0), genScale(0), genAngle(0), sceneScale(1), sceneAngle(0) {}
public:
    struct Cache
    {
        Cache(KRPTSceneItem *item, KRPTSceneItem *parent) 
            : item(item), parent(parent), genTransform(0), genParentTransform(0), 
              genVisibleChildItems(0), genScale(0), genAngle(0), visible(false), 
              nextDirty(false), visibleDirty(false){}
        KRPTSceneItem *item                ;
        KRPTSceneItem *parent              ;
        QTransform     transform           ;
        uint32_t       genTransform        ;
        uint32_t       genParentTransform  ;
        uint32_t       genVisibleChildItems;
        uint32_t       genScale            ;
        uint32_t       genAngle            ;  
        QRectF         bBox                ;
        bool           visible             ;
        bool           nextDirty           ;
        bool           visibleDirty        ;
    };
    struct Anim
    {
        using Values = std::vector<double>;
        using Map = std::map<uint32_t, std::unique_ptr<Anim>>;
        Anim (uint32_t id, KRPTSceneAnim::Event &event, 
              int time, QEasingCurve easingCurve) noexcept 
            : anim(new KRPTSceneAnim(id, event, time, easingCurve)) {}
        ~Anim() noexcept {if(anim)anim->deleteLater();}
        inline int time() const {return anim ? anim->duration() : 0;}
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
    void startAnim(uint32_t id, const T &start, const T &end, uint32_t time, QEasingCurve curve)
    {
        if(qFuzzyCompare(start, end))
        {
            if constexpr (std::is_same_v<std::remove_reference_t<T>, std::vector<double>>)
            {
                owner->animImpl(id, end, 0, true);
            }else
            {
                std::vector<double> value;
                KRPTSceneAnim::valuesFrom(end, value);
                owner->animImpl(id, value, 0, true);
            }
            deleteAnim(id);
            return;
        }
        Anim *anim = addAnim(id, time, curve);
        KRPTSceneAnim *sceneAnim = anim->anim;
        sceneAnim->stop();
        anim->update(start, end);
        sceneAnim->setDuration(time);
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
    Anim* addAnim(uint32_t id, int time, QEasingCurve easingCurve) noexcept
    {
        auto findAnim = anims.find(id);
        if(findAnim == anims.end())
        {
            if(!animFunction)animFunction = std::bind(&KRPTSceneItemData::animEvent, this, 
                std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
            findAnim = anims.emplace(id, std::make_unique<KRPTSceneItemData::Anim>
                                    (id, animFunction, time, easingCurve)).first;
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
        if(anim->time() == time)
        {
            owner->animImpl(id, anim->end, time, true);
            deleteAnim(id);
        }else owner->animImpl(id, anim->current, time, false);
    }
private:
    KRPTSceneItem       *owner       ;
    std::list<Cache>     cache       ;
    uint32_t             genTransform;
    uint32_t             genScale    ;
    uint32_t             genAngle    ;
    double               sceneScale  ;
    double               sceneAngle  ;
    Anim::Map            anims       ;
    KRPTSceneAnim::Event animFunction;
};

//####################################################################################################
//#
//####################################################################################################

KRPTSceneItem::KRPTSceneItem(KRPTScene *scene, KRPTSceneItem *parent) noexcept
    : _scene(scene), _parent(parent), _data(new KRPTSceneItemData(this)), _dirty(Dirty::All), 
      _state(State::NeedPaint | State::VisibledInView | State::NeedChildPaint),
      _updateLocked(0), _visible(true), _angle(0), _scale(1), _opaq(1), _tag(0),
      _borderColor(255, 255, 255), _backgroundColor(30, 30, 30)
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

uint32_t KRPTSceneItem::tag() const noexcept
{
    return _tag;
}

void KRPTSceneItem::setTag(uint32_t tag) noexcept
{
    _tag = tag;
}

const KRPTSceneItem::ItemsList& KRPTSceneItem::visibleChildItems() noexcept
{
    if(_childItems.empty() || must(KRPTSceneItem::Must::NoCheckChildVisibled))
        return _childItems;
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
#if 0
    _sceneTransform = _parent ? transform() * _parent->sceneTransform() : transform();
    return _sceneTransform;
#else
    updateCache();
    return _sceneTransform;
#endif
}

const QTransform& KRPTSceneItem::sceneTransformInv() noexcept 
{
#if 0
    _sceneTransformInv = sceneTransform().inverted();
    return _sceneTransformInv;
#else
//    updateCache();
    if(_dirty[Dirty::SceneTransformInv])
    {
        _dirty -= Dirty::SceneTransformInv;
        _sceneTransformInv = sceneTransform().inverted();
    }
    return _sceneTransformInv;
#endif
}

QRectF KRPTSceneItem::bBox() noexcept
{
#if 0
    if(!_dirty[Dirty::BBox])return _bBox;
    _dirty -= Dirty::BBox;
    QTransform t;
    transform(_rect, _angle, _scale, t);
    bBox(t, _rect, _bBox);
    return _bBox;
#else
    if(!_dirty[Dirty::BBox])return _bBox;
    _dirty -= Dirty::BBox;
    updateCache();
    return _bBox;
#endif
}

QRectF KRPTSceneItem::bBoxMapToParent() noexcept
{
#if 0
    if(!_dirty[Dirty::BBoxMapToParent])return _bBoxMapToParent;
    _dirty -= Dirty::BBoxMapToParent;
    _bBoxMapToParent = bBox().translated(pos());
    return _bBoxMapToParent;
#else
    if(!_dirty[Dirty::BBoxMapToParent])return _bBoxMapToParent;
    _dirty -= Dirty::BBoxMapToParent;
    updateCache();
    return _bBoxMapToParent;
#endif
}

bool KRPTSceneItem::contains(const QPointF &point) noexcept
{
    bool ret = _rect.contains(point);
    if(ret && must(KRPTSceneItem::Must::AccuracyCheckContains))
        ret = outline().contains(point);
    return ret;
}

const QPainterPath & KRPTSceneItem::outline() noexcept
{
    if(!_dirty[Dirty::Outline])return _outline;
    _dirty -= Dirty::Outline;
    _outline.clear();
    outlineImpl();
    return _outline;
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
    lockUpdate(true);
    QTransform t;
    transform(_geometry, _angle, _scale * scale, t);
    translate(transformShift(t, src, pt), time, curve);
    scaleMul(scale, time, curve);
    lockUpdate(false);
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
    if(lock)++_updateLocked; else
    if(_updateLocked > 0)--_updateLocked;
    if(_updateLocked == 0)update();
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

QPointF KRPTSceneItem::mapToItem(KRPTSceneItem *item, const QPointF &point) noexcept
{
    return item == _parent ? mapToParent(point) : item->mapFromScene(mapToScene(point));
}

QPolygonF KRPTSceneItem::mapToItem(KRPTSceneItem *item, const QRectF &rect) noexcept
{
    return item == _parent ? mapToParent(rect) : item->mapFromScene(mapToScene(rect));
}

QPolygonF KRPTSceneItem::mapToItem(KRPTSceneItem *item, const QPolygonF &polygon) noexcept
{
    return item == _parent ? mapToParent(polygon) : item->mapFromScene(mapToScene(polygon));
}

QPointF KRPTSceneItem::mapFromItem(KRPTSceneItem *item, const QPointF &point) noexcept
{
    return item == _parent ? mapFromParent(point) : mapFromScene(item->mapToScene(point));
}

QPolygonF KRPTSceneItem::mapFromItem(KRPTSceneItem *item, const QRectF &rect) noexcept
{
    return item == _parent ? mapFromParent(rect) : mapFromScene(item->mapToScene(rect));
}

QPolygonF KRPTSceneItem::mapFromItem(KRPTSceneItem *item, const QPolygonF &polygon) noexcept
{
    return item == _parent ? mapFromParent(polygon) : mapFromScene(item->mapToScene(polygon));
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
    if(_updateLocked != 0)return;
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
    bool isMoved   = !qFuzzyCompare(_geometry.topLeft(), geometry.topLeft());
    bool isResized = !qFuzzyCompare(_geometry.size   (), geometry.size   ());
    if(!isMoved && ! isResized)return false;
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
    if(isResized)
    {
        _dirty += Dirty::BBox   ;
        _dirty += Dirty::Outline;
    }
    if(must(Must::TransformEvent) || (_parent && _parent->must(Must::ChildTransformEvent)))
    {
        SceneTransformEvent::Ptr e = SceneTransformEvent::get(geometry, oldGeometry, 
            _angle, _angle, _scale, _scale, isMoved, isResized, false, false);
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
    ++_data->genAngle;
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

void KRPTSceneItem::outlineImpl() noexcept
{
    _outline.addRect(_rect);
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

void KRPTSceneItem::animImpl(uint32_t id, const std::vector<double> &value, 
    uint32_t time, bool completed) noexcept
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
    QPen pen(_borderColor, 2);
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
    double tscale = !must(KRPTSceneItem::Must::NoSceneScale ) ? scale : scale *= 1.0 / _data->sceneScale;
    double tangle = !must(KRPTSceneItem::Must::NoSceneRotate) ? angle : angle -= _data->sceneAngle;
    transform.reset();
    transform.translate(rect.x(), rect.y());
    if(!qFuzzyIsNull(angle) || !qFuzzyCompare(tscale, 1))
    {
        double wd2 = rect.width () / 2.0;
        double hd2 = rect.height() / 2.0;
        transform.translate(wd2, hd2);
        if(!qFuzzyCompare(tscale, 1))transform.scale (tscale, tscale);
        if(!qFuzzyIsNull (tangle   ))transform.rotate(tangle        );
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
        if(dirty)
        {
            cache.transform = transform ? *transform * cache.item->transform() : cache.item->transform();
            bBox(cache.transform, _rect, cache.bBox);
        #if 1
            if(cache.item == this)
            {
                _bBoxMapToParent = cache.bBox;
                _bBox = _bBoxMapToParent.translated(-_geometry.topLeft());
                _dirty -= Dirty::BBox;
                _dirty -= Dirty::BBoxMapToParent;
            }
        #endif
            if(cache.parent)
                cache.genParentTransform = cache.parent->_data->genTransform;
            cache.visibleDirty = true;
        }else
        {
            if(visible)
            {
                if(cache.parent && cache.genParentTransform != cache.parent->_data->genTransform)
                {
                    cache.genParentTransform = cache.parent->_data->genTransform;
                    cache.visibleDirty = true;
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
            if(cache.visibleDirty)
            {
                if(!cache.parent)cache.visible = true; else
                    cache.visible = cache.parent->must(KRPTSceneItem::Must::NoClipChilds) ? true :
                    cache.parent->_rect.intersects(cache.bBox);
                cache.visibleDirty = false;
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
        }else cache.visibleDirty = true;
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
    if(!must(KRPTSceneItem::Must::NoSceneScale) && 
       !must(KRPTSceneItem::Must::NoSceneRotate))return dirty;
    _data->sceneScale = 1;
    _data->sceneAngle = 0;
    for(auto &cache : _data->cache)
    {
        if(must(KRPTSceneItem::Must::NoSceneScale))
        {
            if(cache.parent && cache.genScale != cache.parent->_data->genScale)
            {
                dirty = true;
                cache.genScale = cache.parent->_data->genScale;
            }
            if(cache.item != this)
                _data->sceneScale *= cache.item->_scale;
        }
        if(must(KRPTSceneItem::Must::NoSceneRotate))
        {
            if(cache.parent && cache.genAngle != cache.parent->_data->genAngle)
            {
                dirty = true;
                cache.genAngle = cache.parent->_data->genAngle;
            }
            if(cache.item != this)
                _data->sceneAngle += cache.item->_angle;
        }
    }
    if(dirty)
    {
        _dirty += Dirty::TransformInv;
    #if 1
//        _dirty += Dirty::BBox;
//        _dirty += Dirty::BBoxMapToParent;
//        _dirty += Dirty::Transform;
    #endif
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

//****************************************************************************************************
//*
//****************************************************************************************************

void KRPTSceneItem::startAnimImpl(uint32_t id, const std::vector<double> &start, 
    const std::vector<double> &end, uint32_t time, QEasingCurve curve) noexcept
{
    _data->startAnim(id, start, end, time, curve);
}

void KRPTSceneItem::startAnimImpl(uint32_t id, double start, 
    double end, uint32_t time, QEasingCurve curve) noexcept
{
    _data->startAnim(id, start, end, time, curve);
}

void KRPTSceneItem::startAnimImpl(uint32_t id, const QPointF &start, 
    const QPointF &end, uint32_t time, QEasingCurve curve) noexcept
{
    _data->startAnim(id, start, end, time, curve);
}

void KRPTSceneItem::startAnimImpl(uint32_t id, const QRectF &start, 
    const QRectF &end, uint32_t time, QEasingCurve curve) noexcept
{
    _data->startAnim(id, start, end, time, curve);
}

void KRPTSceneItem::startAnimImpl(uint32_t id, const QSizeF &start, 
    const QSizeF &end, uint32_t time, QEasingCurve curve) noexcept
{
    _data->startAnim(id, start, end, time, curve);
}

void KRPTSceneItem::startAnimImpl(uint32_t id, const QColor &start, 
    const QColor &end, uint32_t time, QEasingCurve curve) noexcept
{
    _data->startAnim(id, start, end, time, curve);
}

