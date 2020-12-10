#pragma once

#include <QtWidgets/QMainWindow>

#include "ui_QKD_Qt5_Interface.h"

class QKD_Qt5_Interface : public QMainWindow
{
    Q_OBJECT

public:
    QKD_Qt5_Interface(QWidget *parent = Q_NULLPTR);

private:
    void Test_Init();
private:
    Ui::QKD_Qt5_InterfaceClass ui;
};
