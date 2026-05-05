//########################################################################################################################
//#
//########################################################################################################################

#include "KRPTSceneGroupItem.h"
#include "KRPTScene.h"

//########################################################################################################################
//#
//########################################################################################################################

KRPTSceneGroupItem::KRPTSceneGroupItem(KRPTScene *scene, KRPTSceneItem *parent, const QRectF &geometry) noexcept
    : KRPTSceneAreaItem(scene, parent, geometry,
//      Must::NoPaint         |
//      Must::NoPaintBackground |
      Must::NoPaintForeground |
      Must::NoClipPainter   |
      Must::NoClipChilds    |
      Must::TransformEvent  |
      Must::MousePressEvent |
      Must::MouseMoveEvent  |
      Must::WhellEvent) 
{
    _sizePolicy = SizePolicy::AutoSize;
    setColor(0, QColor(50, 50, 50));
    setColor(1, QColor(80, 80, 80));
}

//************************************************************************************************************************
//*
//************************************************************************************************************************

void KRPTSceneGroupItem::paintBackground(QPainter &painter, uint32_t stage) noexcept
{
    (void)stage;
    painter.setRenderHint(QPainter::Antialiasing);
#if 0
    QPen pen(QColor(0, 0, 0, 100), 3.0);
    painter.setPen(pen);
    painter.drawRoundedRect(_rect.adjusted(0.5, 0.5, -0.5, -0.5), _cornerRadius, _cornerRadius);
#endif
#if 1
    if(mustAny(Must::NoPaintForeground))
    {
        QPen pen(color(1), 1.5);
        pen.setCosmetic(true);
        painter.setPen(pen);
    }else painter.setPen(Qt::NoPen); 
    painter.setBrush(color(0));
    if(qFuzzyIsNull(_cornerRadius))painter.drawRect(_rect.adjusted(0.5, 0.5, -0.5, -0.5));
    else painter.drawRoundedRect(_rect.adjusted(2.0, 2.0, -2.0, -2.0), _cornerRadius, _cornerRadius);
    painter.setBrush(Qt::NoBrush);
#endif
}

void KRPTSceneGroupItem::paintForeground(QPainter &painter, uint32_t stage) noexcept
{
    (void)stage;
    painter.setRenderHint(QPainter::Antialiasing);
    QPen pen(color(1), 1.5);
    pen.setCosmetic(true);
    painter.setPen(pen);
    if(qFuzzyIsNull(_cornerRadius))painter.drawRect(_rect.adjusted(0.5, 0.5, -0.5, -0.5));
    else painter.drawRoundedRect(_rect.adjusted(0.5, 0.5, -0.5, -0.5), _cornerRadius, _cornerRadius);
}

//************************************************************************************************************************
//*
//************************************************************************************************************************

void KRPTSceneGroupItem::outlineImpl() noexcept
{
    _outline.addRect(_rect);
}

void KRPTSceneGroupItem::transformImpl(SceneTransformEvent *e) noexcept
{
    KRPTSceneItem::transformImpl(e);
}

void KRPTSceneGroupItem::childTransformEvent(KRPTSceneItem::Ptr item, SceneTransformEvent *e)noexcept 
{
    (void)e;
}

void KRPTSceneGroupItem::addChildImpl(KRPTSceneItem::Ptr item, KRPTSceneItem::Ptr parent) noexcept
{
    KRPTSceneItem::addChildImpl(item, parent);
    item->upMust(Must::NoPaintForeground);
    item->setColor(0, 0, 0, 0, 0);
}

//************************************************************************************************************************
//*
//************************************************************************************************************************

