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
        KRPTSceneItem::Must::MouseTracking,
        KRPTSceneItem::Must::MousePressEvent,
        KRPTSceneItem::Must::MouseMoveEvent,
        KRPTSceneItem::Must::MouseEnterEvent
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

void KRPTSceneBtnItem::mouseEnterEvent(bool enter) noexcept
{
    QColor color;

    if(enter)color = QColor(255, 0, 0);
    else color= QColor(255, 255, 255);
    update();

    startAnimImpl(KRPTSceneItem::AnimDst::User, _backgroundColor, color, 300, QEasingCurve::Linear);

}

//************************************************************************************************************************
//*
//************************************************************************************************************************

void KRPTSceneBtnItem::outlineImpl() noexcept
{
    _outline.addRect(_rect);
}

void KRPTSceneBtnItem::animImpl(uint32_t id, const std::vector<double> &value, 
    uint32_t time, bool completed, int loop) noexcept
{
    KRPTSceneItem::animImpl(id, value, time, completed, loop);
#if 1
    lockUpdate(true);
    switch(id)
    {
        case KRPTSceneItem::AnimDst::User:

            QColor color(value[0] * 255, value[1] * 255, value[2] * 255);

            setBackgroundColor(color);

//            qDebug() << value[0];
            update();
            break;
    }
    lockUpdate(false);
#endif
}

void KRPTSceneBtnItem::paintBackground(QPainter &painter, uint32_t stage) noexcept
{
    painter.fillRect(_rect, _backgroundColor);
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
