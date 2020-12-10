#pragma once

#include <QObject>
#include <QGraphicsItem>

class StatLegend : public QObject, public QGraphicsItem
{
    Q_OBJECT

public:
    StatLegend(QGraphicsItem* parent = nullptr);
    ~StatLegend();

    void hide();
    void show();

protected:

private:
};
