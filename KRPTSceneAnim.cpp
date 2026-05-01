//########################################################################################################################
//#
//########################################################################################################################

#include "KRPTSceneAnim.h"

//########################################################################################################################
//#
//########################################################################################################################

KRPTSceneAnim::KRPTSceneAnim(uint32_t id, const Event &event, int duration, QEasingCurve easingCurve, int count) noexcept
    : QAbstractAnimation(), _event(event), _id(id), _duration(duration), _easingCurve(easingCurve)
{
    setLoopCount(count);
}

KRPTSceneAnim::~KRPTSceneAnim() noexcept
{
}

//************************************************************************************************************************
//*
//************************************************************************************************************************

int KRPTSceneAnim::duration() const 
{
    return _duration;
}

QEasingCurve KRPTSceneAnim::easingCurve() const noexcept
{
    return _easingCurve;
}

uint32_t KRPTSceneAnim::id() const noexcept
{
    return _id;
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
    _event(_id, time, _easingCurve.valueForProgress(static_cast<double>(time) / 
           static_cast<double>(_duration)), loopCount(), currentLoop());
}

//************************************************************************************************************************
//*
//************************************************************************************************************************

