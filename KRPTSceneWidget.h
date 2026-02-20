//####################################################################################################
//#
//####################################################################################################

#pragma once

//####################################################################################################
//#
//####################################################################################################

#include <QWidget>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QPainter>
#include <QPen>

#include "KRPTScene.h"
#include "KRPTSceneItem.h"

//####################################################################################################
//#
//####################################################################################################

class KRPTSceneWidget : public QWidget
{
Q_OBJECT
public:
    KRPTSceneWidget(QWidget *parent) noexcept;
   ~KRPTSceneWidget() noexcept;
public:
//    void setScene(KRPTScene *scene) noexcept;
protected:
    void resizeEvent      (QResizeEvent *e) override;
    void mousePressEvent  (QMouseEvent  *e) override;
    void mouseReleaseEvent(QMouseEvent  *e) override;
    void mouseMoveEvent   (QMouseEvent  *e) override;
    void wheelEvent       (QWheelEvent  *e) override;
    void paintEvent       (QPaintEvent  *e) override;

private:
    SceneMouseEvent::Ptr createMouseSceneEvent(QMouseEvent *e) noexcept;
    SceneMouseEvent::Ptr createWheelSceneEvent(QWheelEvent *e) noexcept;

private:
    KRPTScene  *_scene  ;

    KRPTSceneItem *_item;

//    std::unique_ptr<ScenePainter>  _painter;
};








