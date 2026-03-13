//####################################################################################################
//#
//####################################################################################################

#pragma once

//####################################################################################################
//#
//####################################################################################################

#include <QtWidgets/QMainWindow>
#include "KRPTSceneWidget.h"

#include "KRPTScene.h"
#include "KRPTSceneView.h"


//####################################################################################################
//#
//####################################################################################################

class MainProcess : public QMainWindow
{
Q_OBJECT
public:
    MainProcess(QWidget *parent = nullptr);
    ~MainProcess();
protected:
    void resizeEvent(QResizeEvent    *value) override;
private:
    KRPTSceneWidget *_sceneWidget;
    KRPTScene       *_scene     ;
    KRPTSceneView   *_view      ;
};
