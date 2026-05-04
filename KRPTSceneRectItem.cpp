//########################################################################################################################
//#
//########################################################################################################################

#include "KRPTSceneRectItem.h"
#include "KRPTScene.h"

//########################################################################################################################
//#
//########################################################################################################################

KRPTSceneRectItem::KRPTSceneRectItem(KRPTScene *scene, KRPTSceneItem *parent, const QRectF &geometry) noexcept
    : KRPTSceneItem(scene, parent, geometry,
        Must::NoClipPainter          | 
        Must::NoCheckChildVisibled   |
        Must::TransformAnim          |
//        Must::ColorAnim              |
//        Must::AccuracyClip           |
        Must::AccuracyCheckContains  |
        Must::Checked                |
        Must::MouseChecked           | 
        Must::MouseTracking          |
        Must::TransformEvent         |
        Must::StateChangeEvent       |
        Must::MousePressEvent        |
        Must::MouseMoveEvent         |
        Must::MouseEnterEvent        |
        Must::MousePressToParentEvent|
        Must::MousePressToSceneEvent |
        Must::CheckedToParentEvent   |
        Must::CheckedToSceneEvent)
//    ,_cornerRadius(6)
{
    setColor(0, QColor( 50,  50,  50));
    setColor(1, QColor(250, 250, 250));
}

KRPTSceneRectItem::~KRPTSceneRectItem() noexcept
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
    (void)stage;
    painter.fillRect(_rect, color(0));
}

void KRPTSceneRectItem::paintForeground(QPainter &painter, uint32_t stage) noexcept
{
    (void)stage;
    painter.setRenderHint(QPainter::Antialiasing);
    QPen pen(color(1), 1);
    pen.setCosmetic(true);
    painter.setPen(pen);
    painter.drawRect(_rect.adjusted(0.5, 0.5, -0.5, -0.5));
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
