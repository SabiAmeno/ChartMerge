#pragma once

#include <QBrush>
#include <QPen>
#include <QMap>
#include <QGraphicsItem>

enum LegendAlign
{
    ALIGN_TOP,
    ALIGN_RIGHT
};

class StatLegend : public QObject, public QGraphicsItem
{
    Q_OBJECT
public:
    StatLegend(QGraphicsItem* parent = nullptr);

    void SetVisible(bool vi) { m_visible = vi; }
    void SetTitle(const QString& title) { m_title = title; }

    virtual QRectF boundingRect() const override;
    void SetRegion(const QRect& rect);
    void SetPen(const QPen& pen) { m_pen = pen; }
    void SetBrush(const QBrush& brush) { m_brush = brush; }
signals:
    void legendClicked(const QString& name, bool vi);
protected:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent* event);
    virtual void hoverMoveEvent(QGraphicsSceneHoverEvent* event);
    virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = Q_NULLPTR);
private:
    QRect m_rect;
    QPen m_pen;
    QBrush m_brush;

    bool m_visible;
    QString m_title;
};

class LegendGroup
{
public:
    LegendGroup(QGraphicsScene* scene);
    ~LegendGroup();

    void UpdateLegendName(const QString& oldname, const QString& newname);
    void UpdateLegendSize(int wd, int hg);
    StatLegend* AddLegend(const QString& name, const QColor& clor);
    void SetVisible(bool vi);
private:
    void updateLegend();

private:
    int __width;
    int __height;

    QMap<QString, StatLegend*> __legends;
    QGraphicsScene* __scene;
};