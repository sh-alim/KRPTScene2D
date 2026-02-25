//####################################################################################################
//#
//####################################################################################################

#include "KRPTSceneAnim.h"
#include "KRPTSceneAnimItem.h"

//####################################################################################################
//#
//####################################################################################################

KRPTSceneAnim::KRPTSceneAnim(KRPTSceneAnimItem *item, uint32_t id, Event event) noexcept
    : QAbstractAnimation(), _item(item), _id(id), _event(event),
    _easing(QEasingCurve::OutExpo), _duration(1000)
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

void KRPTSceneAnim::updateCurrentTime(int time)
{
    double value = _easing.valueForProgress(static_cast<double>(time) / static_cast<double>(_duration));

//    qDebug() << time << value;

    if(_event)_event(this, _id, value);
}

void KRPTSceneAnim::updateDirection(QAbstractAnimation::Direction direction)
{
}

void KRPTSceneAnim::updateState(QAbstractAnimation::State newState, QAbstractAnimation::State oldState)
{
//    qDebug() << newState << oldState;

}
