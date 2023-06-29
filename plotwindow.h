#ifndef PLOTWINDOW_H
#define PLOTWINDOW_H

#include <QMainWindow>
#include <QStandardItem>
#include <memory>
#include <chrono>
#include "plotwidget.h"

//#define USE_EMUL_DATA

class PlotConfig;

#ifdef USE_EMUL_DATA
#include "datasourceemul.h"
class DataSeriesEmul;
#endif

namespace Ui {
class PlotWindow;
}

class PlotWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit PlotWindow(QWidget *parent = nullptr);
    ~PlotWindow();

    PlotWidget *plot() const;
    QCPGraph *graph(int index) const;

public:
    int AddGraph(const QString &name, const QColor &color);
    void ResetPlot();
    void SetWindowTitle(const QString& title);
    void AutoScroll(bool on);
    void AutoScrollWindow(double dt_sec);
    void AutoScale(bool on);
    void DataUpdated(double recv_time);

private:
    void BuildConfig();

private slots:
    void OnConfigChanged(QStandardItem* item);

    void on_actionNew_triggered();

private:
    Ui::PlotWindow *ui;
    PlotConfig* _plotConfig;
    QStandardItemModel* _configModel;

    class ConfigOption
    {
    public:
        ConfigOption();
        bool x_axis_auto_scroll;
        double x_axis_auto_scroll_window;
        double x_axis_begin_sec;
        double x_axis_end_sec;
        bool y_axis_auto_scale;
        double y_axis_lbound;
        double y_axis_ubound;
        bool legend_visible;
    };
    ConfigOption _configOption;

private:
    void resizeEvent(QResizeEvent*);

    void AdjustPlotXRange();
    void AdjustPlotYRange();


public slots:
    void OnRefreshPlot();
private:
    std::atomic<double> _lastRecvTime;
    std::atomic<bool> _isNewDataReceived;
    std::unique_ptr<QTimer> _refreshPlotTimer;

#ifdef USE_EMUL_DATA
public slots:
    void OnRecvEmul(const DataSeriesEmul& data);

private:
    std::unique_ptr<DataSourceEmul> _dataSource;
#endif
};

#endif // PLOTWINDOW_H
