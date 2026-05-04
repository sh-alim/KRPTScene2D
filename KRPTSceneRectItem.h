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

class KRPTSceneRectItem : public KRPTSceneItem
{
public:
    KRPTSceneRectItem    (KRPTScene *scene, KRPTSceneItem *parent, 
                          const QRectF &geometry = QRectF(0, 0, 100, 100)) noexcept;
   ~KRPTSceneRectItem    ()                                                noexcept;
protected:
    void outlineImpl     ()                                                noexcept override;
    void paintBackground (QPainter &painter, uint32_t stage)               noexcept override;
    void paintForeground (QPainter &painter, uint32_t stage)               noexcept override;
};
