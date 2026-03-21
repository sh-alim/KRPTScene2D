//########################################################################################################################
//#
//########################################################################################################################

#include "KRPTSceneEllipseItem.h"
#include "KRPTScene.h"

//########################################################################################################################
//#
//########################################################################################################################

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

//************************************************************************************************************************
//*
//************************************************************************************************************************

void KRPTSceneEllipseItem::outlineImpl() noexcept
{
    _outline.addEllipse(_clientRect);
}

void KRPTSceneEllipseItem::paintBackground(QPainter &painter, uint32_t stage) noexcept
{
    painter.fillPath(outline(), _backgroundColor);
}

void KRPTSceneEllipseItem::paintForeground(QPainter &painter, uint32_t stage) noexcept
{
    QPen pen(_borderColor, 2);
    painter.setPen(pen);
//    painter.drawRect(_clientRect);
//    painter.drawPath(outline());
    painter.drawEllipse(_clientRect);
}
