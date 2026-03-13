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

#include <QGraphicsView>

#include "KRPTScene.h"
#include "KRPTSceneEllipseItem.h"
#include "KRPTSceneRectItem.h"

//####################################################################################################
//#
//####################################################################################################

#if 0
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
    SceneMouseEvent::Ptr createMouseSceneEvent(QSinglePointEvent *e) noexcept;
    SceneMouseEvent::Ptr createWheelSceneEvent(QWheelEvent       *e) noexcept;
private:
    KRPTScene  *_scene  ;

    KRPTSceneItem *_root;
    KRPTSceneItem *_item;
    int _tag = 0;
//    std::unique_ptr<ScenePainter>  _painter;
};

#else

class KRPTSceneWidget : public QGraphicsView
{
Q_OBJECT
friend class KRPTScene;
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

//    void drawForeground(QPainter* p, const QRectF&) override;

    void update() ;

private:
    SceneMouseEvent::Ptr createMouseSceneEvent(QSinglePointEvent *e) noexcept;
    SceneMouseEvent::Ptr createWheelSceneEvent(QWheelEvent       *e) noexcept;
private:
    KRPTScene  *_scene  ;

    KRPTSceneItem *_root;
    KRPTSceneItem *_item;
    int _tag = 0;
//    std::unique_ptr<ScenePainter>  _painter;
};

#endif





