#ifndef PLOTWIDGET_H
#define PLOTWIDGET_H

#include "qcustomplot.h"

class PlotWidget : public QCustomPlot
{
public:
    explicit PlotWidget(QWidget *parent = nullptr);
    virtual ~PlotWidget() Q_DECL_OVERRIDE;
};

#endif // PLOTWIDGET_H
