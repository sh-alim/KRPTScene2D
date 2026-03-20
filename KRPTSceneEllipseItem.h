//####################################################################################################
//#
//####################################################################################################

#pragma once

//####################################################################################################
//#
//####################################################################################################

#include "KRPTSceneItem.h"

//####################################################################################################
//#
//####################################################################################################

class KRPTSceneEllipseItem : public KRPTSceneItem
{
public:
    KRPTSceneEllipseItem(KRPTScene *scene, KRPTSceneItem *parent) noexcept;
    virtual ~KRPTSceneEllipseItem()                               noexcept;
protected:
    void outlineImpl     ()                                  noexcept override;
    void paintBackground (QPainter &painter, uint32_t stage) noexcept override;
    void paintForeground (QPainter &painter, uint32_t stage) noexcept override;
};
