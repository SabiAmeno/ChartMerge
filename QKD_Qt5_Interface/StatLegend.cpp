#include "StatLegend.h"
#include <QGraphicsSceneMouseEvent>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsScene>
#include <QPainter>

StatLegend::StatLegend(QGraphicsItem* parent)
    : QGraphicsItem(parent),
    m_visible(true)
{
    setFlag(QGraphicsItem::ItemIsFocusable);

    setAcceptHoverEvents(true);
}

QRectF StatLegend::boundingRect() const
{
    return m_rect;
}

void StatLegend::SetRegion(const QRect& rect)
{
    m_rect = rect;
}

void StatLegend::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    if (event->buttons() & Qt::LeftButton) {
        m_visible = !m_visible;

        emit legendClicked(m_title, m_visible);
    }

    update();

    QGraphicsItem::mousePressEvent(event);
}

void StatLegend::hoverMoveEvent(QGraphicsSceneHoverEvent* event)
{
    setCursor(Qt::ArrowCursor);

    QGraphicsItem::hoverMoveEvent(event);
}

void StatLegend::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    QRect rect = option->rect;
    rect.setWidth(rect.width() - 40);
    
    QPainterPath path;
    path.addRoundedRect(rect, 2, 2);

    painter->save();
    painter->setPen(m_pen);
    painter->fillPath(path, m_brush);
    painter->drawText(rect.right() + 6, rect.top() + 10, m_title);
    painter->restore();

    if (!m_visible) {
        painter->save();
        painter->fillPath(path, QColor(100, 100, 100));
        painter->restore();
    }
}

LegendGroup::LegendGroup(QGraphicsScene* scene)
    : __scene(scene)
{

}

LegendGroup::~LegendGroup()
{
    for (auto lg : __legends)
        delete lg;
}

void LegendGroup::UpdateLegendName(const QString& oldname, const QString& newname)
{
    auto lg = __legends.value(oldname);
    if (lg) {
        lg->SetTitle(newname);
        __legends.remove(oldname);
        __legends.insert(newname, lg);
        lg->update();
    }
}

void LegendGroup::UpdateLegendSize(int wd, int hg)
{
    __width = wd;
    __height = hg;

    updateLegend();
}

StatLegend* LegendGroup::AddLegend(const QString& name, const QColor& clor)
{
    StatLegend* legend = new StatLegend();
    legend->SetTitle(name);
    legend->SetPen(clor);
    legend->SetBrush(clor);

    __legends.insert(name, legend);
    __scene->addItem(legend);

    return legend;
}

void LegendGroup::SetVisible(bool vi)
{
    for (auto lg : __legends)
        lg->SetVisible(vi);
}

void LegendGroup::updateLegend()
{
    int ind = 0;
    int hg = __height;
    int wth = __width;
    int size = __legends.size();

    int start = (wth - size * 80) / 2;

    for (auto lg : __legends)
        lg->SetRegion(QRect(start + ind++ * 80, hg, 70, 10));
}