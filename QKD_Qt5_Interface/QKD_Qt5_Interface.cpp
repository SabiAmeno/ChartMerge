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
    //series->setColor(clor);
    
    QPen pen;
    pen.setWidth(2);
    pen.setColor(clor);
    series->setPen(pen);
}

void SetCurveDash(QXYSeries* series)
{
    QColor clor(rand() % 256, rand() % 256, rand() % 256);
    QPen pen;
    pen.setWidthF(1.5);
    pen.setColor(clor);
    pen.setStyle(Qt::DashLine);
    series->setPen(pen);
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

    __timer.setInterval(100);
    connect(&__timer, &QTimer::timeout, this, &QKD_Qt5_Interface::GenNewDatas);

    Test_Init();

    connect(ui.actionShowVertices, &QAction::triggered, this, &QKD_Qt5_Interface::ShowGraphVertices);
    connect(ui.actionNewRand, &QAction::triggered, this, &QKD_Qt5_Interface::StartTimer);
}

void QKD_Qt5_Interface::GenNewDatas()
{
    ui.graphicsView->UpdateSeries("Curve1", rand() % 300);
    ui.graphicsView_2->UpdateSeries("Curve1", rand() % 300);
    ui.graphicsView_3->UpdateSeries("Curve1", rand() % 300);
    ui.graphicsView_4->UpdateSeries("Curve1", rand() % 300);
}

void QKD_Qt5_Interface::ShowGraphVertices(bool chkd)
{
    auto gvs = { ui.graphicsView, ui.graphicsView_2, ui.graphicsView_3, ui.graphicsView_4 };
    for (auto gv : gvs)
        gv->HighlightVertices(chkd);
}

void QKD_Qt5_Interface::StartTimer(bool chkd)
{
    chkd ? __timer.start() : __timer.stop();
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

        gv->AddSeries("Curve1", series, i%2 ? SetCurveAttr : SetCurveDash);
        if (!(i % 2)) {
            gv->SetAxisXScope(1600);
            gv->AddSeries("Curve2", new SdFScatter(), [](QXYSeries* series) {series->setColor(Qt::darkGreen); });
        }
        QVector<QPointF> _points;
        for (int i = 0; i < 10; i++) {
            _points.push_back(QPointF(i, rand() % 300));
        }
        gv->UpdateSeries("Curve1", _points);
        //if (!(i % 2))
        //    gv->UpdateSeries("Point1", _points);
        i++;
    }
}
