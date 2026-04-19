//########################################################################################################################
//#
//########################################################################################################################

#include "KRPTSceneBtnItem.h"
#include "KRPTScene.h"

#include <QPainterPath>

//########################################################################################################################
//#
//########################################################################################################################

KRPTSceneBtnItem::KRPTSceneBtnItem(KRPTScene *scene, KRPTSceneItem *parent, const QRectF &geometry) noexcept
    : KRPTSceneItem(scene, parent, geometry,
        Must::NoClipPainter         | 
        Must::NoCheckChildVisibled  |
        Must::Anim                  |
        Must::AccuracyCheckContains |
        Must::Checked               |
        Must::MouseChecked          | 
        Must::MouseTracking         |
        Must::StateChangeEvent      |
        Must::MousePressEvent       |
        Must::MouseMoveEvent        |
        Must::MouseEnterEvent
    )
{
//            KRPTSceneItem::Must::AccuracyClip,
//            KRPTSceneItem::Must::NoSceneRotate,
//            KRPTSceneItem::Must::NoSceneScale,

#if 0
    setColor(0, QColor(  0,  50,   0, 255));
    setColor(0, QColor(  0, 100,   0, 255), State::MouseOver);
    setColor(0, QColor(  0, 100,   0, 255), State::MousePressed);
    setColor(0, QColor(  0,  50,   0, 255), State::MouseOver | State::MousePressed);
    setColor(0, QColor(  0, 180,   0, 255), State::Checked);
    setColor(0, QColor(  0, 150,   0, 255), State::Checked | State::MouseOver);
    setColor(0, QColor(  0,  50,   0, 255), State::Checked | State::MouseOver | State::MousePressed);
    setColor(1, QColor(  0, 255,   0, 255));
    setColor(1, QColor(  0, 255,   0, 255), State::MouseOver);
    setColor(1, QColor(  0, 255,   0, 255), State::MouseOver | State::MousePressed);
    setColor(1, QColor(255, 255,   0, 255), State::Checked);
    setColor(1, QColor(255, 255,   0, 255), State::Checked | State::MouseOver);
    setColor(1, QColor(255, 255,   0, 255), State::Checked | State::MouseOver | State::MousePressed);
#else

    setColor(0, QColor(  23, 33, 43, 255));
    setColor(0, QColor(  32, 43, 54, 255), State::MouseOver);
    setColor(0, QColor(  32, 43, 54, 255), State::MouseOver | State::MousePressed);

    setColor(0, QColor(  82, 136, 193, 255), State::Checked);
    setColor(0, QColor(  82, 136, 193, 255), State::Checked | State::MouseOver);
    setColor(0, QColor(  82, 136, 193, 255), State::Checked | State::MouseOver | State::MousePressed);

//    setColor(0, QColor(  32, 43, 54, 255));


    setColor(1, QColor(  30,  30,   30, 255));

#endif
}

KRPTSceneBtnItem::~KRPTSceneBtnItem() noexcept
{
}

//************************************************************************************************************************
//*
//************************************************************************************************************************

void KRPTSceneBtnItem::stateChangeEvent(const FState &newState, const FState &oldState) noexcept
{
    KRPTSceneItem::stateChangeEvent(newState, oldState);
};

void KRPTSceneBtnItem::mousePressEvent(SceneMouseEvent *e) noexcept 
{
};

void KRPTSceneBtnItem::mouseReleaseEvent(SceneMouseEvent *e) noexcept 
{
};

void KRPTSceneBtnItem::mouseEnterEvent(bool enter) noexcept
{
}

//************************************************************************************************************************
//*
//************************************************************************************************************************

void KRPTSceneBtnItem::outlineImpl() noexcept
{
//    _outline.addRect(_rect);
    _outline.addRoundedRect(_rect, _radius, _radius);
}

void KRPTSceneBtnItem::animImpl(uint32_t id, const std::vector<double> &value, 
    uint32_t time, bool completed, int loop) noexcept
{
    KRPTSceneItem::animImpl(id, value, time, completed, loop);
}

void KRPTSceneBtnItem::paintBackground(QPainter &painter, uint32_t stage) noexcept
{
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(Qt::NoPen); 
    painter.setBrush(color(0)); 
    painter.drawRoundedRect(_rect, _radius, _radius);
    painter.setBrush(Qt::NoBrush); 
//    QPen pen(QColor(255, 255, 255), 2.0);
//    painter.setPen(pen); 
//    painter.drawEllipse(_rect.adjusted(10, 10, -10, -10));
}

void KRPTSceneBtnItem::paintForeground(QPainter &painter, uint32_t stage) noexcept
{
    painter.setRenderHint(QPainter::Antialiasing);
    QPen pen(color(1), 1.5);
    pen.setCosmetic(true);
    painter.setPen(pen);
    painter.drawRoundedRect(_rect.adjusted(0.5, 0.5, -0.5, -0.5), _radius, _radius);
}
