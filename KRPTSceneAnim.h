//####################################################################################################
//#
//####################################################################################################

#pragma once

//####################################################################################################
//#
//####################################################################################################

#include <QAbstractAnimation>
#include <QEasingCurve>

#include <QDebug>

//####################################################################################################
//#
//####################################################################################################

class KRPTSceneAnimItem;

class KRPTSceneAnim : public QAbstractAnimation
{
public:
using Event = std::function<void(KRPTSceneAnim*, uint32_t, double)>;
public:
    KRPTSceneAnim(KRPTSceneAnimItem *item, uint32_t id, Event event) noexcept;
   ~KRPTSceneAnim() noexcept;
protected:
    int  duration         () const                                  override;
    void updateCurrentTime(int time)                                override;
    void updateDirection  (QAbstractAnimation::Direction direction) override;
    void updateState      (QAbstractAnimation::State newState, 
                           QAbstractAnimation::State oldState)      override;
private:
    KRPTSceneAnimItem *_item    ;
    uint32_t           _id      ;
    int                _duration;
    QEasingCurve       _easing  ;

    Event _event;

};
