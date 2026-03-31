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
    KRPTSceneBtnItem(KRPTScene *scene, KRPTSceneItem *parent) noexcept;
    ~KRPTSceneBtnItem()                                       noexcept;
protected:
    void mousePressEvent  (SceneMouseEvent *e) noexcept override;
    void mouseReleaseEvent(SceneMouseEvent *e) noexcept override;
protected:
    void outlineImpl     ()                                  noexcept override;
    void paintBackground (QPainter &painter, uint32_t stage) noexcept override;
    void paintForeground (QPainter &painter, uint32_t stage) noexcept override;
private:
//    QColor _
};
