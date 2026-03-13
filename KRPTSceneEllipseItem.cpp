//####################################################################################################
//#
//####################################################################################################

#include "KRPTSceneEllipseItem.h"
#include "KRPTScene.h"

//####################################################################################################
//#
//####################################################################################################

KRPTSceneEllipseItem::KRPTSceneEllipseItem(KRPTScene *scene, KRPTSceneItem *parent) noexcept
    : KRPTSceneItem(scene, parent)
{
    addMust(
        KRPTSceneItem::Must::AccuracyClip,
        KRPTSceneItem::Must::AccuracyCheckContains
    );
}

KRPTSceneEllipseItem::~KRPTSceneEllipseItem() noexcept
{
}

//****************************************************************************************************
//*
//****************************************************************************************************

void KRPTSceneEllipseItem::outlineImpl() noexcept
{
    _outline.addEllipse(_rect);
}

void KRPTSceneEllipseItem::paintBackground(QPainter &painter) noexcept
{
    painter.fillPath(outline(), _backgroundColor);
}

void KRPTSceneEllipseItem::paintForeground(QPainter &painter) noexcept
{
    QPen pen(_borderColor, 2);
    painter.setPen(pen);
//    painter.drawRect(_rect);
//    painter.drawPath(outline());
    painter.drawEllipse(_rect);
}
