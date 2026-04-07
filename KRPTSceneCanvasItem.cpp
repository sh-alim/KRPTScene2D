//########################################################################################################################
//#
//########################################################################################################################

#include "KRPTSceneCanvasItem.h"
#include "KRPTScene.h"

//########################################################################################################################
//#
//########################################################################################################################

class KRPTSceneCanvasItemClient : public KRPTSceneItem
{
public:
    KRPTSceneCanvasItemClient(KRPTScene *scene, KRPTSceneItem *parent) noexcept;
    ~KRPTSceneCanvasItemClient()                                       noexcept;
protected:
    void paintBackground (QPainter &painter, uint32_t stage) noexcept override;
    void paintForeground (QPainter &painter, uint32_t stage) noexcept override;
protected:
    void childTransformEvent(KRPTSceneItem::Ptr item, SceneTransformEvent *e) noexcept override;

private:
    KRPTSceneCanvasItem *_owner;
};

//************************************************************************************************************************
//*
//************************************************************************************************************************

KRPTSceneCanvasItemClient::KRPTSceneCanvasItemClient(KRPTScene *scene, KRPTSceneItem *parent) noexcept
    : KRPTSceneItem(scene, parent), _owner(static_cast<KRPTSceneCanvasItem*>(parent))
{
#if 1
    upMust
    (
        KRPTSceneItem::Must::NoClipPainter,
        KRPTSceneItem::Must::NoClipChilds

//        KRPTSceneItem::Must::NoCheckChildVisibled,

//            KRPTSceneItem::Must::NoSceneScale,
//            KRPTSceneItem::Must::AccuracyClip,
//            KRPTSceneItem::Must::Anim,
//            KRPTSceneItem::Must::AccuracyClip,
//            KRPTSceneItem::Must::AccuracyCheckContains,
//            KRPTSceneItem::Must::MouseMoved,
//            KRPTSceneItem::Must::MousePressEvent,
//            KRPTSceneItem::Must::MouseMoveEvent,
//            KRPTSceneItem::Must::WhellEvent
    );
#endif

    setColor(0, QColor( 50,  50,  50));
    setColor(1, QColor(250, 250, 250));
}

KRPTSceneCanvasItemClient::~KRPTSceneCanvasItemClient() noexcept
{
}

//************************************************************************************************************************
//*
//************************************************************************************************************************

void KRPTSceneCanvasItemClient::paintBackground(QPainter &painter, uint32_t stage) noexcept
{
}

void KRPTSceneCanvasItemClient::paintForeground(QPainter &painter, uint32_t stage) noexcept
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

void KRPTSceneCanvasItemClient::childTransformEvent(KRPTSceneItem::Ptr item, SceneTransformEvent *e) noexcept 
{
    _owner->childTransformEvent(item, e);
};

//########################################################################################################################
//#
//########################################################################################################################

KRPTSceneCanvasItem::KRPTSceneCanvasItem(KRPTScene *scene, KRPTSceneItem *parent) noexcept
    : KRPTSceneItem(scene, parent), _client(nullptr)
{
#if 1
    upMust
    (
//        KRPTSceneItem::Must::NoClipPainter,
//        KRPTSceneItem::Must::NoClipChilds,
//        KRPTSceneItem::Must::NoCheckChildVisibled,

            KRPTSceneItem::Must::TransformEvent,
//            KRPTSceneItem::Must::AccuracyClip,
//            KRPTSceneItem::Must::Anim,
//            KRPTSceneItem::Must::AccuracyClip,
//            KRPTSceneItem::Must::AccuracyCheckContains,
//            KRPTSceneItem::Must::MouseMoved,
            KRPTSceneItem::Must::MousePressEvent,
            KRPTSceneItem::Must::MouseMoveEvent,
            KRPTSceneItem::Must::WhellEvent
    );
#endif
    resetMinMax();
    setColor(0, QColor( 50,  50,  50));
    setColor(1, QColor(250, 250, 250));
    _client = addChild<KRPTSceneCanvasItemClient>();
    _client->setGeometry(10, 10, 50, 50);
}

KRPTSceneCanvasItem::~KRPTSceneCanvasItem() noexcept
{
}

//************************************************************************************************************************
//*
//************************************************************************************************************************

void KRPTSceneCanvasItem::paintBackground(QPainter &painter, uint32_t stage) noexcept
{
    painter.fillRect(_rect, color(0));
}

void KRPTSceneCanvasItem::paintForeground(QPainter &painter, uint32_t stage) noexcept
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

void KRPTSceneCanvasItem::outlineImpl() noexcept
{
    _outline.addRect(_rect);
}

void KRPTSceneCanvasItem::transformImpl(SceneTransformEvent *e) noexcept
{
    KRPTSceneItem::transformImpl(e);
//    updateMinMax();
    updateClentRect();
}

void KRPTSceneCanvasItem::childTransformEvent(KRPTSceneItem::Ptr item, SceneTransformEvent *e)noexcept 
{
    updateMinMax(item);
}

void KRPTSceneCanvasItem::addChildImpl(KRPTSceneItem::Ptr item, KRPTSceneItem::Ptr parent) noexcept
{
    if(!_client)
    {
        KRPTSceneItem::addChildImpl(item, parent);
        return;
    }
    KRPTSceneItem::addChildImpl(item, _client);

    item->upMust
    (
        KRPTSceneItem::Must::TransformToParentEvent
    );

    item->downMust
    (
        KRPTSceneItem::Must::MouseMoved
    );

    updateMinMax(item);

}

void KRPTSceneCanvasItem::mousePressImpl(SceneMouseEvent *e) noexcept
{
    qDebug() << "=====>>>";
}

void KRPTSceneCanvasItem::mouseReleaseImpl(SceneMouseEvent *e) noexcept
{
}

void KRPTSceneCanvasItem::mouseMoveImpl(SceneMouseEvent *e) noexcept
{
}


//************************************************************************************************************************
//*
//************************************************************************************************************************

void KRPTSceneCanvasItem::resetMinMax() noexcept
{
    _minMax[0].value = std::numeric_limits<double>::max();
    _minMax[1].value = _minMax[0].value;
    _minMax[2].value = 0;
    _minMax[3].value = 0;
    for(auto &item : _minMax)item.item = nullptr;
}

void KRPTSceneCanvasItem::updateMinMax(KRPTSceneItem::Ptr item) noexcept
{
    bool updateMinMax = !item;
    if(!updateMinMax)
        for(auto &item : _minMax)
            if(item.item == nullptr){updateMinMax = true; break;}
    QRectF r;
    if(!updateMinMax)
    {
        r = item->bBoxMapToParent().translated(_client->pos());
        if((_minMax[0].item == item && r.x     () > _minMax[0].value) ||
           (_minMax[1].item == item && r.y     () > _minMax[1].value) ||
           (_minMax[2].item == item && r.right () < _minMax[2].value) ||
           (_minMax[3].item == item && r.bottom() < _minMax[3].value))updateMinMax = true;
    }
    bool updateClientRect = updateMinMax;
    if(!updateMinMax)
    {
        if(r.x() < _minMax[0].value)
        {
            _minMax[0].item  = item;
            _minMax[0].value = r.x();
            updateClientRect = true;
        }
        if(r.y() < _minMax[1].value)
        {
            _minMax[1].item  = item;
            _minMax[1].value = r.y();
            updateClientRect = true;
        }
        if(r.right() > _minMax[2].value)
        {
            _minMax[2].item  = item;
            _minMax[2].value = r.right();
            updateClientRect = true;
        }
        if(r.bottom() > _minMax[3].value)
        {
            _minMax[3].item  = item;
            _minMax[3].value = r.bottom();
            updateClientRect = true;
        }
    }else
    {
        resetMinMax();
        const auto &childs = _client->childItems();
        for(auto &child : childs)
        {
            QRectF r = child->bBoxMapToParent().translated(_client->pos());
            if(r.x     () < _minMax[0].value){_minMax[0].value = r.x     (); _minMax[0].item = child;}
            if(r.y     () < _minMax[1].value){_minMax[1].value = r.y     (); _minMax[1].item = child;}
            if(r.right () > _minMax[2].value){_minMax[2].value = r.right (); _minMax[2].item = child;}
            if(r.bottom() > _minMax[3].value){_minMax[3].value = r.bottom(); _minMax[3].item = child;}
        }
    }
    if(updateClientRect)updateClentRect();
}

void KRPTSceneCanvasItem::updateClentRect() noexcept
{
    double dx0 = _minMax[2].value - width (); dx0 = dx0 > 0 ? dx0 : 0;
    double dy0 = _minMax[3].value - height(); dy0 = dy0 > 0 ? dy0 : 0;
    double dx1 = _minMax[0].value < 0 ? -_minMax[0].value : 0;
    double dy1 = _minMax[1].value < 0 ? -_minMax[1].value : 0;
    _clientRect.setWidth (width () + dx0 + dx1);
    _clientRect.setHeight(height() + dy0 + dy1);
    _clientRect.moveLeft(-dx1);
    _clientRect.moveTop (-dy1);
    if(qFuzzyCompare(_clientRect, _client->geometry()))return;
    QPointF dp = _client->geometry().topLeft() - _clientRect.topLeft();
    lockUpdate(true);
    lockEvents(true);
    _client->setGeometry(_clientRect);
    if(!qFuzzyIsNull(dp))
    {
        const auto &childs = _client->childItems();
        for(auto &child : childs)
        {
            child->lockUpdate(true);
            child->lockEvents(true);
            child->translate(dp);
            child->lockEvents(false);
            child->lockUpdate(false);
        }
    }
    lockEvents(false);
    lockUpdate(false);
}
