//########################################################################################################################
//#
//########################################################################################################################

#include "KRPTSceneScrolledAreaItem.h"
#include "KRPTScene.h"

//########################################################################################################################
//#
//########################################################################################################################

class KRPTSceneScrolledAreaCanvasItem : public KRPTSceneItem
{
friend class KRPTSceneScrolledAreaItem;
public:
    KRPTSceneScrolledAreaCanvasItem(KRPTScene *scene, KRPTSceneItem *parent, 
                                    const QRectF &geometry)                         noexcept;
    ~KRPTSceneScrolledAreaCanvasItem()                                              noexcept;
protected:
    void paintBackground (QPainter &painter, uint32_t stage)                        noexcept override;
    void paintForeground (QPainter &painter, uint32_t stage)                        noexcept override;
protected:
    void addChildImpl          (KRPTSceneItem::Ptr item, KRPTSceneItem::Ptr parent) noexcept override;
    void childTransformEvent   (KRPTSceneItem::Ptr item, SceneTransformEvent *e)    noexcept override;
    void childMousePressEvent  (KRPTSceneItem::Ptr item, SceneMouseEvent *e)        noexcept override;
    void childMouseReleaseEvent(KRPTSceneItem::Ptr item, SceneMouseEvent *e)        noexcept override;
    void childMouseMoveEvent   (KRPTSceneItem::Ptr item, SceneMouseEvent *e)        noexcept override;
    void childMouseOutEvent    (KRPTSceneItem::Ptr item, 
                                KRPTSceneItem::Ptr newItem, SceneMouseEvent *e)     noexcept override;
private:
    KRPTSceneScrolledAreaItem *_owner;
};

//************************************************************************************************************************
//*
//************************************************************************************************************************

KRPTSceneScrolledAreaCanvasItem::KRPTSceneScrolledAreaCanvasItem(KRPTScene *scene, KRPTSceneItem *parent, 
    const QRectF &geometry) noexcept
    : KRPTSceneItem(scene, parent, geometry,
        Must::NoPaint       |
//        Must::NoClipChilds  |
        Must::NoClipPainter |
//        Must::TransformAnim          |
        Must::MouseTracking |
        Must::TransformToParentEvent), 
    _owner(static_cast<KRPTSceneScrolledAreaItem*>(parent))
{
    setColor(0, QColor( 50,  50,  50));
    setColor(1, QColor(250, 250, 250));

#if 0
    setColor(0, QColor(  50,  50,   50, 255));
    setColor(0, QColor(  70, 70,   70, 255), State::MouseOver);
//    setColor(0, QColor(  70, 70,   70, 255), State::MousePressed);
//    setColor(0, QColor(  70, 70,   70, 255), State::MouseOver | State::MousePressed);
    setColor(0, QColor(  70, 70,   70, 255), State::ChildMouseOver);
//    setColor(0, QColor(  0, 180,   0, 255), State::Checked);
//    setColor(0, QColor(  0, 150,   0, 255), State::Checked | State::MouseOver);
//    setColor(0, QColor(  0,  50,   0, 255), State::Checked | State::MouseOver | State::MousePressed);
#endif
}

KRPTSceneScrolledAreaCanvasItem::~KRPTSceneScrolledAreaCanvasItem() noexcept
{
}

//************************************************************************************************************************
//*
//************************************************************************************************************************

void KRPTSceneScrolledAreaCanvasItem::paintBackground(QPainter &painter, uint32_t stage) noexcept
{
    (void)painter; (void)stage;
#if 0
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(Qt::NoPen); 
    painter.setBrush(color(0)); 
    painter.drawRect(_rect);
    painter.setBrush(Qt::NoBrush); 
#endif
}

void KRPTSceneScrolledAreaCanvasItem::paintForeground(QPainter &painter, uint32_t stage) noexcept
{
    (void)stage;
#if 1
    painter.setRenderHint(QPainter::Antialiasing);
    QPen pen(color(1), 1);
    pen.setCosmetic(true);
    painter.setPen(pen);
    painter.drawRect(_rect.adjusted(0.5, 0.5, -0.5, -0.5));
#endif
}

//************************************************************************************************************************
//*
//************************************************************************************************************************

void KRPTSceneScrolledAreaCanvasItem::addChildImpl(KRPTSceneItem::Ptr item, KRPTSceneItem::Ptr parent) noexcept
{
    KRPTSceneItem::addChildImpl(item, parent);
}

void KRPTSceneScrolledAreaCanvasItem::childTransformEvent(KRPTSceneItem::Ptr item, SceneTransformEvent *e) noexcept 
{
    _owner->areaChildTransformEvent(item, e);
};

void KRPTSceneScrolledAreaCanvasItem::childMousePressEvent(KRPTSceneItem::Ptr item, SceneMouseEvent *e) noexcept
{
    _owner->areaChildMousePressEvent(item, e);
}

void KRPTSceneScrolledAreaCanvasItem::childMouseReleaseEvent(KRPTSceneItem::Ptr item, SceneMouseEvent *e) noexcept
{
    _owner->areaChildMouseReleaseEvent(item, e);
}

void KRPTSceneScrolledAreaCanvasItem::childMouseMoveEvent(KRPTSceneItem::Ptr item, SceneMouseEvent *e) noexcept
{
    _owner->areaChildMouseMoveEvent(item, e);
}

void KRPTSceneScrolledAreaCanvasItem::childMouseOutEvent(KRPTSceneItem::Ptr item, 
    KRPTSceneItem::Ptr newItem, SceneMouseEvent *e) noexcept
{
    if(newItem == this || newItem == _owner || 
      (newItem && (newItem->parent() == this || newItem->parent() == _owner)))return;
    _owner->areaChildMouseOutEvent(item, newItem, e);
}

//########################################################################################################################
//#
//########################################################################################################################

KRPTSceneScrolledAreaItem::KRPTSceneScrolledAreaItem(KRPTScene *scene, KRPTSceneItem *parent, 
    const QRectF &geometry) noexcept
    : KRPTSceneItem(scene, parent, geometry,
//        Must::NoClipPainter   |
//        Must::NoClipChilds    |
        Must::AccuracyClip    |
        Must::TransformAnim   |
        Must::TransformEvent  |
        Must::MousePressEvent |
        Must::MouseMoveEvent  |
        Must::WhellEvent), 
    _area(nullptr), _areaSizePolicy(AreaSizePolicy::AutoSize), 
    _scrollPolicy(ScrollPolicy::Mouse | ScrollPolicy::Whell | ScrollPolicy::Vertical | ScrollPolicy::Horisontal),
    _mouseMoveDistance(0)
{
//    setColor(0, QColor( 50,  50,  50));
    setColor(0, QColor(  23, 33, 43, 255));

    setColor(1, QColor(  40, 40, 40, 255));


    _area = addChild<KRPTSceneScrolledAreaCanvasItem>(QRectF(0, 0, 500, 500));
    resetAreaMinMax();
}

KRPTSceneScrolledAreaItem::~KRPTSceneScrolledAreaItem() noexcept
{
}

//************************************************************************************************************************
//*
//************************************************************************************************************************

const KRPTSceneItem::List& KRPTSceneScrolledAreaItem::areaChildItems() const noexcept
{
    return _area->_childItems;
}

QPointF KRPTSceneScrolledAreaItem::areaPos() const noexcept
{
    return _areaRect.topLeft();
}

double  KRPTSceneScrolledAreaItem::areaX() const noexcept
{
    return _areaRect.x();
}

double  KRPTSceneScrolledAreaItem::areaY() const noexcept
{
    return _areaRect.y();
}

QSizeF  KRPTSceneScrolledAreaItem::areaSize() const noexcept
{
    return _areaRect.size();
}

double  KRPTSceneScrolledAreaItem::areaWidth() const noexcept
{
    return _areaRect.width();
}

double  KRPTSceneScrolledAreaItem::areaHeight() const noexcept
{
    return _areaRect.height();
}

KRPTSceneScrolledAreaItem::AreaSizePolicy KRPTSceneScrolledAreaItem::areaSizePolicy() const noexcept
{
    return _areaSizePolicy;
}

QPointF KRPTSceneScrolledAreaItem::margin() const noexcept
{
    return _margin;
}

void KRPTSceneScrolledAreaItem::setAreaSizePolicy(AreaSizePolicy policy) noexcept 
{
    if(policy == _areaSizePolicy)return;
    _areaSizePolicy = policy;
    if(_areaSizePolicy != AreaSizePolicy::None)updateAreaMinMax();
}

bool KRPTSceneScrolledAreaItem::setAreaGeometry(const QRectF &geometry, uint32_t time, QEasingCurve curve) noexcept
{
    if(qFuzzyCompare(_areaRect, geometry))return false;
    if(mustAnim(time))
    {
        startAnimImpl(AnimDst::User, _areaRect, geometry, time, curve);
        return true;
    }
    if(mustAny(Must::TransformAnim))stopAnimImpl(AnimDst::User);
    _areaRect = geometry;
    updateAreaRect();
    return true;
}

bool KRPTSceneScrolledAreaItem::setAreaGeometry(const QPointF &p, const QSizeF &size, 
    uint32_t time, QEasingCurve curve) noexcept
{
    return setAreaGeometry(QRectF(p, size), time, curve);
}

bool KRPTSceneScrolledAreaItem::setAreaGeometry (double x, double y, double w, double h, 
    uint32_t time, QEasingCurve curve) noexcept
{
    return setAreaGeometry(QRectF(x, y, w, h), time, curve);
}

void KRPTSceneScrolledAreaItem::setAreaPos(const QPointF &p, uint32_t time, QEasingCurve curve) noexcept
{
    setAreaGeometry(QRectF(p, _areaRect.size()), time, curve);
}

void KRPTSceneScrolledAreaItem::setAreaPos(double x, double y, uint32_t time, QEasingCurve curve) noexcept
{
    setAreaPos(QPointF(x, y), time, curve);
}

void KRPTSceneScrolledAreaItem::setAreaX(double x, uint32_t time, QEasingCurve curve) noexcept
{
    setAreaPos(x, _areaRect.y(), time, curve);
}

void KRPTSceneScrolledAreaItem::setAreaY(double y, uint32_t time, QEasingCurve curve) noexcept
{
    setAreaPos(_areaRect.x(), y, time, curve);
}

void KRPTSceneScrolledAreaItem::setAreaSize(const QSizeF &size, uint32_t time, QEasingCurve curve) noexcept
{
    if(qFuzzyCompare(_areaRect.size(), size))return;
    setAreaGeometry(_areaRect.topLeft(), size, time, curve);
}

void KRPTSceneScrolledAreaItem::setAreaSize(double w, double h, uint32_t time, QEasingCurve curve) noexcept
{
    setAreaSize(QSizeF(w, h), time, curve);
}

void KRPTSceneScrolledAreaItem::setAreaWidth(double w, uint32_t time, QEasingCurve curve) noexcept
{
    setAreaSize(w, _areaRect.height(), time, curve);
}

void KRPTSceneScrolledAreaItem::setAreaHeight(double h, uint32_t time, QEasingCurve curve) noexcept
{
    setAreaSize(_areaRect.width(), h, time, curve);
}

void KRPTSceneScrolledAreaItem::translateArea(const QPointF &p, uint32_t time, QEasingCurve curve) noexcept
{
    setAreaPos(areaPos() + p, time, curve);
}

void KRPTSceneScrolledAreaItem::translateArea(double x, double y, uint32_t time, QEasingCurve curve) noexcept
{
    setAreaPos(areaX() + x, areaY() + y, time, curve);
}

void KRPTSceneScrolledAreaItem::setMargin(const QPointF &margin) noexcept
{
    if(margin == _margin)return;
    _margin = margin;
    update();
}

//************************************************************************************************************************
//*
//************************************************************************************************************************

QPointF KRPTSceneScrolledAreaItem::mapToArea(const QPointF &point) noexcept
{
    return _area->mapFromParent(point);
}

QPointF KRPTSceneScrolledAreaItem::mapFromArea(const QPointF &point) noexcept
{
    return _area->mapToParent(point);
}

//************************************************************************************************************************
//*
//************************************************************************************************************************

void KRPTSceneScrolledAreaItem::paintBackground(QPainter &painter, uint32_t stage) noexcept
{
    (void)stage;
//    painter.fillRect(_rect, color(0));
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(Qt::NoPen); 
    painter.setBrush(color(0)); 
    if(qFuzzyIsNull(_radius))
        painter.drawRect(_rect.adjusted(0.5, 0.5, -0.5, -0.5));
    else
        painter.drawRoundedRect(_rect.adjusted(0.5, 0.5, -0.5, -0.5), _radius, _radius);
    painter.setBrush(Qt::NoBrush); 
}

void KRPTSceneScrolledAreaItem::paintForeground(QPainter &painter, uint32_t stage) noexcept
{
    (void)stage;
    painter.setRenderHint(QPainter::Antialiasing);
    QPen pen(color(1), 2);
    pen.setCosmetic(true);
    painter.setPen(pen);
    if(qFuzzyIsNull(_radius))painter.drawRect(_rect.adjusted(0.5, 0.5, -0.5, -0.5));
    else painter.drawRoundedRect(_rect.adjusted(0.5, 0.5, -0.5, -0.5), _radius, _radius);
//    painter.drawRect(_areaRect);
}

//************************************************************************************************************************
//*
//************************************************************************************************************************

void KRPTSceneScrolledAreaItem::outlineImpl() noexcept
{
    if(qFuzzyIsNull(_radius))_outline.addRect(_rect);
    else _outline.addRoundedRect(_rect, _radius, _radius);
}

void KRPTSceneScrolledAreaItem::transformImpl(SceneTransformEvent *e) noexcept
{
    KRPTSceneItem::transformImpl(e);
    if(e->resized())
    {
        _trans.dirtySize = true;
        updateAreaRect();
    }
}

void KRPTSceneScrolledAreaItem::childTransformEvent(KRPTSceneItem::Ptr item, SceneTransformEvent *e) noexcept 
{
    (void)item;
    if(e->scaled() || e->rotated())
    {
        _trans.dirtyArea = true;
        updateAreaRect();
    }
}

void KRPTSceneScrolledAreaItem::addChildImpl(KRPTSceneItem::Ptr item, KRPTSceneItem::Ptr parent) noexcept
{
    (void)item;
    if(!_area)
    {
        KRPTSceneItem::addChildImpl(item, parent);
        return;
    }
    _area->addChildImpl(item, _area);
    item->upMust  (Must::TransformToParentEvent, Must::MousePressToParentEvent, Must::MouseMoveToParentEvent);
    item->downMust(KRPTSceneItem::Must::MouseMoved);
    if(_areaSizePolicy != AreaSizePolicy::None)updateAreaMinMax(item);
}

void KRPTSceneScrolledAreaItem::mousePressImpl(SceneMouseEvent *e) noexcept
{
    _mousePos = e->pos();
    _area->stopAnimImpl(AnimDst::Geometry);
    _area->stopAnimImpl(AnimDst::Angle);
    _area->stopAnimImpl(AnimDst::Scale);
    stopAnimImpl(AnimDst::User);
    if(_scrollPolicy.any(ScrollPolicy::Mouse) && _scrollPolicy.any(ScrollPolicy::Horisontal, ScrollPolicy::Vertical))
    {
        _mouseVelocity.setX(0);
        _mouseVelocity.setY(0);
        _mouseMoveDistance = 0;
        _mouseVelocityTimer.start();
    }
}

void KRPTSceneScrolledAreaItem::mouseReleaseImpl(SceneMouseEvent *e) noexcept
{
    (void)e;
    if(_scrollPolicy.any(ScrollPolicy::Mouse) && _scrollPolicy.any(ScrollPolicy::Horisontal, ScrollPolicy::Vertical))
    {
        int64_t dt = _mouseVelocityTimer.restart(); 
        if(dt > 0)translateArea(_mouseVelocity * _mouseMoveDistance / dt * 5, 1000);
    }
}

void KRPTSceneScrolledAreaItem::mouseMoveImpl(SceneMouseEvent *e) noexcept
{
    (void)e;
    QPointF dp = e->pos() - _mousePos;
    if(e->btns() == SceneMouseEvent::Btn::Left)
    {
        if(_scrollPolicy.any(ScrollPolicy::Mouse) && _scrollPolicy.any(ScrollPolicy::Horisontal, ScrollPolicy::Vertical))
        {
            QPointF d(_scrollPolicy[ScrollPolicy::Horisontal] ? dp.x() : 0,
                      _scrollPolicy[ScrollPolicy::Vertical  ] ? dp.y() : 0);
            translateArea(d);
            _mouseMoveDistance += std::hypot(dp.x(), dp.y());
            _mouseVelocity = dp;
        }
    }
    _mousePos = e->pos();
}

void KRPTSceneScrolledAreaItem::whellImpl(SceneMouseEvent *e) noexcept
{
    QPointF _posGrid(110, 110);

    if(_scrollPolicy(ScrollPolicy::Whell) && _scrollPolicy.any(ScrollPolicy::Vertical | ScrollPolicy::Horisontal))
    {
        double dx0 = e->delta().x() == 0 ? 0 : e->delta().x() > 0 ? _posGrid.x() : -_posGrid.x();
        double dy0 = e->delta().y() == 0 ? 0 : e->delta().y() > 0 ? _posGrid.y() : -_posGrid.y();
        double dx1 = std::round((_areaRect.x() + dx0) / _posGrid.x()) * _posGrid.x();
        double dy1 = std::round((_areaRect.y() + dy0) / _posGrid.y()) * _posGrid.y();
        setAreaPos(dx1, dy1, 500);
//        translateArea(dx, dy, 500);
    }
}

void KRPTSceneScrolledAreaItem::animImpl(uint32_t id, const std::vector<double> &value, 
    uint32_t time, bool completed, int loop) noexcept
{
    lockUpdate(true);
    KRPTSceneItem::animImpl(id, value, time, completed, loop);
    switch(id)
    {
        case AnimDst::User :
        {
            _areaRect = QRectF(value[0], value[1], value[2], value[3]);
            updateAreaRect();
            break;
        }
    }
    lockUpdate(false);
}

//************************************************************************************************************************
//*
//************************************************************************************************************************

void KRPTSceneScrolledAreaItem::areaChildTransformEvent(KRPTSceneItem::Ptr item, SceneTransformEvent *e) noexcept 
{
    (void)e;
    if(_areaSizePolicy != AreaSizePolicy::None)updateAreaMinMax(item);
}

void KRPTSceneScrolledAreaItem::areaChildMousePressEvent(KRPTSceneItem::Ptr item, SceneMouseEvent *e) noexcept
{
    if(!_scrollPolicy[ScrollPolicy::MouseChildOver])return;
    QPointF scenePos = item->mapToScene(e->pos());
    QPointF pos = mapFromScene(scenePos);
    mousePressImpl(SceneMouseEvent::get(pos, e->btns(), scenePos, e->keyModifers(), e->delta()).get());
}

void KRPTSceneScrolledAreaItem::areaChildMouseReleaseEvent(KRPTSceneItem::Ptr item, SceneMouseEvent *e) noexcept
{
    if(!_scrollPolicy[ScrollPolicy::MouseChildOver])return;
    QPointF scenePos = item->mapToScene(e->pos());
    QPointF pos = mapFromScene(scenePos);
    mouseReleaseImpl(SceneMouseEvent::get(pos, e->btns(), scenePos, e->keyModifers(), e->delta()).get());
}

void KRPTSceneScrolledAreaItem::areaChildMouseMoveEvent(KRPTSceneItem::Ptr item, SceneMouseEvent *e) noexcept
{
    if(!_scrollPolicy[ScrollPolicy::MouseChildOver])return;
    QPointF scenePos = item->mapToScene(e->pos());
    QPointF pos = mapFromScene(scenePos);
    mouseMoveImpl(SceneMouseEvent::get(pos, e->btns(), scenePos, e->keyModifers(), e->delta()).get());
}

void KRPTSceneScrolledAreaItem::areaChildMouseOutEvent(KRPTSceneItem::Ptr item, 
    KRPTSceneItem::Ptr newItem, SceneMouseEvent *e) noexcept
{
    QPointF scenePos = item->mapToScene(e->pos());
    QPointF pos = mapFromScene(scenePos);
    mouseOutImpl(newItem, SceneMouseEvent::get(pos, e->btns(), scenePos, e->keyModifers(), e->delta()).get());
}

//************************************************************************************************************************
//*
//************************************************************************************************************************

void KRPTSceneScrolledAreaItem::updateAreaRect() noexcept
{
    QPointF dp;
    QRectF  rect;
    if(_trans.dirtyArea)
    {
        double angleRad = _area->_angle * 0.017453292519943295769;
        _trans.sin = std::sin(angleRad);
        _trans.cos = std::cos(angleRad);
    }
    if(_trans.dirtySize || _trans.dirtyArea)
    {
        double tsin = _trans.sin < 0 ? -_trans.sin : _trans.sin;
        double tcos = _trans.cos < 0 ? -_trans.cos : _trans.cos;
        _trans.rect = QRectF(_rect.center().x(), _rect.center().y(), 
                             _rect.height() * tsin + _rect.width() * tcos, 
                             _rect.height() * tcos + _rect.width() * tsin);
        _trans.rect.moveCenter(_rect.center());
    }
    _trans.dirtySize = false;
    _trans.dirtyArea = false;
    rect = QRectF(_areaRect.x(), _areaRect.y(), 
        std::max(_trans.rect.width (), _areaRect.width ()),
        std::max(_trans.rect.height(), _areaRect.height()));
    QPointF dc = rect.center() - _rect.center();
    QRectF r = rect;
    r.moveCenter(QPointF(dc.x() *  _trans.cos - dc.y() * -_trans.sin + _rect.center().x(), 
                         dc.x() * -_trans.sin + dc.y() *  _trans.cos + _rect.center().y()));
    double left   = _trans.rect.left  () < r.left  () ? _trans.rect.left  () - r.left  () : 0;
    double top    = _trans.rect.top   () < r.top   () ? _trans.rect.top   () - r.top   () : 0;
    double right  = _trans.rect.right () > r.right () ? _trans.rect.right () - r.right () : 0;
    double bottom = _trans.rect.bottom() > r.bottom() ? _trans.rect.bottom() - r.bottom() : 0;
    QPointF dp0(left + right, top + bottom);
    dp.setX(0); dp.setY(0);
    if(!qFuzzyIsNull(dp0))
    {
        dp.setX(dp0.x() * _trans.cos - dp0.y() * _trans.sin);
        dp.setY(dp0.x() * _trans.sin + dp0.y() * _trans.cos);
        rect.translate(dp);
    }
    if(!qFuzzyCompare(_area->_geometry, rect))
    {
        _area->lockEvents(true);
        _area->setGeometry(rect);
        _area->lockEvents(false);
    }
    _areaRect.translate(dp);
}

//************************************************************************************************************************
//*
//************************************************************************************************************************

void KRPTSceneScrolledAreaItem::resetAreaMinMax() noexcept
{
    _areaMinMax[0].value = std::numeric_limits<double>::max();
    _areaMinMax[1].value = _areaMinMax[0].value;
    _areaMinMax[2].value = 0;
    _areaMinMax[3].value = 0;
    for(auto &item : _areaMinMax)item.item = nullptr;
}

void KRPTSceneScrolledAreaItem::updateAreaMinMax(KRPTSceneItem::Ptr item) noexcept
{
    bool updateMinMax = !item;
    if(!updateMinMax)
        for(auto &item : _areaMinMax)
            if(item.item == nullptr){updateMinMax = true; break;}
    QRectF r;
    if(!updateMinMax)
    {
        r = item->bBoxMapToParent();
        if((_areaMinMax[0].item == item && r.x     () - _margin.x() > _areaMinMax[0].value) ||
           (_areaMinMax[1].item == item && r.y     () - _margin.y() > _areaMinMax[1].value) ||
           (_areaMinMax[2].item == item && r.right () + _margin.x() < _areaMinMax[2].value) ||
           (_areaMinMax[3].item == item && r.bottom() + _margin.y() < _areaMinMax[3].value))updateMinMax = true;
    }
    bool updateClientRect = updateMinMax;
    if(!updateMinMax)
    {
        if(r.x() - _margin.x() < _areaMinMax[0].value)
        {
            _areaMinMax[0].item  = item;
            _areaMinMax[0].value = r.x() - _margin.x();
            updateClientRect = true;
        }
        if(r.y() - _margin.y() < _areaMinMax[1].value)
        {
            _areaMinMax[1].item  = item;
            _areaMinMax[1].value = r.y() - _margin.y();
            updateClientRect = true;
        }
        if(r.right() + _margin.x() > _areaMinMax[2].value)
        {
            _areaMinMax[2].item  = item;
            _areaMinMax[2].value = r.right() + _margin.x();
            updateClientRect = true;
        }
        if(r.bottom() + _margin.y() > _areaMinMax[3].value)
        {
            _areaMinMax[3].item  = item;
            _areaMinMax[3].value = r.bottom() + _margin.y();
            updateClientRect = true;
        }
    }else
    {
        resetAreaMinMax();
        const auto &childs = _area->_childItems;
        for(auto &child : childs)
        {
            QRectF r = child->bBoxMapToParent();
            if(r.x     () - _margin.x() < _areaMinMax[0].value){_areaMinMax[0].value = r.x     () - _margin.x(); _areaMinMax[0].item = child;}
            if(r.y     () - _margin.y() < _areaMinMax[1].value){_areaMinMax[1].value = r.y     () - _margin.y(); _areaMinMax[1].item = child;}
            if(r.right () + _margin.x() > _areaMinMax[2].value){_areaMinMax[2].value = r.right () + _margin.x(); _areaMinMax[2].item = child;}
            if(r.bottom() + _margin.y() > _areaMinMax[3].value){_areaMinMax[3].value = r.bottom() + _margin.y(); _areaMinMax[3].item = child;}
        }
    }
    if(updateClientRect)
    {
        double w  = _areaSizePolicy == AreaSizePolicy::AutoSize ? _areaMinMax[2].value : _areaMinMax[2].value - _areaMinMax[0].value;
        double h  = _areaSizePolicy == AreaSizePolicy::AutoSize ? _areaMinMax[3].value : _areaMinMax[3].value - _areaMinMax[1].value;
        double dx = _areaSizePolicy == AreaSizePolicy::AutoSize ? 0 : _areaMinMax[0].value;
        double dy = _areaSizePolicy == AreaSizePolicy::AutoSize ? 0 : _areaMinMax[1].value;
        QRectF geometry = _area->_geometry;
        geometry.setWidth (w);
        geometry.setHeight(h);
        if(_areaSizePolicy == AreaSizePolicy::AutoPosSize)geometry.translate(dx, dy);
        if(!qFuzzyCompare(geometry, _area->_geometry))
        {
            lockUpdate(true);
            lockEvents(true);
            if(_areaSizePolicy == AreaSizePolicy::AutoPosSize && (!qFuzzyIsNull(dx) || !qFuzzyIsNull(dy)))
            {
                const auto &childs = _area->_childItems;
                for(auto &child : childs)
                {
                    child->lockUpdate(true);
                    child->lockEvents(true);
                    child->translate(-dx, -dy);
                    child->lockEvents(false);
                    child->lockUpdate(false);
                }
                _areaMinMax[0].value -= dx;
                _areaMinMax[1].value -= dy;
                _areaMinMax[2].value -= dx;
                _areaMinMax[3].value -= dy;
            }
            _areaRect = geometry;
            updateAreaRect();
            lockEvents(false);
            lockUpdate(false);
        }
    }
}

