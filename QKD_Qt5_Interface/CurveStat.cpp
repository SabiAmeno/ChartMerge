#include "CurveStat.h"
#include <QEasingCurve>

CurveStat::CurveStat(QWidget *parent)
    : QChartView(parent)
    , __axisX_max(g_axis_default_range), __axisX_min(0)
    , __axisY_max(g_axis_default_range), __axisY_min(0)
    , __lgs(scene())
{
    __chart = new QChart();
    setChart(__chart);
    setRenderHints(QPainter::TextAntialiasing | QPainter::Antialiasing);

    __chart->createDefaultAxes();
    __chart->legend()->hide();
//    __chart->setBackgroundBrush(Qt::transparent);

    __axisx = new QValueAxis();
    __axisx->setRange(__axisX_min, __axisX_max);
    __chart->addAxis(__axisx, Qt::AlignBottom);

    __axisy = new QValueAxis();
    __axisy->setRange(__axisY_min, __axisY_max);
    __chart->addAxis(__axisy, Qt::AlignLeft);

    __pos_text = new QGraphicsTextItem();
    scene()->addItem(__pos_text);
    setMinimumSize(400, 100);
    setMouseTracking(true);
}

CurveStat::~CurveStat()
{
}

void CurveStat::ShowPosition(const QPointF& pos)
{
    QFont font;
    font.setFamily("Arial");
    font.setBold(true);
    font.setPointSize(10);

    __pos_text->setFont(font);
    __pos_text->setDefaultTextColor(Qt::red);
    __pos_text->setPos(__chart->mapToPosition(pos));
    __pos_text->setPlainText(QString("[ %1 , %2 ]").arg(QString::number(pos.x()), QString::number(pos.y())));
}


void CurveStat::ChangeCurveVisible(const QString& name, bool stat)
{
    auto series = __series.value(name);
    if (series) {
        series->setVisible(stat);

        auto assic_series = __assoc_scatter.value(series);
        if (assic_series)
            assic_series->setVisible(stat);
    }
}

void CurveStat::RenameSeries(const QString& oldname, const QString& newname)
{
    auto series = __series.value(oldname);
    if (series) {
        series->setName(newname);
        __series.remove(oldname);
        __series.insert(newname, series);

        __lgs.UpdateLegendName(oldname, newname);
    }
}

void CurveStat::AddSeries(const QString& name, QXYSeries* series, SET_ATTR_FUNC func)
{
    if (__series.contains(name)) {
        //warn
        return;
    }
    series->setVisible();
    series->setName(name);
    series->setUseOpenGL();
    func(series);

    __series.insert(name, series);
    __chart->addSeries(series);

    if (QAbstractSeries::SeriesTypeLine == series->type()) {
        QScatterSeries* scatter = new SdFScatter();
        scatter->setColor(Qt::blue);
        scatter->setMarkerShape(QScatterSeries::MarkerShapeRectangle);
        scatter->setMarkerSize(10);
        scatter->setUseOpenGL();
        __assoc_scatter.insert(series, scatter);
        __chart->addSeries(scatter);

        connect(scatter, &QScatterSeries::clicked, this, &CurveStat::ShowPosition);
    }
    StatLegend* legend = __lgs.AddLegend(name, series->color());
    connect(legend, &StatLegend::legendClicked, this, &CurveStat::ChangeCurveVisible);
}

void CurveStat::UpdateSeries(const QString& name, const QVector<QPointF>& data)
{
    auto series = __series.value(name);
    if (series) {
        series->replace(data);

        auto assic_series = __assoc_scatter.value(series);
        if (assic_series)
            updateAssicScatter(series, assic_series);

        updateAxisRange(data);
        reAttachAxis(series);
        __chart->update();
    }
}

void CurveStat::UpdateSeries(const QString& name, double pt)
{
    auto series = __series.value(name);
    if (series) {
        int data_count = series->count();
        /*实时显示新的数据，通过__MAX_X_SCOPE控制是否不显示过期的数据*/
        if (__MAX_X_SCOPE > 0 && data_count + 1 > __MAX_X_SCOPE) {
            int i = 0;
            auto old_data = series->pointsVector();
            QVector<QPointF> newdata(old_data.length());
            for (i = 0; i < data_count - 1; i++)
                newdata[i] = QPointF(i, old_data[i + 1].y());
            newdata[i] = QPointF(i, pt);
            series->replace(newdata);
        } else {
            /*当可视的区域未显示足够的数据时，不增长X轴范围*/
            __axisX_max += data_count > __axisX_max ? 1 : 0;
            series->append(QPointF(data_count, pt));
        }
        updateAxisUpperRange(__axisX_max, pt);

        //这里将会对折线形的series关联的scatter进行更新
        auto assic_series = __assoc_scatter.value(series);
        if (assic_series)
            updateAssicScatter(series, assic_series);

        //reAttachAxis(series);
        __chart->update();
    }
}

void CurveStat::SetLegendVisible(bool vi)
{
    __lgs.SetVisible(vi);
}

void CurveStat::HighlightVertices(bool h)
{
    __highlight_vertices = h;
    for (auto scat : __assoc_scatter)
        scat->setVisible(h);
    __chart->update();
}

void CurveStat::mouseMoveEvent(QMouseEvent* event)
{
    return QChartView::mouseMoveEvent(event);
}


void CurveStat::resizeEvent(QResizeEvent* event)
{
    QSize sz = event->size();
    __lgs.UpdateLegendSize(sz.width(), 20);

    QChartView::resizeEvent(event);
}

void CurveStat::updateAxisRange(const QVector<QPointF>& data)
{
    double min_axisX = UINT32_MAX, max_axisX = -UINT32_MAX,
           min_axisY = UINT32_MAX, max_axisY = -UINT32_MAX;

    for (auto d : data) {
        min_axisX = std::min(d.x(), min_axisX);
        max_axisX = std::max(d.x(), max_axisX);
        min_axisY = std::min(d.y(), min_axisY);
        max_axisY = std::max(d.y(), max_axisY);
    }

    max_axisX = std::max(g_axis_default_range, max_axisX);
    max_axisY = std::max(g_axis_default_range, max_axisY);

    __axisx->setRange(min_axisX, max_axisX);
    __axisy->setRange(min_axisY, max_axisY);
}

void CurveStat::updateAxisUpperRange(int xmax, int ymax)
{
    /*X轴最小值默认从0开始，这里不再更新*/
    __axisX_max = xmax > __axisX_max ? xmax : __axisX_max;
    __axisY_max = ymax > __axisY_max ? ymax + 2 : __axisY_max;
    __axisY_min = ymax < __axisY_min ? ymax : __axisY_min;

    __axisx->setMax(__axisX_max);
    __axisy->setMin(__axisY_min);
    __axisy->setMax(__axisY_max);
}

void CurveStat::reAttachAxis(QAbstractSeries* series)
{
    series->attachAxis(__axisx);
    series->attachAxis(__axisy);
}

void CurveStat::updateAssicScatter(QXYSeries* orig, QScatterSeries* assic_series)
{
    auto old_data = orig->points();
    assic_series->replace(old_data);
    assic_series->setVisible(__highlight_vertices && orig->isVisible());
    reAttachAxis(assic_series);
}
