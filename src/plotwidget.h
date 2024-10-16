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

    void ExportData(QString &export_dir);
    void ImportData(QStringList &import_files);

private:
    void ImportCSV(QString filename);
    void ImportMcap(QString filename);

public slots:
    void ShowPointToolTip(QMouseEvent* event);
};

#endif // PLOTWIDGET_H
