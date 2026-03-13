//####################################################################################################
//#
//####################################################################################################

#include "KRPTSceneRectItem.h"
#include "KRPTScene.h"

//####################################################################################################
//#
//####################################################################################################

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

//****************************************************************************************************
//*
//****************************************************************************************************

void KRPTSceneRectItem::outlineImpl() noexcept
{
    _outline.addRect(_rect);
}

void KRPTSceneRectItem::paintBackground(QPainter &painter) noexcept
{
    painter.fillRect(_rect, _backgroundColor);
}

void KRPTSceneRectItem::paintForeground(QPainter &painter) noexcept
{
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
    }
    painter.restore();
#endif

//    QPolygonF p = mapToParent(bBox()); 
//    painter.drawPolygon(bBox());
//    painter.drawPath(outline());
}
