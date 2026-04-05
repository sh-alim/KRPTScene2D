//########################################################################################################################
//#
//########################################################################################################################

#pragma once

//########################################################################################################################
//#
//########################################################################################################################

#include "KRPTSceneItem.h"

//########################################################################################################################
//#
//########################################################################################################################

class KRPTSceneBtnItem : public KRPTSceneItem
{
public:
    KRPTSceneBtnItem(KRPTScene *scene, KRPTSceneItem *parent)            noexcept;
    ~KRPTSceneBtnItem()                                                  noexcept;
protected:
    void stateChangeEvent(const FState &newState, const FState &oldState) noexcept override;

    void mousePressEvent  (SceneMouseEvent *e)                           noexcept override;
    void mouseReleaseEvent(SceneMouseEvent *e)                           noexcept override;
    void mouseEnterEvent  (bool enter)                                   noexcept override;
protected:
    void outlineImpl     ()                                              noexcept override;
    void animImpl        (uint32_t id, const std::vector<double> &value, 
                          uint32_t time, bool completed, int loop)       noexcept override;
    void paintBackground (QPainter &painter, uint32_t stage)             noexcept override;
    void paintForeground (QPainter &painter, uint32_t stage)             noexcept override;
protected:
    uint32_t _radius = 6;


};
