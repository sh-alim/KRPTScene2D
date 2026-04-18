//########################################################################################################################
//#
//########################################################################################################################

#include "KRPTSceneAreaItem.h"
#include "KRPTScene.h"

//########################################################################################################################
//#
//########################################################################################################################

KRPTSceneAreaItem::KRPTSceneAreaItem(KRPTScene *scene, KRPTSceneItem *parent) noexcept
    : KRPTSceneItem(scene, parent, QRectF(0, 0, 100, 100), 
      KRPTSceneItem::Must::NoClipPainter   |
      KRPTSceneItem::Must::TransformEvent  |
      KRPTSceneItem::Must::MousePressEvent |
      KRPTSceneItem::Must::MouseMoveEvent  |
      KRPTSceneItem::Must::WhellEvent)
{
    resetMinMax();
    setColor(0, QColor( 50,  50,  50));
    setColor(1, QColor(250, 250, 250));


    _margin = QPointF(10, 10);
}

//************************************************************************************************************************
//*
//************************************************************************************************************************

QPointF KRPTSceneAreaItem::margin() const noexcept
{
    return _margin;
}

void KRPTSceneAreaItem::setMargin(const QPointF &margin) noexcept
{
    _margin = margin;
    resetMinMax();
}

//************************************************************************************************************************
//*
//************************************************************************************************************************

void KRPTSceneAreaItem::paintBackground(QPainter &painter, uint32_t stage) noexcept
{
    painter.fillRect(_rect, color(0));
}

void KRPTSceneAreaItem::paintForeground(QPainter &painter, uint32_t stage) noexcept
{
    painter.setRenderHint(QPainter::Antialiasing);
    QPen pen(color(1), 1);
    pen.setCosmetic(true);
    painter.setPen(pen);
    painter.drawRect(_rect.adjusted(0.5, 0.5, -0.5, -0.5));
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
    updateMinMax(item);
}

void KRPTSceneAreaItem::addChildImpl(KRPTSceneItem::Ptr item, KRPTSceneItem::Ptr parent) noexcept
{
    KRPTSceneItem::addChildImpl(item, parent);
    item->upMust(KRPTSceneItem::Must::TransformToParentEvent);
    item->downMust(KRPTSceneItem::Must::MouseMoved);
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
#if 0
    double w  = _minMax[2].value - _minMax[0].value;
    double h  = _minMax[3].value - _minMax[1].value;
    double dx = _minMax[0].value;
    double dy = _minMax[1].value;
    QRectF geometry = _geometry;
    geometry.setWidth (w);
    geometry.setHeight(h);
    geometry.translate(dx, dy);
    if(!qFuzzyCompare(geometry, _geometry))
    {
        lockUpdate(true);
        lockEvents(true);
        if(!qFuzzyIsNull(dx) || !qFuzzyIsNull(dy))
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
#else
    double w  = _minMax[2].value;
    double h  = _minMax[3].value;
    QRectF geometry = _geometry;
    geometry.setWidth (w);
    geometry.setHeight(h);
    if(!qFuzzyCompare(geometry, _geometry))
    {
        setGeometry(geometry);
    }
#endif
}
