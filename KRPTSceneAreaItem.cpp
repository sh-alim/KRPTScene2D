//########################################################################################################################
//#
//########################################################################################################################

#include "KRPTSceneAreaItem.h"
#include "KRPTScene.h"

//########################################################################################################################
//#
//########################################################################################################################

KRPTSceneAreaItem::KRPTSceneAreaItem(KRPTScene *scene, KRPTSceneItem *parent, const QRectF &geometry, FMust must) noexcept
    : KRPTSceneItem(scene, parent, geometry, (Must)must.flag() | 
//      Must::NoPaint         |
//      Must::NoPaintBackground |
//      Must::NoPaintForeground |
      Must::NoClipPainter   |
      Must::TransformEvent  |
      Must::MousePressEvent |
      Must::MouseMoveEvent  |
      Must::WhellEvent), _margin(5, 5), _sizePolicy(SizePolicy::AutoPosSize), _cornerRadius(6), _lockAutoUpdate(false)
{
    resetMinMax();
    setColor(0, QColor(50, 50, 50));
    setColor(1, QColor(80, 80, 80));
}

//************************************************************************************************************************
//*
//************************************************************************************************************************

QPointF KRPTSceneAreaItem::margin() const noexcept
{
    return _margin;
}

KRPTSceneAreaItem::SizePolicy KRPTSceneAreaItem::sizePolicy() const noexcept
{
    return _sizePolicy;
}

void KRPTSceneAreaItem::setMargin(const QPointF &margin) noexcept
{
    if(qFuzzyCompare(_margin, margin))return;
    _margin = margin;
    resetMinMax();
    updateMinMax();
}

void KRPTSceneAreaItem::setSizePolicy(SizePolicy policy) noexcept
{
    if(_sizePolicy == policy)return;
    _sizePolicy = policy;
    resetMinMax();
    updateMinMax();
}

double KRPTSceneAreaItem::cornerRadius() const noexcept 
{
    return _cornerRadius;
}

void KRPTSceneAreaItem::setCornerRadius(double radius) noexcept
{
    if(qFuzzyCompare(_cornerRadius, radius))return;
    _cornerRadius = radius;
    update();
}

void KRPTSceneAreaItem::lockAutoUpdate(bool update) noexcept
{
    if(_lockAutoUpdate == update)return;
    _lockAutoUpdate = update;
    if(_lockAutoUpdate)
    {
        lockUpdate(true);
        lockEvents(true);
    }else
    {
        lockUpdate(false);
        lockEvents(false);
        updateMinMax();
    }
}

//************************************************************************************************************************
//*
//************************************************************************************************************************

void KRPTSceneAreaItem::paintBackground(QPainter &painter, uint32_t stage) noexcept
{
    (void)stage;
    painter.setRenderHint(QPainter::Antialiasing);
#if 0
    QPen pen(QColor(0, 0, 0, 100), 3.0);
    painter.setPen(pen);
    painter.drawRoundedRect(_rect.adjusted(0.5, 0.5, -0.5, -0.5), _cornerRadius, _cornerRadius);
#endif
#if 1
    if(mustAny(Must::NoPaintForeground))
    {
        QPen pen(color(1), 1.5);
        pen.setCosmetic(true);
        painter.setPen(pen);
    }else painter.setPen(Qt::NoPen); 
    painter.setBrush(color(0));
    if(qFuzzyIsNull(_cornerRadius))painter.drawRect(_rect.adjusted(0.5, 0.5, -0.5, -0.5));
    else painter.drawRoundedRect(_rect.adjusted(2.0, 2.0, -2.0, -2.0), _cornerRadius, _cornerRadius);
    painter.setBrush(Qt::NoBrush);
#endif
}

void KRPTSceneAreaItem::paintForeground(QPainter &painter, uint32_t stage) noexcept
{
    (void)stage;
    painter.setRenderHint(QPainter::Antialiasing);
    QPen pen(color(1), 1.5);
    pen.setCosmetic(true);
    painter.setPen(pen);
    if(qFuzzyIsNull(_cornerRadius))painter.drawRect(_rect.adjusted(0.5, 0.5, -0.5, -0.5));
    else painter.drawRoundedRect(_rect.adjusted(0.5, 0.5, -0.5, -0.5), _cornerRadius, _cornerRadius);
}

//************************************************************************************************************************
//*
//************************************************************************************************************************

void KRPTSceneAreaItem::outlineImpl() noexcept
{
    _outline.addRect(_rect);
}

void KRPTSceneAreaItem::transformImpl(SceneTransformEvent *e) noexcept
{
    KRPTSceneItem::transformImpl(e);
}

void KRPTSceneAreaItem::childTransformEvent(KRPTSceneItem::Ptr item, SceneTransformEvent *e)noexcept 
{
    (void)e;
    updateMinMax(item);
}

void KRPTSceneAreaItem::addChildImpl(KRPTSceneItem::Ptr item, KRPTSceneItem::Ptr parent) noexcept
{
    KRPTSceneItem::addChildImpl(item, parent);
    item->lockUpdate(true);
    item->upMust(KRPTSceneItem::Must::TransformToParentEvent);
    item->downMust(KRPTSceneItem::Must::MouseMoved);
    if(_sizePolicy == SizePolicy::AutoSize)
        item->translate(_margin);
    item->lockUpdate(false);
    updateMinMax(item);
}

//************************************************************************************************************************
//*
//************************************************************************************************************************

void KRPTSceneAreaItem::resetMinMax() noexcept
{
    _minMax[0].value = std::numeric_limits<double>::max();
    _minMax[1].value = _minMax[0].value;
    _minMax[2].value = 0;
    _minMax[3].value = 0;
    for(auto &item : _minMax)item.item = nullptr;
}

void KRPTSceneAreaItem::updateMinMax(KRPTSceneItem::Ptr item) noexcept
{
    if(_lockAutoUpdate)return;
    bool updateMinMax = !item;
    if(!updateMinMax)
        for(auto &item : _minMax)
            if(item.item == nullptr){updateMinMax = true; break;}
    QRectF r;
    if(!updateMinMax)
    {
        r = item->bBoxMapToParent();
        if((_minMax[0].item == item && r.x     () - _margin.x() > _minMax[0].value) ||
           (_minMax[1].item == item && r.y     () - _margin.y() > _minMax[1].value) ||
           (_minMax[2].item == item && r.right () + _margin.x() < _minMax[2].value) ||
           (_minMax[3].item == item && r.bottom() + _margin.y() < _minMax[3].value))updateMinMax = true;
    }
    bool updateClientRect = updateMinMax;
    if(!updateMinMax)
    {
        if(r.x() - _margin.x() < _minMax[0].value)
        {
            _minMax[0].item  = item;
            _minMax[0].value = r.x() - _margin.x();
            updateClientRect = true;
        }
        if(r.y() - _margin.y() < _minMax[1].value)
        {
            _minMax[1].item  = item;
            _minMax[1].value = r.y() - _margin.y();
            updateClientRect = true;
        }
        if(r.right() + _margin.x() > _minMax[2].value)
        {
            _minMax[2].item  = item;
            _minMax[2].value = r.right() + _margin.x();
            updateClientRect = true;
        }
        if(r.bottom() + _margin.y() > _minMax[3].value)
        {
            _minMax[3].item  = item;
            _minMax[3].value = r.bottom() + _margin.y();
            updateClientRect = true;
        }
    }else
    {
        resetMinMax();
        const auto &childs = _childItems;
        for(auto &child : childs)
        {
            QRectF r = child->bBoxMapToParent();
            if(r.x     () - _margin.x() < _minMax[0].value){_minMax[0].value = r.x     () - _margin.x(); _minMax[0].item = child;}
            if(r.y     () - _margin.y() < _minMax[1].value){_minMax[1].value = r.y     () - _margin.y(); _minMax[1].item = child;}
            if(r.right () + _margin.x() > _minMax[2].value){_minMax[2].value = r.right () + _margin.x(); _minMax[2].item = child;}
            if(r.bottom() + _margin.y() > _minMax[3].value){_minMax[3].value = r.bottom() + _margin.y(); _minMax[3].item = child;}
        }
    }
    if(updateClientRect)updateClentRect();
}

void KRPTSceneAreaItem::updateClentRect() noexcept
{
    double w  = _sizePolicy == SizePolicy::AutoSize ? _minMax[2].value : _minMax[2].value - _minMax[0].value;
    double h  = _sizePolicy == SizePolicy::AutoSize ? _minMax[3].value : _minMax[3].value - _minMax[1].value;
    double dx = _sizePolicy == SizePolicy::AutoSize ? 0 : _minMax[0].value;
    double dy = _sizePolicy == SizePolicy::AutoSize ? 0 : _minMax[1].value;
    QRectF geometry = _geometry;
    geometry.setWidth (w);
    geometry.setHeight(h);
    if(_sizePolicy == SizePolicy::AutoPosSize)geometry.translate(dx, dy);
    if(!qFuzzyCompare(geometry, _geometry))
    {
        lockUpdate(true);
        lockEvents(true);
        if(_sizePolicy == SizePolicy::AutoPosSize && (!qFuzzyIsNull(dx) || !qFuzzyIsNull(dy)))
        {
            const auto &childs = _childItems;
            for(auto &child : childs)
            {
                child->lockUpdate(true);
                child->lockEvents(true);
                child->translate(-dx, -dy);
                child->lockEvents(false);
                child->lockUpdate(false);
            }
            _minMax[0].value -= dx;
            _minMax[1].value -= dy;
            _minMax[2].value -= dx;
            _minMax[3].value -= dy;
        }
        setGeometry(geometry);
        lockEvents(false);
        lockUpdate(false);
    }
}
