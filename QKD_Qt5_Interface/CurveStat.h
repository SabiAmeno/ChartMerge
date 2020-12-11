#pragma once

#include <QCursor>
#include <QtCharts/QChartView>
#include <QtCharts/QXYSeries>
#include <QtCharts/QValueAxis>
#include <QGraphicsTextItem>
#include <QtCharts/QScatterSeries>
#include "StatLegend.h"

QT_CHARTS_USE_NAMESPACE

constexpr static double g_axis_default_range = 100;

/*�������ߵ����ԣ��û���Ҫ���������Զ���series������*/
typedef void(*SET_ATTR_FUNC)(QXYSeries* series);

class SdFScatter : public QScatterSeries
{
    Q_OBJECT
public:
    explicit SdFScatter(QObject* parent = nullptr)
        : QScatterSeries(parent)
    {
        __render = new QScatterSeries(this);
        __render->setColor(Qt::red);
        __render->setMarkerShape(QScatterSeries::MarkerShapeCircle);
        __render->setMarkerSize(10);
        __render->setUseOpenGL();
        __render->hide();

        connect(this, &QScatterSeries::clicked, this, &SdFScatter::HighlightClicked);
        connect(this, &QScatterSeries::hovered, this, &SdFScatter::SetPointer);
        connect(this, &QScatterSeries::pointsReplaced, this, &SdFScatter::ClearHightClicked);
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
    void ClearHightClicked()
    {
        __render->clear();
        __render->hide();
    }

    void SetPointer(const QPointF& pos, bool state)
    {
        chart()->cursor().setShape(state ? Qt::PointingHandCursor : Qt::ArrowCursor);
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
    
    void RenameSeries(const QString& oldname, const QString& newname);
    void AddSeries(const QString& name, QXYSeries* series, SET_ATTR_FUNC func);
    void UpdateSeries(const QString& name, const QVector<QPointF>& data);
    void UpdateSeries(const QString& name, double pt);
    void SetLegendVisible(bool vi);

    /*����LineSeries��Ч*/
    void HighlightVertices(bool h);

    void SetAxisXScope(int scope) { __MAX_X_SCOPE = scope; }

private slots:
    void ShowPosition(const QPointF& pos);
    void ChangeCurveVisible(const QString& name, bool stat);
protected:
    virtual void mouseMoveEvent(QMouseEvent* event) override;
    virtual void resizeEvent(QResizeEvent* event) override;
private:
    void updateAxisRange(const QVector<QPointF>& data);
    void updateAxisUpperRange(int xmax, int ymax);
    void reAttachAxis(QAbstractSeries* series);
    void updateAssicScatter(QXYSeries* orig, QScatterSeries* assic_series);
private:
    QChart* __chart;

    QValueAxis* __axisx;
    QValueAxis* __axisy;

    LegendGroup __lgs;
    QGraphicsTextItem* __pos_text;

    QMap<QString, QXYSeries*> __series;
    QMap<QXYSeries*, QScatterSeries*> __assoc_scatter;

    double __axisX_min, __axisX_max;
    double __axisY_min, __axisY_max;

    /*ȫ�����߶�����ʾ�� ���ȼ�����ĳ�����ߵĿɼ���*/
    bool __highlight_vertices = true;

    /*��x������ֵ������ֵʱ���������ٸı䣬-1Ϊ�����������*/
    int __MAX_X_SCOPE = 100;
};
