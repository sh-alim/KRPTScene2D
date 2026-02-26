//####################################################################################################
//#
//####################################################################################################

#include "KRPTSceneAnim.h"

//####################################################################################################
//#
//####################################################################################################

KRPTSceneAnim::KRPTSceneAnim(uint32_t id, const Event &event, int duration, QEasingCurve easingCurve) noexcept
    : QAbstractAnimation(), _id(id), _event(event), _duration(_duration), _easingCurve(easingCurve)
//    _easing(QEasingCurve::Linear)
//    _easingCurve(QEasingCurve::OutExpo)
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

QEasingCurve KRPTSceneAnim::easingCurve() const noexcept
{
    return _easingCurve;
}

void KRPTSceneAnim::setDuration(int duration) noexcept
{
    _duration = duration;
}

void KRPTSceneAnim::setEasingCurve(QEasingCurve easingCurve) noexcept
{
    _easingCurve = easingCurve;
}

void KRPTSceneAnim::updateCurrentTime(int time)
{
    if(!_event)return;
    _event(_id, time, _easingCurve.valueForProgress(static_cast<double>(time) / static_cast<double>(_duration)));
}

//****************************************************************************************************
//*
//****************************************************************************************************

