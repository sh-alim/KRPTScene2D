//########################################################################################################################
//#
//########################################################################################################################

#include "MainProcess.h"

//########################################################################################################################
//#
//########################################################################################################################

static const std::vector<std::pair<KRPTSceneItem::Must, QString>> must =
{
    {KRPTSceneItem::Must::NoPaint                , "NoPaint"                },
    {KRPTSceneItem::Must::NoClipChilds           , "NoClipChilds"           },
    {KRPTSceneItem::Must::NoClipPainter          , "NoClipPainter"          },
    {KRPTSceneItem::Must::NoCheckChildVisibled   , "NoCheckChildVisibled"   },
    {KRPTSceneItem::Must::NoSceneScale           , "NoSceneScale"           },
    {KRPTSceneItem::Must::NoSceneRotate          , "NoSceneRotate"          },
    {KRPTSceneItem::Must::Anim                   , "Anim"                   },
    {KRPTSceneItem::Must::AccuracyCheckContains  , "AccuracyCheckContains"  },
    {KRPTSceneItem::Must::AccuracyClip           , "AccuracyClip"           },
    {KRPTSceneItem::Must::NoMouseEventTranslate  , "NoMouseEventTranslate"  },
    {KRPTSceneItem::Must::MouseTracking          , "MouseTracking"          },
    {KRPTSceneItem::Must::StateChangeEvent       , "StateChangeEvent"       },
    {KRPTSceneItem::Must::MousePressEvent        , "MousePressEvent"        },
    {KRPTSceneItem::Must::MouseMoveEvent         , "MouseMoveEvent"         },
    {KRPTSceneItem::Must::MouseEnterEvent        , "MouseEnterEvent"        },
    {KRPTSceneItem::Must::WhellEvent             , "WhellEvent"             },
    {KRPTSceneItem::Must::TransformEvent         , "TransformEvent"         },
    {KRPTSceneItem::Must::SceneTransformEvent    , "SceneTransformEvent"    },
    {KRPTSceneItem::Must::SceneScaleEvent        , "SceneScaleEvent"        },
    {KRPTSceneItem::Must::SceneRotateEvent       , "SceneRotateEvent"       },
    {KRPTSceneItem::Must::TransformToParentEvent , "TransformToParentEvent" },
    {KRPTSceneItem::Must::TransformToSceneEvent  , "TransformToSceneEvent"  },
    {KRPTSceneItem::Must::MousePressToParentEvent, "MousePressToParentEvent"},
    {KRPTSceneItem::Must::MousePressToSceneEvent , "MousePressToSceneEvent" },
    {KRPTSceneItem::Must::MouseMoveToParentEvent , "MouseMoveToParentEvent" },
    {KRPTSceneItem::Must::MouseMoveToSceneEvent  , "MouseMoveToSceneEvent"  },
    {KRPTSceneItem::Must::WhellToParentEvent     , "WhellToParentEvent"     },
    {KRPTSceneItem::Must::WhellToSceneEvent      , "WhellToSceneEvent"      },
    {KRPTSceneItem::Must::MouseMoved             , "MouseMoved"             },
    {KRPTSceneItem::Must::Checked                , "Checked"                }
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
    _view->setScene(_scene);

//    _view->setHwAccel(false);

    _scene->setBackgroundColor(QColor(30, 50, 50));
    _scene->setBorderColor(QColor(0, 255, 0));

    _view->setTranslateEvents(true);

    setGeometry(300, 50, 1800, 1400);

//    _root = _scene->addItem<KRPTSceneRectItem>();
//    _root = _scene->addItem<KRPTSceneScrolledAreaItem>(QRectF(10, 10, 500, 500));
//    _root = _scene->addItem<KRPTSceneAreaItem>();

    _root = _scene->addItem<KRPTSceneBtnGroupItem>(QRectF(10, 10, 70, 70));

//    _root = _scene->addItem<KRPTSceneBtnItem>(QRectF(100, 100, 50, 50));

        auto itm = dynamic_cast<KRPTSceneScrolledAreaItem*>(_root);
        if(itm)
        {
//            itm->setAreaSizePolicy(KRPTSceneScrolledAreaItem::AreaSizePolicy::AutoPosSize);
        }


    _root->setTag(++_tag);



#if 1
    double x1 = 5;
    double y1 = 5;
    for(int i = 0; i < 10; ++i)
    {
        auto child = _root->addChild<KRPTSceneBtnItem>(QRectF(x1, y1, 48, 48));

//        child->setScale(1.1);

//        auto child = _root->addChild<KRPTSceneRectItem>();
//        child->setGeometry(10, y1, 50, 50);
        y1 += 55;

//        if(x1 > 2000)
//        {
//            y1 += 55;
//            x1 = 15;
//        }
    }
//    ((KRPTSceneScrolledAreaItem*)_root)->setAreaSize(100, 5 * 60 + 10);
#endif

    _root->upMust
    (
//        KRPTSceneItem::Must::NoClipPainter,
//            KRPTSceneItem::Must::NoClipChilds,
//        KRPTSceneItem::Must::NoCheckChildVisibled,

//            KRPTSceneItem::Must::NoSceneRotate,
//            KRPTSceneItem::Must::NoSceneScale,
//            KRPTSceneItem::Must::AccuracyClip,
//            KRPTSceneItem::Must::Anim,
//            KRPTSceneItem::Must::AccuracyClip,
//            KRPTSceneItem::Must::AccuracyCheckContains,
//            KRPTSceneItem::Must::MouseMoved,
//            KRPTSceneItem::Must::MousePressEvent,
//            KRPTSceneItem::Must::MouseMoveEvent,
//            KRPTSceneItem::Must::WhellEvent
    );

#if 0
//    _i0 = _root->addChild<KRPTSceneRectItem>();
    _i0 = _scene->addItem<KRPTSceneRectItem>();
    _i0->setGeometry(QRectF(50, 50, 150, 150));
//    _i0->setBackgroundColor(QColor(0, 255, 0));

    _i0->upMust
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
            KRPTSceneItem::Must::MouseMoved,
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


    _i0->setTag(++_tag);
#endif
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
            if(i == 2)
            {
                auto itm = dynamic_cast<KRPTSceneScrolledAreaItem*>(_selectedItem);
                if(itm)
                {
                    itm->setAreaSize(QSizeF(value, itm->height()));
                }else
                _selectedItem->setWidth(value);
            }
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
            _scene->update();
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

    _root->setGeometry(50, 50, width() - 100, height() - 100);
}

void MainProcess::mousePressEvent(QMouseEvent *e)
{
    _mousePos = e->position();

//    if(_selectedItem)
//        _selectedItem->setBorderColor(QColor(255, 255, 255));

    _selectedItem = _scene->itemFromPos(e->position(), [](KRPTSceneItem *item)
    {
//        return item->must(KRPTSceneItem::Must::MousePressEvent);
        return true;
    });
    if(!_selectedItem)return;

//    _selectedItem->setBorderColor(QColor(0, 255, 0));


    QPointF p = _selectedItem->mapFromScene(e->position());
//    QPointF p1 = _selectedItem->mapToParent(_selectedItem->posAnchorPoint());
//    qDebug() << "[0]" << p;

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
            _mustBtns[i]->blockSignals(true);
            _mustBtns[i]->setChecked(_selectedItem->mustAny(must[i].first));
            _mustBtns[i]->blockSignals(false);
        }
        _scene->update();
    }else

    if(e->buttons() & Qt::MouseButton::RightButton)
    {
        auto item = _scene->mousePressedItem();
        if(!item)return;
        QPointF p = item->mapFromScene(e->position());

        auto itm = dynamic_cast<KRPTSceneScrolledAreaItem*>(item);
        if(itm)
        {
            p = itm->mapToArea(p);
        }

        if(!(e->modifiers() & Qt::Modifier::CTRL))
        {
            QRectF r(p, QSizeF(100, 100));

//        auto child = item->addChild<KRPTSceneRectItem>();
        auto child = item->addChild<KRPTSceneBtnItem>(r);

//        child->setBackgroundColor(QColor(0, 255, 0));

        child->upMust
        (
//            KRPTSceneItem::Must::NoClipChilds,
            KRPTSceneItem::Must::NoClipPainter,
//            KRPTSceneItem::Must::NoSceneRotate,
//            KRPTSceneItem::Must::NoSceneScale,
//            KRPTSceneItem::Must::AccuracyClip,

//            KRPTSceneItem::Must::SceneTransformEvent,
//            KRPTSceneItem::Must::SceneScaleEvent,

            KRPTSceneItem::Must::Anim,
//            KRPTSceneItem::Must::AccuracyClip,
//            KRPTSceneItem::Must::AccuracyCheckContains,
            KRPTSceneItem::Must::MouseMoved,
            KRPTSceneItem::Must::MousePressEvent,
            KRPTSceneItem::Must::MouseMoveEvent,
            KRPTSceneItem::Must::WhellEvent
        );

//        child->setSize(150, 50);
//        child->setPos(p);

        child->setTag(++_tag);

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
//        child->setPosAnchor(KRPTSceneItem::TransformAnchor::LeftCenter);
//        child->setPosAnchor(KRPTSceneItem::TransformAnchor::BottomCenter);
        }else
        {

        int x = 0;
        int y = 0;
        for(int i = 0; i < 100000; ++i)
        {
            auto child = item->addChild<KRPTSceneRectItem>();
//            child->setBackgroundColor(QColor(0, 255, 0));
            child->upMust
            (
//            KRPTSceneItem::Must::NoClipChilds,
                KRPTSceneItem::Must::NoClipPainter,
//                KRPTSceneItem::Must::NoSceneRotate,
//                KRPTSceneItem::Must::NoSceneScale,
//            KRPTSceneItem::Must::AccuracyClip,
                KRPTSceneItem::Must::Anim,
//            KRPTSceneItem::Must::AccuracyClip,
//            KRPTSceneItem::Must::AccuracyCheckContains,
                KRPTSceneItem::Must::MouseMoved,
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
#if 0
    auto item = _i0;

//    item->setScale((e->angleDelta().y() > 0 )? item->scale() + 0.01 : item->scale() - 0.01);
    item->setAngle((e->angleDelta().y() > 0 )? item->angle() + 0.5 : item->angle() - 0.5);
    if(_selectedItem)
    {
//    QPointF p = _selectedItem->mapFromScene(e->position());
//    qDebug() << "[1]" << p;// << e->position() << _mousePos;
    }
#endif
}

//************************************************************************************************************************
//*
//************************************************************************************************************************
