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

enum class PlotType { RT_PLOT, IM_PLOT };

class PlotWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit PlotWindow(QWidget *parent = nullptr, PlotType type = PlotType::RT_PLOT);
    ~PlotWindow();

    PlotWidget *plot() const;
    QCPGraph *graph(int index) const;

public:
    int AddGraph(const QString &name, const QColor &color, int line_width = 1, int scatter_shape = 0, int scatter_skip = 0, bool visible = true);
    void SetGraphVisible(const QString &name, bool visible);
    void ResetPlot();   // clear data and remove all graphs
    void ResetData();   // clear data only
    void Replot();
    void SetWindowTitle(const QString& title);
    QString GetWindowTitle() const;
    void ShowLegend(bool show);
    // config x-Axis
    void AutoScroll(bool on);
    void AutoScrollWindow(double dt_sec);
    void SetXRange(double ti, double tf);
    // config y-Axis
    void AutoScale(bool on);
    void SetYRange(double lbound, double ubound);

    void DataUpdated(double recv_time);

    void RecalculatePlotLayout();
    PlotType GetType() { return _plotType; }

protected:
    void ExtendAll();

protected:
    void hideEvent(QHideEvent* event);

signals:
    void widgetHidden(QWidget* widget);

private:
    void BuildConfig();

private slots:
    void OnConfigChanged(QStandardItem* item);
    void OnHorzScrollBarChanged(int value);
    void OnVertScrollBarChanged(int value);
    void OnXAxisRangeChanged(QCPRange range);
    void OnYAxisRangeChanged(QCPRange range);
    void OnSelectionChangedByUser();
    void OnClearTriggered();
    void OnResetTriggered();
    void OnImportTriggered();
    void OnAppendTriggered();
    void OnExportTriggered();
    void OnExtendAllTriggered();
    void OnMousePressed(QMouseEvent*);
    void OnMouseReleased(QMouseEvent*);

private:
    Ui::PlotWindow *ui;
    PlotConfig* _plotConfig;
    QStandardItemModel* _configModel;
    PlotType _plotType;

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
        unsigned int line_width;
    };
    ConfigOption _configOption;

private:
    void resizeEvent(QResizeEvent*);
    void closeEvent(QCloseEvent*);

    void AdjustPlotXRange();
    void AdjustPlotYRange();

    const QStandardItem* FindFirstConfigOptionItem(const QString& cat, const QString& item);

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
