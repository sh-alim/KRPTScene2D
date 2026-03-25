//########################################################################################################################
//#
//########################################################################################################################

#include "KRPTSceneItem.h"
#include "KRPTScene.h"
#include "KRPTSceneAnim.h"

#ifdef _OPENMP
#include <omp.h>
#endif

//########################################################################################################################
//#
//########################################################################################################################

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

static inline void project(const std::array<QPointF, 5> &pts, const QPointF &axis, double &min, double &max) 
{
    min = max = QPointF::dotProduct(pts[0], axis);
    for(size_t i = 1; i < pts.size(); ++i) 
    {
        double p = QPointF::dotProduct(pts[i], axis);
        if(p < min)min = p;
        if(p > max)max = p;
    }
}

static inline bool intersect(const QRectF &rect, const std::array<QPointF, 5> &p) 
{
    QPointF axes[4] = {{1, 0}, {0, 1},
        {p[1].x() - p[0].x(), p[1].y() - p[0].y()},
        {p[3].x() - p[0].x(), p[3].y() - p[0].y()}};
    std::array<QPointF, 5> r = {rect.topLeft    (), rect.topRight  (),
                                rect.bottomRight(), rect.bottomLeft(), rect.center()};
    for(auto &ax : axes) 
    {
        double minA, maxA, minB, maxB;
        project(r, ax, minA, maxA);
        project(p, ax, minB, maxB);
        if(maxA < minB || maxB < minA)return false;
    }
    return true;
}

//########################################################################################################################
//#
//########################################################################################################################

class KRPTSceneItemData
{
friend class KRPTSceneItem;
public:
    KRPTSceneItemData(KRPTSceneItem *owner)
        : owner(owner), genTransform(0), genScale(0), genAngle(0), sceneScale(1), sceneAngle(0) {}
   ~KRPTSceneItemData() noexcept
   {
       for(auto &anim : anims)
       {
           delete anim.second->anim;
           anim.second->anim = nullptr;
       }
       anims.clear();
   }
public:
    struct Cache
    {
        Cache(KRPTSceneItem *item, KRPTSceneItem *parent) 
            : item(item), parent(parent), scale(1), angle(0), genTransform(0), genParentTransform(0), 
              genVisibleChildItems(0), genScale(0), genAngle(0), visible(false), dirty(true), visibleDirty(false){}
        KRPTSceneItem *item                ;
        KRPTSceneItem *parent              ;
        QTransform     transform           ;
        double         scale               ;
        double         angle               ;
        uint32_t       genTransform        ;
        uint32_t       genParentTransform  ;
        uint32_t       genVisibleChildItems;
        uint32_t       genScale            ;
        uint32_t       genAngle            ;  
        QRectF         bBox                ;
        bool           visible             ;
        bool           dirty               ;
        bool           visibleDirty        ;


        std::array<QPointF, 5>  points; //!!!!!

    };
    struct Anim
    {
        using Values = std::vector<double>;
        using Map = std::unordered_map<uint32_t, std::unique_ptr<Anim>>;
        Anim (uint32_t id, KRPTSceneAnim::Event &event, 
              int time, QEasingCurve easingCurve, int loopCount) noexcept 
            : anim(new KRPTSceneAnim(id, event, time, easingCurve, loopCount)) {}
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
    void startAnim(uint32_t id, const T &start, const T &end, uint32_t time, 
        QEasingCurve curve, int loopCount = 1)
    {
        if(qFuzzyCompare(start, end))
        {
            if constexpr (std::is_same_v<std::remove_reference_t<T>, std::vector<double>>)
            {
                owner->animImpl(id, end, 0, true, 0);
            }else
            {
                std::vector<double> value;
                KRPTSceneAnim::valuesFrom(end, value);
                owner->animImpl(id, value, 0, true, 0);
            }
            deleteAnim(id);
            return;
        }
        Anim *anim = addAnim(id, time, curve, loopCount);
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
    Anim* addAnim(uint32_t id, int time, QEasingCurve easingCurve, int loopCount) noexcept
    {
        auto findAnim = anims.find(id);
        if(findAnim == anims.end())
        {
            if(!animFunction)animFunction = std::bind(&KRPTSceneItemData::animEvent, this, 
                std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, 
                std::placeholders::_4, std::placeholders::_5);
            findAnim = anims.emplace(id, std::make_unique<KRPTSceneItemData::Anim>
                                    (id, animFunction, time, easingCurve, loopCount)).first;
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
    void animEvent(uint32_t id, int time, double progress, int loopCount, int currentLoop) noexcept
    {
        auto anim = this->anim(id);
        if(!anim)return;
        KRPTSceneAnim::interpolate(anim->start, anim->end, progress, anim->current);
        if(anim->time() == time && loopCount != -1 && currentLoop == loopCount - 1)
        {
            owner->animImpl(id, anim->end, time, true, currentLoop);
            deleteAnim(id);
        }else owner->animImpl(id, anim->current, time, false, currentLoop);
    }
private:
    KRPTSceneItem            *owner       ;
    std::array<QTransform, 9> transforms  ;
    std::list<Cache>          cache       ;
    uint32_t                  genTransform;
    uint32_t                  genScale    ;
    uint32_t                  genAngle    ;
    double                    sceneScale  ;
    double                    sceneAngle  ;
    Anim::Map                 anims       ;
    KRPTSceneAnim::Event      animFunction;
};

//########################################################################################################################
//#
//########################################################################################################################

KRPTSceneItem::KRPTSceneItem(KRPTScene *scene, KRPTSceneItem *parent) noexcept
    : _scene(scene), _parent(parent), _data(new KRPTSceneItemData(this)), _dirty(Dirty::All), 
      _state(State::NeedPaint | State::VisibledInView | State::NeedChildPaint),
      _updateLocked(0), _visible(true), _angle(0), _scale(1), _opaq(1), 
      _transformAnchor(TransformAnchor::Center), _posAnchor(TransformAnchor::LeftTop), 
      _paintStageCount(1), _tag(0), _borderColor(255, 255, 255), _backgroundColor(30, 30, 30)
{
}

KRPTSceneItem::~KRPTSceneItem() noexcept
{
    for(auto &child : _childItems)
        delete child;
    delete _data;
}

//************************************************************************************************************************
//*
//************************************************************************************************************************

bool KRPTSceneItem::delChild(KRPTSceneItem *item) noexcept
{
    return delChildImpl(item, this);
}

//************************************************************************************************************************
//*
//************************************************************************************************************************

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
    const auto &childItems = filterChildItems();

    if(childItems.empty() || must(KRPTSceneItem::Must::NoCheckChildVisibled))
        return childItems;
    if(!dirtyVisibleChilds())return _visibleChildItems;
    _visibleChildItems.clear();
#ifndef _OPENMP
    for(auto &item : childItems)
    {
        item->updateCache(true);
        bool needPaint = (!item->_parent || 
            item->_state[State::VisibledInView, State::NeedChildPaint]) && _visible;
        if(needPaint)
            _visibleChildItems.emplace_back(item);
    }
#else
    if(childItems.size() < 500)
    {
        for(auto &item : childItems)
        {
            item->updateCache(true);
            bool needPaint = (!item->_parent || 
                item->_state[State::VisibledInView, State::NeedChildPaint]) && _visible;
            if(needPaint)
                _visibleChildItems.emplace_back(item);
        }
        return _visibleChildItems;
    }
    uint32_t threadCount = std::min(4, omp_get_num_procs());
    size_t chsz = childItems.size() / threadCount;
    struct Rec
    {
        ItemsList::const_iterator begin;
        ItemsList::const_iterator end  ;
        ItemsList                 list ;
    };
    std::vector<Rec> rec(threadCount);
    ItemsList::const_iterator it = childItems.begin();
    for(uint32_t i = 0; i < threadCount; ++i)
    {
        rec[i].begin = it;
        rec[i].end = i < threadCount - 1 ? std::next(rec[i].begin, chsz) : childItems.end();
        it = rec[i].end;
    }
    #pragma omp parallel num_threads(threadCount)
    {
        int tid = omp_get_thread_num();
        Rec &r = rec[tid];
        for(auto it = r.begin; it != r.end; ++it)
        {
            auto item = *it;
            item->updateCache(true);
            bool needPaint = (!item->_parent || 
                item->_state[State::VisibledInView, State::NeedChildPaint]) && _visible;
            if(needPaint)
                r.list.emplace_back(item);
        }
    }
    for(auto &r : rec)
        _visibleChildItems.splice(_visibleChildItems.end(), r.list);
#endif
    return _visibleChildItems;
}

KRPTSceneItem::TransformAnchor KRPTSceneItem::transformAnchor() const noexcept
{
    return _transformAnchor;
}

KRPTSceneItem::TransformAnchor KRPTSceneItem::posAnchor() const noexcept
{
    return _posAnchor;
}

const QTransform& KRPTSceneItem::transform() noexcept 
{
    if(!dirtyTransform())return _transform;
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
#if 1
    transform();
    if(!_dirty[Dirty::SceneTransform])return _sceneTransform;
    _dirty -= Dirty::SceneTransform;
    updateCache();
    return _sceneTransform;
#else
    _sceneTransform = _parent ? transform() * _parent->sceneTransform() : transform();
    return _sceneTransform;
#endif
}

const QTransform& KRPTSceneItem::sceneTransformInv() noexcept 
{
    if(!_dirty[Dirty::SceneTransformInv])_sceneTransformInv;
    _dirty -= Dirty::SceneTransformInv;
    _sceneTransformInv = sceneTransform().inverted();
    return _sceneTransformInv;
}

double KRPTSceneItem::sceneScale() noexcept 
{
    dirtyTransform();
    return _data->sceneScale;
}

double KRPTSceneItem::sceneAngle() noexcept 
{
    dirtyTransform();
    return _data->sceneAngle;
}

QRectF KRPTSceneItem::bBox() noexcept
{
    if(!_dirty[Dirty::BBox])return _bBox;
    _dirty -= Dirty::BBox;
    _bBox = bBoxMapToParent().translated(-_geometry.topLeft());
    return _bBox;
}

QRectF KRPTSceneItem::bBoxMapToParent() noexcept
{
    if(!_dirty[Dirty::BBoxMapToParent])return _bBoxMapToParent;
    _dirty -= Dirty::BBoxMapToParent;
    bBox(transform(), _rect, _bBoxMapToParent);
    return _bBoxMapToParent;
}

bool KRPTSceneItem::contains(const QPointF &point) noexcept
{
    bool ret = _rect.contains(point);
    if(ret && must(KRPTSceneItem::Must::AccuracyCheckContains))
        ret = outline().contains(point);
    return ret;
}

const QPainterPath &KRPTSceneItem::outline() noexcept
{
    if(!_dirty[Dirty::Outline])return _outline;
    _dirty -= Dirty::Outline;
    _outline.clear();
    outlineImpl();
    return _outline;
}

void KRPTSceneItem::setVisible(bool visible) noexcept
{
    if(_visible == visible)return;
    _visible = visible;
    if(_parent)
        _parent->_dirty += Dirty::VisibleChildItems;
    update();
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

void KRPTSceneItem::setTransformAnchor(TransformAnchor anchor) noexcept
{
    if(anchor == _transformAnchor)return;
    _transformAnchor = anchor;
}

void KRPTSceneItem::setPosAnchor(TransformAnchor anchor) noexcept
{
    if(anchor == _posAnchor)return;
    _posAnchor = anchor;
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
    bool needPaint = !must(Must::NoPaint) && 
        (!_parent || _state[State::NeedPaint]) && _visible && 
        !qFuzzyIsNull(_opaq) && !qFuzzyIsNull(_scale);
    return needPaint;
}

bool KRPTSceneItem::canBeUpdated() const noexcept
{
    bool canBeUpdated = (!_childItems.empty() || !must(Must::NoPaint)) && 
        (!_parent ||_state[State::NeedPaint] || _dirty[Dirty::Transform]) && 
        _visible && !qFuzzyIsNull(_opaq) && !qFuzzyIsNull(_scale);
    return canBeUpdated;
}

bool KRPTSceneItem::needChildPaint() const noexcept
{
    bool needPaint = (!_parent || _state[State::NeedChildPaint]) && _visible;
    return needPaint;
}

//************************************************************************************************************************
//*
//************************************************************************************************************************

void KRPTSceneItem::update() noexcept
{
    if(_updateLocked != 0 || !canBeUpdated())return;
    _scene->update();
}

const KRPTSceneItem::ItemsList &KRPTSceneItem::filterChildItems() noexcept
{
    return _childItems;
}

void KRPTSceneItem::addChildImpl(KRPTSceneItem::Ptr item, KRPTSceneItem *parent) noexcept
{
    item->_parent = parent;
    ItemsList::iterator it = parent->_childItems.emplace(parent->_childItems.end(), item);
    parent->_index.emplace(item, it);
    addChildEvent(item);
    _dirty += Dirty::VisibleChildItems;
    update();
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
    update();
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
    _dirty += Dirty::Transform   ;
    _dirty += Dirty::TransformInv;
    if(isMoved)
        _dirty += Dirty::TransformTrans;
    if(isResized)
    {
        _dirty += Dirty::TransformSize;
        _dirty += Dirty::BBox         ;
        _dirty += Dirty::Outline      ;
    }
    _dirty += Dirty::VisibleChildItems;
    _dirty += Dirty::SceneTransform   ;
    _dirty += Dirty::SceneTransformInv;
    _dirty += Dirty::BBoxMapToParent  ;
    if(_parent)
        _parent->_dirty += Dirty::VisibleChildItems;
    ++_data->genTransform;
    if(must(Must::TransformEvent) || (_parent && _parent->must(Must::ChildTransformEvent)))
    {
        SceneTransformEvent::Ptr e = SceneTransformEvent::get(geometry, oldGeometry, 
            _angle, _angle, _scale, _scale, isMoved, isResized, false, false);
        if(must(Must::TransformEvent))transformImpl(e.get());
        if(_parent && _parent->must(Must::ChildTransformEvent))
            _parent->childTransformEvent(this, e.get());
    }
    update();
    return true;
}

bool KRPTSceneItem::setAngleImpl(double angle) noexcept
{
    if(qFuzzyCompare(_angle, angle))return false;
    double oldAngle = _angle;
    _dirty += Dirty::Transform        ;
    _dirty += Dirty::TransformRotate  ;
    _dirty += Dirty::TransformInv     ;
    _dirty += Dirty::VisibleChildItems;
    _dirty += Dirty::SceneTransform   ;
    _dirty += Dirty::SceneTransformInv;
    _dirty += Dirty::BBox             ;
    _dirty += Dirty::BBoxMapToParent  ;
    if(_parent)
        _parent->_dirty += Dirty::VisibleChildItems;
    ++_data->genTransform;
    ++_data->genAngle;
    _angle = angle;
    SceneTransformEvent::Ptr e = SceneTransformEvent::get(_geometry, _geometry, 
        _angle, oldAngle, _scale, _scale, false, false, true, false);
    if(must(Must::TransformEvent))transformImpl(e.get());
    update();
    return true;
}

bool KRPTSceneItem::setScaleImpl(double scale) noexcept
{
    if(qFuzzyCompare(_scale, scale))return false;
    double oldScale = _scale;
    _dirty += Dirty::Transform        ;
    _dirty += Dirty::TransformScale   ;
    _dirty += Dirty::TransformInv     ;
    _dirty += Dirty::SceneTransform   ;
    _dirty += Dirty::SceneTransformInv;
    _dirty += Dirty::VisibleChildItems;
    _dirty += Dirty::BBox             ;
    _dirty += Dirty::BBoxMapToParent  ;
    if(_parent)
        _parent->_dirty += Dirty::VisibleChildItems;
    ++_data->genTransform;
    ++_data->genScale;
    _scale = scale;
    SceneTransformEvent::Ptr e = SceneTransformEvent::get(_geometry, _geometry, 
    _angle, _angle, _scale, oldScale, false, false, false, true);
    if(must(Must::TransformEvent))transformImpl(e.get());
    update();
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
    uint32_t time, bool completed, int loop) noexcept
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

void KRPTSceneItem::paintBackground(QPainter &painter, uint32_t stage) noexcept
{
    painter.fillRect(_rect, _backgroundColor);
}

void KRPTSceneItem::paintForeground(QPainter &painter, uint32_t stage) noexcept
{
    QPen pen(_borderColor, 2);
    painter.setPen(pen);
    painter.drawRect(_rect);
}

//************************************************************************************************************************
//*
//************************************************************************************************************************

void KRPTSceneItem::transform(const QRectF &rect, double angle, double scale, QTransform &transform) noexcept 
{
    if(_dirty[Dirty::TransformTrans])
    {
        _data->transforms[0].reset();
        _data->transforms[0].translate(rect.x(), rect.y());
    }
    if(_dirty[Dirty::TransformScale])
    {
        _data->transforms[1].reset();
        _data->transforms[1].scale(scale, scale);

    }
    if(_dirty[Dirty::TransformRotate])
    {
        _data->transforms[2].reset();
        _data->transforms[2].rotate(angle);
    }
    if(_dirty.any(Dirty::TransformRotate, Dirty::TransformScale))
    {
        _data->transforms[3] = _data->transforms[2] * _data->transforms[1];
    }
    if(_dirty.any(Dirty::TransformRotate, Dirty::TransformScale, Dirty::TransformSize))
    {
        double dx0 = 0, dy0 = 0, dx1 = 0, dy1 = 0;
        double w = rect.width(), h = rect.height();
        double w2 = w * 0.5, h2 = h * 0.5;
        switch(_transformAnchor)
        {
            case TransformAnchor::Center       : dx0 = w2; dy0 = h2; break;
            case TransformAnchor::RightTop     : dx0 =  w; dy0 =  0; break;
            case TransformAnchor::LeftBottom   : dx0 =  0; dy0 =  h; break;
            case TransformAnchor::RightBottom  : dx0 =  w; dy0 =  h; break;
            case TransformAnchor::LeftCenter   : dx0 =  0; dy0 = h2; break;
            case TransformAnchor::RightCenter  : dx0 =  w; dy0 = h2; break;
            case TransformAnchor::TopCenter    : dx0 = w2; dy0 =  0; break;
            case TransformAnchor::BottomCenter : dx0 = w2; dy0 =  h; break;
        }
        switch(_posAnchor)
        {
            case TransformAnchor::Center       : dx1 = w2; dy1 = h2; break;
            case TransformAnchor::RightTop     : dx1 =  w; dy1 =  0; break;
            case TransformAnchor::LeftBottom   : dx1 =  0; dy1 =  h; break;
            case TransformAnchor::RightBottom  : dx1 =  w; dy1 =  h; break;
            case TransformAnchor::LeftCenter   : dx1 =  0; dy1 = h2; break;
            case TransformAnchor::RightCenter  : dx1 =  w; dy1 = h2; break;
            case TransformAnchor::TopCenter    : dx1 = w2; dy1 =  0; break;
            case TransformAnchor::BottomCenter : dx1 = w2; dy1 =  h; break;
        }
        _data->transforms[4].reset();
        _data->transforms[4].translate(dx0 - dx1, dy0 - dy1);
        _data->transforms[4] = _data->transforms[3] * _data->transforms[4];
        _data->transforms[4].translate(-dx0, -dy0);
    }
    if(must(KRPTSceneItem::Must::NoSceneScale))
    {
        if(_dirty[Dirty::SceneScale])
        {
            double scale = 1.0 / _data->sceneScale;
            _data->transforms[5].reset();
            _data->transforms[5].scale(scale, scale);
        }
        if(_dirty.any(Dirty::TransformRotate, Dirty::TransformScale, Dirty::TransformSize, Dirty::SceneScale))
        {
            _data->transforms[7] = _data->transforms[4] * _data->transforms[5];
        }
    }else _data->transforms[7] = _data->transforms[4];

    if(must(KRPTSceneItem::Must::NoSceneRotate))
    {
        if(_dirty[Dirty::SceneRotate])
        {
            _data->transforms[6].reset();
            _data->transforms[6].rotate(-_data->sceneAngle);
        }
        if(_dirty.any(Dirty::TransformRotate, Dirty::TransformScale, Dirty::TransformSize, 
                      Dirty::SceneRotate, Dirty::SceneScale))
        {
            _data->transforms[8] = _data->transforms[7] * _data->transforms[6];
        }
    }else _data->transforms[8] = _data->transforms[7];
    transform = _data->transforms[8] * _data->transforms[0];
    _dirty.down(Dirty::TransformScale, Dirty::TransformRotate, Dirty::TransformTrans,
                Dirty::TransformSize , Dirty::SceneScale     , Dirty::SceneRotate   , Dirty::Transform);
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

void KRPTSceneItem::mappedRectPoints(const QTransform &transform, const QRectF &rect, 
    std::array<QPointF, 5> &p) noexcept 
{
    p[0] = transform.map(rect.topLeft    ());
    p[1] = transform.map(rect.topRight   ());
    p[2] = transform.map(rect.bottomRight());
    p[3] = transform.map(rect.bottomLeft ());
    p[4] = transform.map(rect.center     ());
}

void KRPTSceneItem::bBox(const std::array<QPointF, 5> &p, QRectF &bBox) noexcept 
{
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

void KRPTSceneItem::bBox(const QTransform &transform, const QRectF &rect, QRectF &bBox) noexcept 
{
    std::array<QPointF, 5> p;
    mappedRectPoints(transform, rect, p);
    this->bBox(p, bBox);
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
    bool dirty = false;
    _state += State::VisibledInView;
    _state += State::NeedPaint     ;
    _state += State::NeedChildPaint;
    QTransform *transform = nullptr;
    auto it = _data->cache.begin();
    for(; it != _data->cache.end(); ++it)
    {
        auto &cache = *it;
        if(cache.dirty)
        {
            dirty = cache.dirty;
            cache.dirty = false;
        }
        if(cache.genTransform != cache.item->_data->genTransform)
        {
            dirty = true;
            cache.genTransform = cache.item->_data->genTransform;
        }
        if(dirty)
        {
            cache.transform = transform ? *transform * cache.item->transform() : cache.item->transform();
        #if 0
            bBox(cache.transform, _rect, cache.bBox);
            if(cache.item == this)
            {
                _bBoxMapToParent = cache.bBox;
                _bBox = _bBoxMapToParent.translated(-_geometry.topLeft());
                _dirty -= Dirty::BBox;
                _dirty -= Dirty::BBoxMapToParent;
            }
        #else
            mappedRectPoints(cache.transform, _rect, cache.points);
            bBox(cache.points, cache.bBox);
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
            if(cache.parent && cache.genParentTransform != cache.parent->_data->genTransform)
            {
                cache.genParentTransform = cache.parent->_data->genTransform;
                cache.visibleDirty = true;
            }
        }
        if(cache.visibleDirty)
        {
        #if 0
            if(!cache.parent)cache.visible = true; else
                cache.visible = cache.parent->must(KRPTSceneItem::Must::NoClipChilds) ? true :
                cache.parent->_rect.intersects(cache.bBox);
        #else
            if(!cache.parent || cache.parent->must(KRPTSceneItem::Must::NoClipChilds))cache.visible = true; else
            {
                cache.visible = cache.parent->_rect.intersects(cache.bBox);
//                if(cache.visible && !cache.parent->_rect.contains(cache.points[4]))
//                {
//                    cache.visible = intersect(cache.parent->_rect, cache.points);
//                }
            }
        #endif
            cache.visibleDirty = false;
        }
        if(!cache.visible)
        {
            _state -= State::VisibledInView;
            _state -= State::NeedPaint;
            if(cache.parent && !must(KRPTSceneItem::Must::NoClipChilds))
                _state -= State::NeedChildPaint;
            if(visible)
            {
                auto next = std::next(it);
                if(next != _data->cache.end())
                    next->dirty = true;
                break;
            }
        }
        transform = &cache.transform;
    }
    if(transform && it == _data->cache.end())
    {
        _sceneTransform = *transform;
        _dirty -= Dirty::SceneTransform;
        _dirty += Dirty::SceneTransformInv;
        if(must(KRPTSceneItem::Must::SceneTransformEvent))
            sceneTransformEvent(_sceneTransform);
    }
    return dirty;
}

bool KRPTSceneItem::dirtyTransform() noexcept
{
    bool dirty = _dirty[Dirty::Transform];
    if(!must(KRPTSceneItem::Must::NoSceneScale    ) && 
       !must(KRPTSceneItem::Must::NoSceneRotate   ) && 
       !must(KRPTSceneItem::Must::SceneScaleEvent ) &&
       !must(KRPTSceneItem::Must::SceneRotateEvent))return dirty;
    if(_data->cache.empty())
    {
        KRPTSceneItem *item = this;
        while(item)
        {
            _data->cache.emplace_back(item, item->_parent);
            item = item->_parent;
        }
    }
    bool scaleDirty = false;
    bool angleDirty = false;
    double sceneScale = _data->sceneScale;
    double sceneAngle = _data->sceneAngle;
    _data->sceneScale = 1;
    _data->sceneAngle = 0;
    auto it = _data->cache.rbegin();
    for(; it != _data->cache.rend(); ++it)
    {
        auto &cache = *it;
        if(cache.item == this)
        {
            if((scaleDirty || angleDirty) && (must(KRPTSceneItem::Must::NoSceneScale) || 
               must(KRPTSceneItem::Must::NoSceneRotate)))cache.dirty = true;
            break;
        }
        if(cache.genScale != cache.item->_data->genScale)
        {
            cache.genScale = cache.item->_data->genScale;
            scaleDirty = true;
            if(must(KRPTSceneItem::Must::NoSceneScale))_dirty += Dirty::SceneScale;
        }
        if(cache.genAngle != cache.item->_data->genAngle)
        {
            cache.genAngle = cache.item->_data->genAngle;
            angleDirty = true;
            if(must(KRPTSceneItem::Must::NoSceneRotate))_dirty += Dirty::SceneRotate;
        }
        if(scaleDirty)
        {
            _data->sceneScale *= (!cache.item->must(KRPTSceneItem::Must::NoSceneScale)) ? cache.item->_scale :
                1 / _data->sceneScale * cache.item->_scale;
            cache.scale = _data->sceneScale;
        }
        if(angleDirty)
        {
            _data->sceneAngle += !cache.item->must(KRPTSceneItem::Must::NoSceneRotate) ? cache.item->_angle :
                -_data->sceneAngle + cache.item->_angle;
            cache.angle = _data->sceneAngle;
        }
        _data->sceneScale = cache.scale;
        _data->sceneAngle = cache.angle;
    }
    if(dirty || scaleDirty || angleDirty)
    {
        if(must(KRPTSceneItem::Must::SceneScaleEvent) && scaleDirty)
            sceneScaleEvent(_data->sceneScale, sceneScale);
        if(must(KRPTSceneItem::Must::SceneRotateEvent) && angleDirty)
            sceneScaleEvent(_data->sceneAngle, sceneAngle);
        if(must(KRPTSceneItem::Must::NoSceneScale) || must(KRPTSceneItem::Must::NoSceneRotate))
        {
            _dirty += Dirty::Transform        ;
            _dirty += Dirty::TransformInv     ;
            _dirty += Dirty::SceneTransform   ;
            _dirty += Dirty::SceneTransformInv;
            _dirty += Dirty::BBox             ;
            _dirty += Dirty::BBoxMapToParent  ;
        }
    }
    return must(KRPTSceneItem::Must::NoSceneScale) || must(KRPTSceneItem::Must::NoSceneRotate) ?
        dirty || scaleDirty || angleDirty : dirty;
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

//************************************************************************************************************************
//*
//************************************************************************************************************************

void KRPTSceneItem::stopAnimImpl(uint32_t id) noexcept
{
    if(must(KRPTSceneItem::Must::Anim))_data->stopAnim(id);
}

void KRPTSceneItem::startAnimImpl(uint32_t id, const std::vector<double> &start, 
    const std::vector<double> &end, uint32_t time, QEasingCurve curve, int loopCount) noexcept
{
    _data->startAnim(id, start, end, time, curve, loopCount);
}

void KRPTSceneItem::startAnimImpl(uint32_t id, double start, 
    double end, uint32_t time, QEasingCurve curve, int loopCount) noexcept
{
    _data->startAnim(id, start, end, time, curve, loopCount);
}

void KRPTSceneItem::startAnimImpl(uint32_t id, const QPointF &start, 
    const QPointF &end, uint32_t time, QEasingCurve curve, int loopCount) noexcept
{
    _data->startAnim(id, start, end, time, curve, loopCount);
}

void KRPTSceneItem::startAnimImpl(uint32_t id, const QRectF &start, 
    const QRectF &end, uint32_t time, QEasingCurve curve, int loopCount) noexcept
{
    _data->startAnim(id, start, end, time, curve, loopCount);
}

void KRPTSceneItem::startAnimImpl(uint32_t id, const QSizeF &start, 
    const QSizeF &end, uint32_t time, QEasingCurve curve, int loopCount) noexcept
{
    _data->startAnim(id, start, end, time, curve, loopCount);
}

void KRPTSceneItem::startAnimImpl(uint32_t id, const QColor &start, 
    const QColor &end, uint32_t time, QEasingCurve curve, int loopCount) noexcept
{
    _data->startAnim(id, start, end, time, curve, loopCount);
}

