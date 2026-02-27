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

class KRPTSceneRectItem : public KRPTSceneItem
{
public:
    KRPTSceneRectItem(KRPTScene *scene, KRPTSceneItem *parent) noexcept;
    virtual ~KRPTSceneRectItem()                               noexcept;
protected:
    void outlineImpl     ()                  noexcept override;
    void paintBackground (QPainter &painter) noexcept override;
    void paintForeground (QPainter &painter) noexcept override;
};
