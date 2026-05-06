//########################################################################################################################
//#
//########################################################################################################################

#include "KRPTSceneBtnGridItem.h"
#include "KRPTScene.h"

//########################################################################################################################
//#
//########################################################################################################################

KRPTSceneBtnGridItem::KRPTSceneBtnGridItem(KRPTScene *scene, KRPTSceneItem *parent, const QPointF &pos, 
    const QSizeF &childSize, Direction direction, uint32_t childsPerLine) noexcept
    : KRPTSceneAreaItem(scene, parent, QRectF(pos, QSizeF(10, 10)),
//      Must::NoPaint         |
//      Must::NoPaintBackground |
      Must::NoPaintForeground |
      Must::NoClipPainter   |
      Must::NoClipChilds    |
      Must::TransformEvent  |
      Must::MousePressEvent |
      Must::MouseMoveEvent  |
      Must::WhellEvent), _childSize(childSize), _direction(direction), _childsPerLine(childsPerLine) 
{
    _sizePolicy = SizePolicy::AutoSize;
    setColor(0, 30, 30, 30, 255);
    setColor(1, 80, 80, 80, 255);
}

//************************************************************************************************************************
//*
//************************************************************************************************************************

KRPTSceneBtnItem* KRPTSceneBtnGridItem::addBtn(const QByteArray &imageSrc, const QString &text) noexcept
{
    KRPTSceneBtnItem *item = addChild<KRPTSceneBtnItem>(QRectF(QPointF(), _childSize));
    item->lockUpdate(true);
    item->setImageSrc(imageSrc);
    item->upMust(Must::NoPaintForeground);
    item->setColor(0, color(0));
    item->delColor(0, State::Checked);
    item->delColor(1, State::Checked);
    item->lockUpdate(false);
    return item;
}

KRPTSceneSelectableBtnItem* KRPTSceneBtnGridItem::addSelectableBtn() noexcept
{
    KRPTSceneSelectableBtnItem *item = addChild<KRPTSceneSelectableBtnItem>(QRectF(QPointF(), _childSize));
    QColor c = color(0);
    item->lockUpdate(true);
    item->setColorAnimTime(200, QEasingCurve::InOutSine);
    item->setMargin(0, 0);
//    item->clearColors();

    item->setColor(0, color(0));


    c = color(1);
    item->setColor(1, c.red(), c.green(),  c.blue(), 0);
    item->setColor(1, c, State::Checked);
//    item->setColor(1, 255, 0, 0, 255, State::Checked);

    item->lockUpdate(false);
    return item;
}

//************************************************************************************************************************
//*
//************************************************************************************************************************

void KRPTSceneBtnGridItem::paintBackground(QPainter &painter, uint32_t stage) noexcept
{
    (void)stage;
    painter.setRenderHint(QPainter::Antialiasing);
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
}

void KRPTSceneBtnGridItem::paintForeground(QPainter &painter, uint32_t stage) noexcept
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

#if 0

void KRPTSceneBtnGridItem::outlineImpl() noexcept
{
    _outline.addRect(_rect);
}

void KRPTSceneBtnGridItem::transformImpl(SceneTransformEvent *e) noexcept
{
    KRPTSceneAreaItem::transformImpl(e);
}
#endif

void KRPTSceneBtnGridItem::childTransformEvent(KRPTSceneItem::Ptr item, SceneTransformEvent *e)noexcept 
{
    (void)e;
}

void KRPTSceneBtnGridItem::addChildImpl(KRPTSceneItem::Ptr item, KRPTSceneItem::Ptr parent) noexcept
{
    uint32_t sz = (uint32_t)_childItems.size() + 1;
    uint32_t n0 = _childsPerLine < 2 ? 0      : (uint32_t)(std::ceil(sz / (double)_childsPerLine) - 1);
    uint32_t n1 = _childsPerLine < 2 ? sz - 1 : sz - n0 * _childsPerLine - 1;
    QPointF p(_direction == Direction::Horisontal ? n1 * _childSize.height() : n0 * _childSize.width(),
              _direction == Direction::Horisontal ? n0 * _childSize.height() : n1 * _childSize.width());
    item->setGeometry(p, _childSize);
    KRPTSceneAreaItem::addChildImpl(item, parent);
}

//************************************************************************************************************************
//*
//************************************************************************************************************************

