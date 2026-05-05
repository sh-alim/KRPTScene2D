#include "MainProcess.h"
#include <QtWidgets/QApplication>

//#include <vld.h>

#if 1
int main(int argc, char *argv[])
{
//    QCoreApplication::setAttribute(Qt::AA_DisableHighDpiScaling);
//    QCoreApplication::setAttribute(Qt::AA_Use96Dpi);

#if 0
    qputenv("QT_SCALE_FACTOR", "1");
    qputenv("QT_ENABLE_HIGHDPI_SCALING", "0");
    qputenv("QT_AUTO_SCREEN_SCALE_FACTOR", "0");
    qputenv("QT_SCREEN_SCALE_FACTORS", "1");
#endif

    QApplication a(argc, argv);
    MainProcess w;
    w.show();
    return a.exec();
}

#else

#include <QApplication>
#include <QGraphicsView>
#include <QGraphicsRectItem>
#include <QElapsedTimer>
#include <QTimer>

static const int NUM_CHILDREN = 100000;   // ← меняй здесь

class ParentItem : public QGraphicsRectItem
{
public:
    ParentItem() : QGraphicsRectItem(-50, -50, 100, 100)
    {
        setFlag(QGraphicsItem::ItemIsMovable);
        setFlag(QGraphicsItem::ItemIsSelectable);
        setBrush(Qt::yellow);
    }
};

class TestView : public QGraphicsView
{
public:
    TestView(QGraphicsScene* scene)
        : QGraphicsView(scene)
    {
        setRenderHint(QPainter::Antialiasing, false);
//        setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
        setCacheMode(QGraphicsView::CacheNone);

        fpsTimer.start();
    }

protected:
    void paintEvent(QPaintEvent* event) override
    {
        QGraphicsView::paintEvent(event);

        frameCount++;

        qint64 elapsed = fpsTimer.elapsed();
        if (elapsed >= 1000) {
//            double fps = frameCount * 1000.0 / elapsed;
            double fps = 1000.0 / frameCount;// / elapsed;

            setWindowTitle(QString("FPS: %1").arg(fps, 0, 'f', 1));

            frameCount = 0;
            fpsTimer.restart();
        }
    }

private:
    QElapsedTimer fpsTimer;
    int frameCount = 0;
};

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QGraphicsScene scene;
    scene.setSceneRect(-5000, -5000, 10000, 10000);


    // Родительский item
    auto* parent = new ParentItem();
    scene.addItem(parent);

    // Добавляем N дочерних прямоугольников
    for (int i = 0; i < NUM_CHILDREN; i++) {
        auto* child = new QGraphicsRectItem(-5, -5, 10, 10, parent);
//        child->setBrush(Qt::gray);
        child->setCacheMode(QGraphicsItem::NoCache);
        // Раскладываем детей в сетку
        child->setPos((i % 200) * 12, (i / 200) * 12);
    }

    TestView view(&scene);
    view.resize(800, 600);
    view.show();

    return a.exec();
}

#endif
