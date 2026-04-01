//########################################################################################################################
//#
//########################################################################################################################

#include "KRPTSceneBtnItem.h"
#include "KRPTScene.h"

//########################################################################################################################
//#
//########################################################################################################################

KRPTSceneBtnItem::KRPTSceneBtnItem(KRPTScene *scene, KRPTSceneItem *parent) noexcept
    : KRPTSceneItem(scene, parent)
{
    upMust(
        KRPTSceneItem::Must::NoClipChilds,
        KRPTSceneItem::Must::NoClipPainter,
//        KRPTSceneItem::Must::NoCheckChildVisibled,
        KRPTSceneItem::Must::Anim,

//            KRPTSceneItem::Must::AccuracyCheckContains,
//            KRPTSceneItem::Must::AccuracyClip,

//            KRPTSceneItem::Must::NoSceneRotate,
//            KRPTSceneItem::Must::NoSceneScale,
        KRPTSceneItem::Must::MouseMoveble,
        KRPTSceneItem::Must::MousePressEvent,
        KRPTSceneItem::Must::MouseMoveEvent
//            KRPTSceneItem::Must::WhellEvent
    );
}

KRPTSceneBtnItem::~KRPTSceneBtnItem() noexcept
{
}

//************************************************************************************************************************
//*
//************************************************************************************************************************

void KRPTSceneBtnItem::mousePressEvent(SceneMouseEvent *e) noexcept 
{
};

void KRPTSceneBtnItem::mouseReleaseEvent(SceneMouseEvent *e) noexcept 
{
};

//************************************************************************************************************************
//*
//************************************************************************************************************************

void KRPTSceneBtnItem::outlineImpl() noexcept
{
    _outline.addRect(_rect);
}

void KRPTSceneBtnItem::paintBackground(QPainter &painter, uint32_t stage) noexcept
{
//    painter.fillRect(_rect, _backgroundColor);
}

void KRPTSceneBtnItem::paintForeground(QPainter &painter, uint32_t stage) noexcept
{
    painter.setRenderHint(QPainter::Antialiasing);
    QPen pen(_borderColor, 1);
    pen.setCosmetic(true);
    painter.setPen(pen);
    painter.drawRect(_rect);
#if 0
    painter.save();
    if(_parent)
    {
        painter.setTransform(_parent->sceneTransform());
        painter.drawRect(bBoxMapToParent());
//        painter.drawRect(bBox());
//        painter.drawRect(_geometry);
    }
    painter.restore();
#endif
}
