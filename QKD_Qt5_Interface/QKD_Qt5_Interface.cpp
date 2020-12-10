#include "QKD_Qt5_Interface.h"
#include <QtCharts/QSplineSeries>
#include <QtCharts/QScatterSeries>
#include <time.h>

void SetCurveAttr(QXYSeries* series)
{
    QColor clor;
    clor.setRed(rand() % 256);
    clor.setGreen(rand() % 256);
    clor.setBlue(rand() % 256);
    series->setColor(clor);
}

void SetPointAttr(QXYSeries* series)
{
    QScatterSeries* scatter = qobject_cast<QScatterSeries*>(series);
    scatter->setColor(Qt::darkBlue);
    scatter->setMarkerShape(QScatterSeries::MarkerShapeRectangle);
    scatter->setMarkerSize(5);
}

QKD_Qt5_Interface::QKD_Qt5_Interface(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

    __timer.setInterval(200);
    __timer.start();
    connect(&__timer, &QTimer::timeout, this, &QKD_Qt5_Interface::GenNewDatas);

    Test_Init();
}

void QKD_Qt5_Interface::GenNewDatas()
{
    int d = rand() % 100;
    ui.graphicsView->UpdateSeries("Curve1", d);
}

void QKD_Qt5_Interface::Test_Init()
{
    int i = 0;
    srand(time(NULL));

    auto gvs = { ui.graphicsView, ui.graphicsView_2, ui.graphicsView_3, ui.graphicsView_4 };
    for (auto gv : gvs) {
        QXYSeries* series = NULL;
        if (i % 2)
            series = new QSplineSeries();
        else
            series = new QLineSeries();

        gv->AddSeries("Curve1", series, SetCurveAttr);
        if (!(i % 2))
            gv->AddSeries("Point1", new SdFScatter(), SetPointAttr);

        QVector<QPointF> _points;
        for (int i = 0; i < 100; i++) {
            _points.push_back(QPointF(i, rand() % 100));
        }
        gv->UpdateSeries("Curve1", _points);
        if (!(i % 2))
            gv->UpdateSeries("Point1", _points);
        i++;
    }
}
