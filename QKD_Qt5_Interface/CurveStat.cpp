#include "CurveStat.h"
#include "StatLegend.h"

CurveStat::CurveStat(QWidget *parent)
    : QChartView(parent)
    , __axisX_max(100), __axisX_min(0)
    , __axisY_max(100), __axisY_min(0)
{
    __chart = new QChart();
    setChart(__chart);
    setRenderHints(QPainter::TextAntialiasing | QPainter::Antialiasing);

    __chart->createDefaultAxes();
    __chart->legend()->hide();

    __axisx = new QValueAxis();
    __axisx->setRange(__axisX_min, __axisX_max);
    __chart->addAxis(__axisx, Qt::AlignBottom);

    __axisy = new QValueAxis();
    __axisy->setRange(__axisY_min, __axisY_max);
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

    if (QAbstractSeries::SeriesTypeLine == series->type()) {
        QScatterSeries* scatter = new QScatterSeries();
        scatter->setColor(Qt::blue);
        scatter->setMarkerShape(QScatterSeries::MarkerShapeCircle);
        scatter->setMarkerSize(10);
        __assoc_scatter.insert(series, scatter);
    }
    __series.insert(name, series);
    __chart->addSeries(series);
}

void CurveStat::UpdateSeries(const QString& name, const QVector<QPointF>& data)
{
    if (__series.find(name) != __series.end()) {
        auto series = __series.value(name);
        series->replace(data);

        updateAxisRange(data);
        series->attachAxis(__axisx);
        series->attachAxis(__axisy);
        __chart->update();
    }
}

void CurveStat::UpdateSeries(const QString& name, double pt)
{
    if (__series.contains(name)) {
        auto series = __series.value(name);
        int data_count = series->count();
        if (data_count + 1 >= __MAX_X_SCOPE) {
            int i = 0;
            auto old_data = series->pointsVector();
            for (i = 0; i < data_count - 1; i++)
                old_data[i] = QPointF(i, old_data[i + 1].y());
            old_data.append(QPointF(i, pt));
            series->replace(old_data);
        } else 
            series->append(QPointF(data_count, pt));
        //这里将会对折线形的series关联的scatter进行更新

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

void CurveStat::HighlightVertices(bool h)
{
    __highlight_vertices = h;

}

void CurveStat::mouseMoveEvent(QMouseEvent* event)
{
    QPoint pts = event->pos();
    __pos_text->setPos(pts);
    QPointF map_pt = __chart->mapToValue(pts);
    __pos_text->setPlainText(QString("(%1, %2)").arg(QString::number(map_pt.x()), QString::number(map_pt.y())));

    return QChartView::mouseMoveEvent(event);
}

void CurveStat::updateAxisRange(const QVector<QPointF>& data)
{
    double min_axisX = UINT32_MAX, max_axisX = -UINT32_MAX,
           min_axisY = UINT32_MAX, max_axisY = -UINT32_MAX;

    for (auto d : data) {
        if (d.x() < min_axisX) min_axisX = d.x();
        if (d.x() > max_axisX) max_axisX = d.x();
        if (d.y() < min_axisY) min_axisY = d.y();
        if (d.y() > max_axisY) max_axisY = d.y();
    }
    max_axisX += 5;
    max_axisY += 5;
    __axisx->setRange(min_axisX, max_axisX);
    __axisy->setRange(min_axisY, max_axisY);
}
