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
    upMust(KRPTSceneItem::Must::AccuracyClip, KRPTSceneItem::Must::AccuracyCheckContains);
    setColor(0, QColor( 50,  50,  50));
    setColor(1, QColor(250, 250, 250));
}

KRPTSceneEllipseItem::~KRPTSceneEllipseItem() noexcept
{
}

//************************************************************************************************************************
//*
//************************************************************************************************************************

void KRPTSceneEllipseItem::outlineImpl() noexcept
{
    _outline.addEllipse(_rect);
}

void KRPTSceneEllipseItem::paintBackground(QPainter &painter, uint32_t stage) noexcept
{
    painter.fillPath(outline(), color(0));
}

void KRPTSceneEllipseItem::paintForeground(QPainter &painter, uint32_t stage) noexcept
{
    QPen pen(color(1), 2);
    painter.setPen(pen);
//    painter.drawRect(_rect);
//    painter.drawPath(outline());
    painter.drawEllipse(_rect);
}
