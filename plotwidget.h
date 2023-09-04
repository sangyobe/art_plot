#ifndef PLOTWIDGET_H
#define PLOTWIDGET_H

#include "qcustomplot.h"

class PlotWidget : public QCustomPlot
{
public:
    explicit PlotWidget(QWidget *parent = nullptr);
    virtual ~PlotWidget() Q_DECL_OVERRIDE;

    QCPRange getKeyRange(bool &foundRange);
    QCPRange getValueRange(bool &foundRange);

    void ExportToCSV(QString& export_dir);
    void ImportFromCSV(QStringList& import_files);

public slots:
    void ShowPointToolTip(QMouseEvent* event);
};

#endif // PLOTWIDGET_H
