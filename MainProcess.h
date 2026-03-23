//########################################################################################################################
//#
//########################################################################################################################

#pragma once

//########################################################################################################################
//#
//########################################################################################################################

#include <QtWidgets/QMainWindow>
//#include "KRPTSceneWidget.h"

#include "KRPTScene.h"
#include "KRPTSceneView.h"
#include "KRPTSceneRectItem.h"

//########################################################################################################################
//#
//########################################################################################################################

class MainProcess : public QMainWindow
{
Q_OBJECT
public:
    MainProcess(QWidget *parent = nullptr);
    ~MainProcess();
protected:
    void mousePressEvent  (QMouseEvent  *e) override;
    void mouseReleaseEvent(QMouseEvent  *e) override;
    void mouseMoveEvent   (QMouseEvent  *e) override;
    void wheelEvent       (QWheelEvent  *e) override;
    void resizeEvent      (QResizeEvent *e) override;
private:
    KRPTScene       *_scene     ;
    KRPTSceneView   *_view      ;
};
