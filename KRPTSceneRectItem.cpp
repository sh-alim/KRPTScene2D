//########################################################################################################################
//#
//########################################################################################################################

#include "KRPTSceneRectItem.h"
#include "KRPTScene.h"

//########################################################################################################################
//#
//########################################################################################################################

KRPTSceneRectItem::KRPTSceneRectItem(KRPTScene *scene, KRPTSceneItem *parent) noexcept
    : KRPTSceneItem(scene, parent)
{
}

KRPTSceneRectItem::~KRPTSceneRectItem() noexcept
{
}

void KRPTSceneRectItem::setColor() noexcept
{
}

//************************************************************************************************************************
//*
//************************************************************************************************************************

void KRPTSceneRectItem::outlineImpl() noexcept
{
    _outline.addRect(_rect);
}

void KRPTSceneRectItem::paintBackground(QPainter &painter, uint32_t stage) noexcept
{
//    painter.fillRect(_rect, _backgroundColor);
}

void KRPTSceneRectItem::paintForeground(QPainter &painter, uint32_t stage) noexcept
{
    painter.setRenderHint(QPainter::Antialiasing);
    QPen pen(_borderColor, 1);
    pen.setCosmetic(true);
    painter.setPen(pen);
    painter.drawRect(_rect);

#if 1
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

//    QPolygonF p = mapToParent(bBox()); 
//    painter.drawPolygon(bBox());
//    painter.drawPath(outline());
}
