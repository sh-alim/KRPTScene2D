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
        KRPTSceneItem::Must::NoClipChilds,
//        KRPTSceneItem::Must::NoCheckChildVisibled,

//            KRPTSceneItem::Must::NoSceneScale,
//            KRPTSceneItem::Must::AccuracyClip,
//            KRPTSceneItem::Must::Anim,
//            KRPTSceneItem::Must::AccuracyClip,
//            KRPTSceneItem::Must::AccuracyCheckContains,
//            KRPTSceneItem::Must::MouseMoved,
//            KRPTSceneItem::Must::MousePressEvent,
//            KRPTSceneItem::Must::MouseMoveEvent,
            KRPTSceneItem::Must::WhellEvent
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

            KRPTSceneItem::Must::TransformEvent
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
    for(auto item : _minMaxItems)item = nullptr;


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
//    updateClent();
    KRPTSceneItem::transformImpl(e);

    updateMinMaxItems();
}

void KRPTSceneCanvasItem::childTransformEvent(KRPTSceneItem::Ptr item, SceneTransformEvent *e)noexcept 
{
    updateMinClientRect(item);
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

    updateMinMaxItems();

}

//************************************************************************************************************************
//*
//************************************************************************************************************************

void KRPTSceneCanvasItem::updateMinMaxItems() noexcept
{
    const auto &childs = _client->childItems();
    if(childs.empty())
    {
        for(auto item : _minMaxItems)item = nullptr;
        return;
    }
    double minMax[4] = {std::numeric_limits<double>::max(), 
        std::numeric_limits<double>::max(), 0, 0};
    for(auto &child : childs)
    {
        QRectF r = child->bBoxMapToParent().translated(_client->pos());
        if(r.x     () < minMax[0]){minMax[0] = r.x     (); _minMaxItems[0] = child;}
        if(r.y     () < minMax[1]){minMax[1] = r.y     (); _minMaxItems[1] = child;}
        if(r.right () > minMax[2]){minMax[2] = r.right (); _minMaxItems[2] = child;}
        if(r.bottom() > minMax[3]){minMax[3] = r.bottom(); _minMaxItems[3] = child;}
    }
    double dx0 = minMax[2] - width (); dx0 = dx0 > 0 ? dx0 : 0;
    double dy0 = minMax[3] - height(); dy0 = dy0 > 0 ? dy0 : 0;
    double dx1 = minMax[0] < 0 ? -minMax[0] : 0;
    double dy1 = minMax[1] < 0 ? -minMax[1] : 0;
    QPointF dp = _clientRect.topLeft();
    _clientRect.setWidth (width () + dx0 + dx1);
    _clientRect.setHeight(height() + dy0 + dy1);
    _clientRect.moveLeft(-dx1);
    _clientRect.moveTop (-dy1);
    dp -= _clientRect.topLeft();
    lockEvents(true);
    lockUpdate(true);
    _client->setGeometry(_clientRect);
    if(dp.x() != 0 || dp.y() != 0)
    {
        for(auto &child : childs)
        {
            child->lockEvents(true);
            child->lockUpdate(true);
            child->translate(dp);
            child->lockEvents(false);
            child->lockUpdate(false);
        }
    }
    lockEvents(false);
    lockUpdate(false);
}

void KRPTSceneCanvasItem::updateMinClientRect(KRPTSceneItem::Ptr item) noexcept
{
    QRectF r0 = item->bBoxMapToParent().translated(_client->pos());

    double minMax[4] = {std::numeric_limits<double>::max(), 
        std::numeric_limits<double>::max(), 0, 0};

//    if(_minMaxItems[0] && item != _minMaxItems[0])
    if(_minMaxItems[0])
    {
        QRectF r1 = _minMaxItems[0]->bBoxMapToParent().translated(_client->pos());
        if(r1.x() < r0.x())
        {
            minMax[0] = r1.x(); 
            _minMaxItems[0] = item;
        }
    }

    if(_minMaxItems[1])
    {
        QRectF r1 = _minMaxItems[1]->bBoxMapToParent().translated(_client->pos());
        if(r1.y() < r0.y())
        {
            minMax[1] = r1.y(); 
            _minMaxItems[1] = item;
        }
    }


    double dx0 = minMax[2] - width (); dx0 = dx0 > 0 ? dx0 : 0;
    double dy0 = minMax[3] - height(); dy0 = dy0 > 0 ? dy0 : 0;
    double dx1 = minMax[0] < 0 ? -minMax[0] : 0;
    double dy1 = minMax[1] < 0 ? -minMax[1] : 0;
    QPointF dp = _clientRect.topLeft();
    _clientRect.setWidth (width () + dx0 + dx1);
    _clientRect.setHeight(height() + dy0 + dy1);
    _clientRect.moveLeft(-dx1);
    _clientRect.moveTop (-dy1);
    dp -= _clientRect.topLeft();
    lockEvents(true);
    lockUpdate(true);
    _client->setGeometry(_clientRect);
#if 0
    if(dp.x() != 0 || dp.y() != 0)
    {
        for(auto &child : childs)
        {
            child->lockEvents(true);
            child->lockUpdate(true);
            child->translate(dp);
            child->lockEvents(false);
            child->lockUpdate(false);
        }
    }
#endif
    lockEvents(false);
    lockUpdate(false);


//    updateMinMaxItems();

    return;



#if 0
    if(_minMaxItems[2] && _minMaxItems[2] == item)
    {
//        updateMinMaxItems();
        QRectF bbox = _minMaxItems[2]->bBoxMapToParent();

        if(bbox.right() < _minClientRect.right())
        {
        qDebug() << "----";

            _minClientRect.setRight(bbox.right());
        }
    }

    if(bbox.right() > _minClientRect.right())
    {
        _minMaxItems[2] = item;
        _minClientRect.setRight(bbox.right());
    }else
    if(_minMaxItems[2] && _minMaxItems[2] == item)
    {
        updateMinMaxItems();
        QRectF bbox = _minMaxItems[2]->bBoxMapToParent();
        _minClientRect.setRight(bbox.right());
    }

    if(bbox.bottom() > _minClientRect.bottom())
    {
        _minMaxItems[3] = item;
        _minClientRect.setBottom(bbox.bottom());
    }

    _client->setGeometry(_minClientRect);
#endif

}

void KRPTSceneCanvasItem::updateClent() noexcept
{
//    _minClientRect
#if 0
    QRectF bbox = _client->bBox();
    QRectF geometry = _client->geometry();

    if(bbox.width() < width())
    {
        geometry.setWidth(width());
    }
    if(bbox.height() < height())
    {
        geometry.setHeight(height());
    }

    _client->setGeometry(geometry);
#endif
}
