//########################################################################################################################
//#
//########################################################################################################################

#pragma once

//########################################################################################################################
//#
//########################################################################################################################

#include "KRPTSceneItem.h"
#include "KRPTSceneAreaItem.h"
#include "KRPTSceneBtnItem.h"
#include "KRPTSceneSelectableBtnItem.h"

//########################################################################################################################
//#
//########################################################################################################################

class KRPTSceneBtnGridItem : public KRPTSceneAreaItem
{
public:
    enum class Direction {Horisontal, Vertical};
public:
    KRPTSceneBtnGridItem(KRPTScene *scene, KRPTSceneItem *parent, const QPointF &pos, 
                         const QSizeF &childSize, Direction direction = Direction::Horisontal, uint32_t childsPerLine = 1)             noexcept;
public:
    KRPTSceneBtnItem* addBtn(const QByteArray &imageSrc, const QString &text) noexcept;
    KRPTSceneSelectableBtnItem* addSelectableBtn() noexcept;


protected:
    void       paintBackground    (QPainter &painter, uint32_t stage)                     noexcept override;
    void       paintForeground    (QPainter &painter, uint32_t stage)                     noexcept override;
protected:
//    void       outlineImpl        ()                                                      noexcept override;
//    void       transformImpl      (SceneTransformEvent *e)                                noexcept override;
    void       childTransformEvent(KRPTSceneItem::Ptr item, 
                                   SceneTransformEvent *e)                                noexcept override;
    void       addChildImpl       (KRPTSceneItem::Ptr item, 
                                   KRPTSceneItem::Ptr parent)                             noexcept override;
private:
    QSizeF    _childSize;
    Direction _direction;
    uint32_t  _childsPerLine;

};

