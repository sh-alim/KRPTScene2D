//####################################################################################################
//#
//####################################################################################################

#include "KRPTSceneAnimItem.h"
#include "KRPTScene.h"
#include "KRPTSceneAnim.h"

//####################################################################################################
//#
//####################################################################################################

KRPTSceneAnimItem::KRPTSceneAnimItem(KRPTScene *scene, KRPTSceneItem *parent) noexcept
    : KRPTSceneItem(scene, parent)
{
    addMust
    (
//        KRPTSceneItem::Must::NoClipChilds,
//        KRPTSceneItem::Must::NoPaint,
//        KRPTSceneItem::Must::TransformEvent,
//        KRPTSceneItem::Must::MousePressEvent,
//        KRPTSceneItem::Must::MouseMoveEvent,
//        KRPTSceneItem::Must::MouseReleaseEvent,
//        KRPTSceneItem::Must::WhellEvent
    );

    KRPTSceneAnim::Event event = std::bind(&KRPTSceneAnimItem::animEvent, this, 
        std::placeholders::_1,
        std::placeholders::_2,
        std::placeholders::_3
    );

    _anim = new KRPTSceneAnim(this, 0, event);

    _anim->start();
}

KRPTSceneAnimItem::~KRPTSceneAnimItem() noexcept
{
}

//****************************************************************************************************
//*
//****************************************************************************************************


//****************************************************************************************************
//*
//****************************************************************************************************

void KRPTSceneAnimItem::animEvent(KRPTSceneAnim *anim, uint32_t id, double value) noexcept
{
    qDebug() << id << value;

}


void KRPTSceneAnimItem::transformEvent(SceneTransformEvent *e) noexcept
{
}

void KRPTSceneAnimItem::mousePressEvent(SceneMouseEvent *e) noexcept
{
}

void KRPTSceneAnimItem::mouseReleaseEvent(SceneMouseEvent *e) noexcept
{
}

void KRPTSceneAnimItem::mouseMoveEvent(SceneMouseEvent *e) noexcept
{
}

void KRPTSceneAnimItem::whellEvent(SceneMouseEvent *e) noexcept
{
}

void KRPTSceneAnimItem::paintBackground(QPainter &painter) noexcept
{
#if 1
    _backgroundColor = QColor(255, 255, 0, 155);
    painter.fillRect(_rect, _backgroundColor);
#endif
}

void KRPTSceneAnimItem::paintForeground(QPainter &painter) noexcept
{
#if 1
    _borderColor = QColor(255, 255, 0);
    QPen pen(_borderColor);
    pen.setWidth(1);
    pen.setCosmetic(true);
    painter.setPen(pen);
    painter.drawRect(_rect);
#endif
}

//****************************************************************************************************
//*
//****************************************************************************************************

