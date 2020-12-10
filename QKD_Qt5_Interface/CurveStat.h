#pragma once

#include <QtCharts/QChartView>
#include <QtCharts/QXYSeries>
#include <QtCharts/QValueAxis>
#include <QGraphicsTextItem>
#include <QtCharts/QScatterSeries>

QT_CHARTS_USE_NAMESPACE

typedef void(*SET_ATTR_FUNC)(QXYSeries*);

class StatLegend;


class SdFScatter : public QScatterSeries
{
    Q_OBJECT
public:
    explicit SdFScatter(QObject* parent = nullptr)
        : QScatterSeries(parent)
    {
        __render = new QScatterSeries(this);
        __render->setColor(Qt::darkGreen);
        __render->setMarkerShape(QScatterSeries::MarkerShapeCircle);
        __render->setMarkerSize(10);
        __render->hide();

        connect(this, &QScatterSeries::clicked, this, &SdFScatter::HighlightClicked);
    }
private slots:
    void HighlightClicked(const QPointF& pos)
    {
        if (chart()->series().count(__render));
        else
            chart()->addSeries(__render);

        __render->clear();
        __render->append(pos);

        auto axiss = chart()->axes();
        for (auto ax : axiss)
            __render->attachAxis(ax);
        __render->show();
    }

private:
    QScatterSeries* __render;
};

class CurveStat : public QChartView
{
    Q_OBJECT

public:
    CurveStat(QWidget* parent);
    ~CurveStat();

    void AddSeries(const QString& name, QXYSeries* series, SET_ATTR_FUNC func);
    void UpdateSeries(const QString& name, const QVector<QPointF>& data);
    void SetLegendVisible(bool vi);
protected:
    virtual void mouseMoveEvent(QMouseEvent* event) override;
private:
    QChart* __chart;

    QValueAxis* __axisx;
    QValueAxis* __axisy;

    QGraphicsTextItem* __pos_text;

    QMap<QString, QXYSeries*> __series;
    QMap<QString, StatLegend*> __legends;
};
