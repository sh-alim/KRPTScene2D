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
};

//************************************************************************************************************************
//*
//************************************************************************************************************************

KRPTSceneCanvasItemClient::KRPTSceneCanvasItemClient(KRPTScene *scene, KRPTSceneItem *parent) noexcept
    : KRPTSceneItem(scene, parent)
{
#if 1
    upMust
    (
        KRPTSceneItem::Must::NoClipPainter,
            KRPTSceneItem::Must::NoClipChilds,
//        KRPTSceneItem::Must::NoCheckChildVisibled,

//            KRPTSceneItem::Must::NoSceneRotate,
//            KRPTSceneItem::Must::NoSceneScale,
//            KRPTSceneItem::Must::AccuracyClip,
//            KRPTSceneItem::Must::Anim,
//            KRPTSceneItem::Must::AccuracyClip,
//            KRPTSceneItem::Must::AccuracyCheckContains,
            KRPTSceneItem::Must::MouseMoved,
            KRPTSceneItem::Must::MousePressEvent,
            KRPTSceneItem::Must::MouseMoveEvent,
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

//########################################################################################################################
//#
//########################################################################################################################

KRPTSceneCanvasItem::KRPTSceneCanvasItem(KRPTScene *scene, KRPTSceneItem *parent) noexcept
    : KRPTSceneItem(scene, parent), _client(nullptr)
{
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

void KRPTSceneCanvasItem::outlineImpl() noexcept
{
    _outline.addRect(_rect);
}

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

void KRPTSceneCanvasItem::addChildImpl(KRPTSceneItem::Ptr item, KRPTSceneItem::Ptr parent) noexcept
{
    if(!_client)KRPTSceneItem::addChildImpl(item, parent);
    else KRPTSceneItem::addChildImpl(item, _client);
}
