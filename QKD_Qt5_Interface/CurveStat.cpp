#include "CurveStat.h"
#include "StatLegend.h"

CurveStat::CurveStat(QWidget *parent)
    : QChartView(parent)
{
    __chart = new QChart();
    setChart(__chart);
    setRenderHints(QPainter::TextAntialiasing | QPainter::Antialiasing);

    __chart->createDefaultAxes();

    __axisx = new QValueAxis();
    __axisx->setRange(0, 100);
    __chart->addAxis(__axisx, Qt::AlignBottom);

    __axisy = new QValueAxis();
    __axisy->setRange(0, 100);
    __chart->addAxis(__axisy, Qt::AlignLeft);

    __pos_text = new QGraphicsTextItem();
    scene()->addItem(__pos_text);

    setMouseTracking(true);
}

CurveStat::~CurveStat()
{
}

void CurveStat::AddSeries(const QString& name, QXYSeries* series, SET_ATTR_FUNC func)
{
    if (__series.contains(name)) {
        //warn
        return;
    }
    series->setVisible();
    series->setName(name);
    series->useOpenGL();
    func(series);

    __series.insert(name, series);
    __chart->addSeries(series);
}

void CurveStat::UpdateSeries(const QString& name, const QVector<QPointF>& data)
{
    if (__series.find(name) != __series.end()) {
        auto series = __series.value(name);
        series->replace(data);
        series->attachAxis(__axisx);
        series->attachAxis(__axisy);
        __chart->update();
    }
}

void CurveStat::SetLegendVisible(bool vi)
{
    switch (vi)
    {
    case true:
        for (auto lg : __legends)
            lg->show();
        break;
    default:
        for (auto lg : __legends)
            lg->hide();
        break;
    }
}

void CurveStat::mouseMoveEvent(QMouseEvent* event)
{
    QPoint pts = event->pos();
    __pos_text->setPos(pts);
    QPointF map_pt = __chart->mapToValue(pts);
    __pos_text->setPlainText(QString("(%1, %2)").arg(QString::number(map_pt.x()), QString::number(map_pt.y())));

    return QChartView::mouseMoveEvent(event);
}
