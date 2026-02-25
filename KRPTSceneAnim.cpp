//####################################################################################################
//#
//####################################################################################################

#include "KRPTSceneAnim.h"

//####################################################################################################
//#
//####################################################################################################

KRPTSceneAnim::KRPTSceneAnim(uint32_t id, const Event &event) noexcept
    : QAbstractAnimation(), _id(id), _event(event),
//    _easing(QEasingCurve::OutExpo)
    _easing(QEasingCurve::Linear)
    , _duration(1000)
{
}

KRPTSceneAnim::~KRPTSceneAnim() noexcept
{
}

//****************************************************************************************************
//*
//****************************************************************************************************

int KRPTSceneAnim::duration() const 
{
    return _duration;
}

void KRPTSceneAnim::setDuration(int duration) noexcept
{
    _duration = duration;
}

void KRPTSceneAnim::updateCurrentTime(int time)
{
    if(!_event)return;
    _event(_id, time, _easing.valueForProgress(static_cast<double>(time) / static_cast<double>(_duration)));
}

void KRPTSceneAnim::updateDirection(QAbstractAnimation::Direction direction)
{
}

void KRPTSceneAnim::updateState(QAbstractAnimation::State newState, QAbstractAnimation::State oldState)
{
}
