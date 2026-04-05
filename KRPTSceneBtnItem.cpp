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
        KRPTSceneItem::Must::StateChangeEvent,

        KRPTSceneItem::Must::MousePressEvent,
        KRPTSceneItem::Must::MouseMoveEvent,
        KRPTSceneItem::Must::MouseEnterEvent
    );


#if 0
    addStateColor(State::No       , QColor(0, 0, 0));
    addStateColor(State::MouseOver, QColor(255, 255, 255));
    addStateColor(State::MousePressed, QColor(255, 0, 0));
    addStateColor(State::MouseOver | State::MousePressed, QColor(255, 0, 255));
//    addStateColor(State::MouseOver | State::Checked, QColor(0, 255, 0));

//    auto color0 = stateColor(State::MouseOver | State::Checked);
//    auto color1 = stateColor(State::MouseOver);
//    qDebug() << color0 << color1;
#endif
    setColor(0, QColor(255,   0,   0));
    setColor(0, QColor(255,   255,   0), State::MouseOver);
//    setColor(0, QColor(0,   255,   255), State::MouseOver | State::MousePressed);

    setColor(1, QColor(  0, 255,   0));
//    setColor(1, QColor(255,  0, 255), State::MouseOver);
//    setColor(2, QColor(  0,   0, 255));
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

//    startAnimImpl(KRPTSceneItem::AnimDst::User, _backgroundColor, stateColor(_state), 1200, QEasingCurve::Linear);
//    startAnimImpl(KRPTSceneItem::AnimDst::User, _colorKof, 1, 1200, QEasingCurve::Linear);
//    startAnimImpl(KRPTSceneItem::AnimDst::User, 0, 1, 1000, QEasingCurve::Linear);
    update();
};

void KRPTSceneBtnItem::mousePressEvent(SceneMouseEvent *e) noexcept 
{
};

void KRPTSceneBtnItem::mouseReleaseEvent(SceneMouseEvent *e) noexcept 
{
};

void KRPTSceneBtnItem::mouseEnterEvent(bool enter) noexcept
{
//    QColor color;
//    if(enter)color = QColor(255, 0, 0);
//    else color= QColor(255, 255, 255);
//    update();
//    startAnimImpl(KRPTSceneItem::AnimDst::User, _backgroundColor, color, 300, QEasingCurve::Linear);
//    update();
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
#if 0
    lockUpdate(true);
    switch(id)
    {
        case KRPTSceneItem::AnimDst::User:
//            QColor color(value[0] * 255, value[1] * 255, value[2] * 255);
//            setBackgroundColor(color);

//            int r = c0.red()   + (c1.red()   - c0.red())   * t;

            if(time == 0)
                setStartColor(color());

            _colorKof = value[0];

            QColor c0(startColor());
            QColor c1(stateColor(_state));

            c1.setRedF  (c0.redF  () + (c1.redF  () - c0.redF  ()) * _colorKof);
            c1.setGreenF(c0.greenF() + (c1.greenF() - c0.greenF()) * _colorKof);
            c1.setBlueF (c0.blueF () + (c1.blueF () - c0.blueF ()) * _colorKof);

            setColor(c1);
//            setBackgroundColor(_c1);

            update();
            break;
    }
    lockUpdate(false);
#else
#if 0
    lockUpdate(true);
    switch(id)
    {
        case KRPTSceneItem::AnimDst::User:
            updateColor(value[0], time == 0);
            update();
            break;
    }
    lockUpdate(false);
#endif
#endif
}

void KRPTSceneBtnItem::paintBackground(QPainter &painter, uint32_t stage) noexcept
{
//    QColor color = stateColor(_state);
    painter.fillRect(_rect, color(0));
//    painter.fillRect(_rect, _backgroundColor);
}

void KRPTSceneBtnItem::paintForeground(QPainter &painter, uint32_t stage) noexcept
{
    painter.setRenderHint(QPainter::Antialiasing);


//    QPen pen(_borderColor, 1);
    QPen pen(color(1), 3);
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
