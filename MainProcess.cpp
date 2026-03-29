//########################################################################################################################
//#
//########################################################################################################################

#include "MainProcess.h"

//########################################################################################################################
//#
//########################################################################################################################

static const std::vector<std::pair<KRPTSceneItem::Must, QString>> must =
{
    {KRPTSceneItem::Must::NoPaint               , "NoPaint"              },
    {KRPTSceneItem::Must::NoClipChilds          , "NoClipChilds"         },
    {KRPTSceneItem::Must::NoClipPainter         , "NoClipPainter"        },
    {KRPTSceneItem::Must::NoSceneScale          , "NoSceneScale"         },
    {KRPTSceneItem::Must::NoSceneRotate         , "NoSceneRotate"        },
    {KRPTSceneItem::Must::NoMouseEventTranslate , "NoMouseEventTranslate"},
    {KRPTSceneItem::Must::NoCheckChildVisibled  , "NoCheckChildVisibled" },
    {KRPTSceneItem::Must::MousePressEvent       , "MousePressEvent"      },
    {KRPTSceneItem::Must::MouseReleaseEvent     , "MouseReleaseEvent"    },
    {KRPTSceneItem::Must::MouseMoveEvent        , "MouseMoveEvent"       },
    {KRPTSceneItem::Must::WhellEvent            , "WhellEvent"           },
    {KRPTSceneItem::Must::TransformEvent        , "TransformEvent"       },
    {KRPTSceneItem::Must::ChildTransformEvent   , "SceneTransformEvent"  },
    {KRPTSceneItem::Must::SceneTransformEvent   , "SceneTransformEvent"  },
    {KRPTSceneItem::Must::SceneScaleEvent       , "SceneScaleEvent"      },
    {KRPTSceneItem::Must::SceneRotateEvent      , "SceneRotateEvent"     },
    {KRPTSceneItem::Must::Anim                  , "Anim"                 },
    {KRPTSceneItem::Must::AccuracyCheckContains , "AccuracyCheckContains"},
    {KRPTSceneItem::Must::AccuracyClip          , "AccuracyClip"         },
    {KRPTSceneItem::Must::MouseMoveble          , "MouseMoveble"         }
};

//########################################################################################################################
//#
//########################################################################################################################

MainProcess::MainProcess(QWidget *parent)
    : QMainWindow(parent)
{
//    _sceneWidget = new KRPTSceneWidget(this);
//    setCentralWidget(_sceneWidget);

    _view = new KRPTSceneView(this);
    _scene = new KRPTScene(_view);
    _scene->setBackgroundColor(QColor(30, 50, 50));

    _view->setTranslateEvents(true);

    setGeometry(300, 50, 1600, 1000);

    auto _root = _scene->addItem<KRPTSceneRectItem>();
    _root->setGeometry(QRectF(10, 10, 1000, 800));

    _root->addMust
    (
        KRPTSceneItem::Must::NoClipPainter,
//            KRPTSceneItem::Must::NoClipChilds,
//        KRPTSceneItem::Must::NoCheckChildVisibled,

//            KRPTSceneItem::Must::NoSceneRotate,
//            KRPTSceneItem::Must::NoSceneScale,
//            KRPTSceneItem::Must::AccuracyClip,
            KRPTSceneItem::Must::Anim,
//            KRPTSceneItem::Must::AccuracyClip,
//            KRPTSceneItem::Must::AccuracyCheckContains,
            KRPTSceneItem::Must::MouseMoveble,
            KRPTSceneItem::Must::MousePressEvent,
            KRPTSceneItem::Must::MouseMoveEvent,
            KRPTSceneItem::Must::WhellEvent
    );

    auto child = _root->addChild<KRPTSceneRectItem>();
    child->setGeometry(QRectF(50, 50, 150, 150));
    child->setBackgroundColor(QColor(0, 255, 0));

    child->addMust
    (
//            KRPTSceneItem::Must::NoClipChilds,
            KRPTSceneItem::Must::NoClipPainter,

//        KRPTSceneItem::Must::NoCheckChildVisibled,
 
//            KRPTSceneItem::Must::NoSceneRotate,
//            KRPTSceneItem::Must::NoSceneScale,
//            KRPTSceneItem::Must::AccuracyClip,
            KRPTSceneItem::Must::Anim,
//            KRPTSceneItem::Must::AccuracyClip,
//            KRPTSceneItem::Must::AccuracyCheckContains,
            KRPTSceneItem::Must::MouseMoveble,
            KRPTSceneItem::Must::MousePressEvent,
            KRPTSceneItem::Must::MouseMoveEvent,
            KRPTSceneItem::Must::WhellEvent
    );

//        child->setTransformAnchor(KRPTSceneItem::TransformAnchor::LeftTop);
//        child->setTransformAnchor(KRPTSceneItem::TransformAnchor::RightTop);
//        child->setTransformAnchor(KRPTSceneItem::TransformAnchor::LeftBottom);
//        child->setTransformAnchor(KRPTSceneItem::TransformAnchor::RightCenter);
//        child->setTransformAnchor(KRPTSceneItem::TransformAnchor::BottomCenter);
 
//        child->setPosAnchor(KRPTSceneItem::TransformAnchor::LeftTop);
//        child->setPosAnchor(KRPTSceneItem::TransformAnchor::Center);
//        child->setPosAnchor(KRPTSceneItem::TransformAnchor::RightTop);
//        child->setPosAnchor(KRPTSceneItem::TransformAnchor::LeftBottom);
//        child->setPosAnchor(KRPTSceneItem::TransformAnchor::RightCenter);
//        child->setPosAnchor(KRPTSceneItem::TransformAnchor::BottomCenter);


    child->setTag(1);

    _sliders.resize(6);
    int x = 10, y = 10;
    for(int i = 0; i < _sliders.size(); ++i)
    {
        _sliders[i] = new QSlider(Qt::Orientation::Horizontal, this); 
        _sliders[i]->setGeometry(x, y, 180, 20); 
        y += _sliders[i]->height();

        if(i == 0)
        {
            _sliders[i]->setMinimum(1);
            _sliders[i]->setMaximum(200);
            _sliders[i]->setValue(100);
        }
        if(i == 1)
        {
            _sliders[i]->setMinimum(0);
            _sliders[i]->setMaximum(360 * 2);
            _sliders[i]->setValue(0);
        }
        if(i == 4 || i == 5)
        {
            _sliders[i]->setMinimum(-100);
            _sliders[i]->setMaximum(1500);
            _sliders[i]->setValue(0);
        }
        connect(_sliders[i], &QSlider::valueChanged, [this, i] (int value)
        {
            if(!_selectedItem)return;
            if(i == 0)_selectedItem->setScale(value / 100.0);
            if(i == 1)_selectedItem->setAngle(value / 2.0);
            if(i == 2)_selectedItem->setWidth(value);
            if(i == 3)_selectedItem->setHeight(value);
            if(i == 4)_selectedItem->setX(value);
            if(i == 5)_selectedItem->setY(value);
        });
    }
    y +=10;

    _mustBtns.resize(must.size());

    for(int i = 0; i < _mustBtns.size(); ++i)
    {
        _mustBtns[i] = new QPushButton(this);
        _mustBtns[i]->setCheckable(true);
        _mustBtns[i]->setText(must[i].second);
        _mustBtns[i]->setGeometry(x, y, 180, 30);
        y += _mustBtns[i]->height();
        connect(_mustBtns[i], &QPushButton::toggled, [this, i](bool checked)
        {
            if(!_selectedItem)return;
            if(checked)_selectedItem->upMust(must[i].first);
            else _selectedItem->downMust(must[i].first);
        });
    }

#if 0
     QButtonGroup *group = new QButtonGroup(this);
     _btns.resize(urls.size());
    int x = 10;
    int y = 10;
    for(int i = 0; i < _btns.size(); ++i)
    {
        _btns[i] = new QPushButton(this);
        _btns[i]->setCheckable(true);
        _btns[i]->setText(urls[i].first);

        _btns[i]->setStyleSheet(btnStyle);
        _btns[i]->setGeometry(x, y, 130, 30);

        y += _btns[i]->height() + 5;
        group->addButton(_btns[i]);
        connect(_btns[i], &QPushButton::toggled, [this, i](bool checked)
        {
            if(checked)
            {
                _geoWidget->setSourceUrl(urls[i].second);
            }
        });
    }
#endif

}

MainProcess::~MainProcess()
{
    delete _scene;
}

//************************************************************************************************************************
//*
//************************************************************************************************************************

void MainProcess::resizeEvent(QResizeEvent *value)
{
    _view->setGeometry(200, 10, width() - 210, height() - 20);
}

void MainProcess::mousePressEvent(QMouseEvent *e)
{
    _selectedItem = _scene->itemFromPos(e->position(), [](KRPTSceneItem *item)
    {
//        return item->must(KRPTSceneItem::Must::MousePressEvent);
        return true;
    });
    if(!_selectedItem)return;

    QPointF p = _selectedItem->mapFromScene(e->position());
    QPointF p1 = _selectedItem->mapToParent(_selectedItem->posAnchorPoint());
    qDebug() << p1 << _selectedItem->pos();

    if(e->buttons() & Qt::MouseButton::LeftButton)
    {
        _sliders[0]->blockSignals(true);
        _sliders[0]->setValue(_selectedItem->scale() * 100);
        _sliders[0]->blockSignals(false);
        _sliders[1]->blockSignals(true);
        _sliders[1]->setValue(_selectedItem->angle() * 2);
        _sliders[1]->blockSignals(false);
        _sliders[2]->blockSignals(true);
        _sliders[2]->setMinimum(_selectedItem->width() / 2);
        _sliders[2]->setMaximum(_selectedItem->width() * 2);
        _sliders[2]->setValue(_selectedItem->width());
        _sliders[2]->blockSignals(false);
        _sliders[3]->blockSignals(true);
        _sliders[3]->setMinimum(_selectedItem->height() / 2);
        _sliders[3]->setMaximum(_selectedItem->height() * 2);
        _sliders[3]->setValue(_selectedItem->height());
        _sliders[3]->blockSignals(false);
        _sliders[4]->blockSignals(true);
        _sliders[4]->setMinimum(-100);
//        _sliders[4]->setMaximum(width());
        _sliders[4]->setMaximum(500);

        _sliders[4]->setValue(_selectedItem->x());
        _sliders[4]->blockSignals(false);
        _sliders[5]->blockSignals(true);
        _sliders[5]->setMinimum(-100);
        _sliders[5]->setMaximum(height());
        _sliders[5]->setValue(_selectedItem->y());
        _sliders[5]->blockSignals(false);

        for(int i = 0; i < _mustBtns.size(); ++i)
        {
            _mustBtns[i]->setChecked(_selectedItem->must(must[i].first));
        }
        _scene->update();
    }else

    if(e->buttons() & Qt::MouseButton::RightButton)
    {
        auto item = _scene->mousePressedItem();
        if(!item)return;
        QPointF p = item->mapFromScene(e->position());

        if(!(e->modifiers() & Qt::Modifier::CTRL))
        {
        auto child = item->addChild<KRPTSceneRectItem>();
        child->setBackgroundColor(QColor(0, 255, 0));
        child->addMust
        (
//            KRPTSceneItem::Must::NoClipChilds,
            KRPTSceneItem::Must::NoClipPainter,
            KRPTSceneItem::Must::NoSceneRotate,
            KRPTSceneItem::Must::NoSceneScale,
//            KRPTSceneItem::Must::AccuracyClip,
            KRPTSceneItem::Must::Anim,
//            KRPTSceneItem::Must::AccuracyClip,
//            KRPTSceneItem::Must::AccuracyCheckContains,
            KRPTSceneItem::Must::MouseMoveble,
            KRPTSceneItem::Must::MousePressEvent,
            KRPTSceneItem::Must::MouseMoveEvent,
            KRPTSceneItem::Must::WhellEvent
        );

        child->setSize(50, 50);
        child->setPos(p);

        child->setTag(100);

//        child->setGeometry(p, QSizeF(50, 50));
 
//        child->setTransformAnchor(KRPTSceneItem::TransformAnchor::LeftTop);
//        child->setTransformAnchor(KRPTSceneItem::TransformAnchor::RightTop);
//        child->setTransformAnchor(KRPTSceneItem::TransformAnchor::LeftBottom);
//        child->setTransformAnchor(KRPTSceneItem::TransformAnchor::RightCenter);
//        child->setTransformAnchor(KRPTSceneItem::TransformAnchor::BottomCenter);
//        child->setPosAnchor(KRPTSceneItem::TransformAnchor::LeftTop);
//        child->setPosAnchor(KRPTSceneItem::TransformAnchor::Center);
//        child->setPosAnchor(KRPTSceneItem::TransformAnchor::RightTop);
//        child->setPosAnchor(KRPTSceneItem::TransformAnchor::LeftBottom);
//        child->setPosAnchor(KRPTSceneItem::TransformAnchor::RightCenter);
        child->setPosAnchor(KRPTSceneItem::TransformAnchor::LeftCenter);
//        child->setPosAnchor(KRPTSceneItem::TransformAnchor::BottomCenter);
        }else
        {

        int x = 0;
        int y = 0;
        for(int i = 0; i < 500000; ++i)
        {
            auto child = item->addChild<KRPTSceneRectItem>();
            child->setBackgroundColor(QColor(0, 255, 0));
            child->addMust
            (
//            KRPTSceneItem::Must::NoClipChilds,
                KRPTSceneItem::Must::NoClipPainter,
//                KRPTSceneItem::Must::NoSceneRotate,
//                KRPTSceneItem::Must::NoSceneScale,
//            KRPTSceneItem::Must::AccuracyClip,
                KRPTSceneItem::Must::Anim,
//            KRPTSceneItem::Must::AccuracyClip,
//            KRPTSceneItem::Must::AccuracyCheckContains,
                KRPTSceneItem::Must::MouseMoveble,
                KRPTSceneItem::Must::MousePressEvent,
                KRPTSceneItem::Must::MouseMoveEvent,
                KRPTSceneItem::Must::WhellEvent
            );

            child->setSize(50, 50);
            child->setPos(x, y);
            child->setTag(200);


            x += child->width();

            if(x > item->width())
            {
                x = 0;
                y += child->height();
            }
        }
        }
    }

}

void MainProcess::mouseReleaseEvent(QMouseEvent *e)
{
}

void MainProcess::mouseMoveEvent(QMouseEvent *e)
{
}

void MainProcess::wheelEvent(QWheelEvent *e)
{
}

//************************************************************************************************************************
//*
//************************************************************************************************************************
