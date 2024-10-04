#ifndef PLOTWINDOW_H
#define PLOTWINDOW_H

#include <QMainWindow>
#include <QStandardItem>
#include <QMutex>
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
    int AddGraph(const QString &name, const QColor &color, const QString group_name = QString(""), int line_width = 1, int scatter_shape = 0, int scatter_skip = 0, bool visible = true);
    int GetGraphCount();
    void AddData(int gid, double key, double value);
    void SetGraphVisible(const QString &name, bool visible);
    void SelectGraph(const QString &name, int index = -1);
    void DeselectAllGraphs();
    void ResetPlot();   // clear data and remove all graphs
    void ResetData();   // clear data only
    void Replot();
    void SetWindowTitle(const QString& title);
    QString GetWindowTitle() const;
    // config x-Axis
    void AutoScroll(bool on);
    void AutoScrollWindow(double dt_sec);
    void SetXRange(double ti, double tf);
    void SetXBegin(double ti);
    void SetXEnd(double tf);
    // config y-Axis
    void AutoScale(bool on);
    void SetYRange(double lbound, double ubound);
    void SetYLBound(double lbound);
    void SetYUBound(double ubound);
    // config legend
    void ShowLegend(bool show);
    void SetLegendLocation(QFlags<Qt::AlignmentFlag> flag);
    // config style
    void SetLineWidth(int w);

    void DataUpdated(double recv_time);

    void RecalculatePlotLayout();
    PlotType GetType() { return _plotType; }

protected:
    void SetRefreshInterval(int ms);

    // drag & drop CSV file
    void dragEnterEvent(QDragEnterEvent* event);
    void dragLeaveEvent(QDragLeaveEvent* event);
    void dropEvent(QDropEvent* event);
    void dragMoveEvent(QDragEnterEvent* event);

protected:
    void ExtendAll();

protected:
    void hideEvent(QHideEvent* event);
    void closeEvent(QCloseEvent*);
    void keyPressEvent(QKeyEvent* event);
    void resizeEvent(QResizeEvent*);

signals:
    void widgetHidden(QWidget* widget);
    void widgetClosed(QWidget* widget);

private:
    void BuildConfig();
    QByteArray SavePlotConfig() const;
    bool RestorePlotConfig(const QByteArray &config);
    QByteArray SaveDataSeriesConfig() const;
    bool RestoreDataSeriesConfig(const QByteArray & config, const QString& name, QString& alias, QColor& color);
    QByteArray SavePlotOption() const;
    bool RestorePlotOption(const QByteArray &config);

private slots:
    void OnConfigChanged(QStandardItem* item);
    void OnConfigItemGraphClicked(QString name, int index);
    void OnConfigItemGraphColorSelected(QString name, int index, QColor color);
    void OnConfigItemGraphRestoreNameActionSelected(QString name, int index);
    void OnConfigItemGraphShowDataInfoActionSelected(QString name, int index);
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
    void OnCloseTriggered();
    void OnExtendAllTriggered();
    void OnMousePressed(QMouseEvent*);
    void OnMouseReleased(QMouseEvent*);
    //void OnRangeChanged(const QCPRange &newRange, const QCPRange &oldRange);
    void OnRefreshRateFastTriggered();
    void OnRefreshRateNormalTriggered();
    void OnRefreshRateSlowTriggered();

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
        QFlags<Qt::AlignmentFlag> legend_location;
        unsigned int style_line_width;
    };
    ConfigOption _configOption;

private:
    void AdjustPlotXRange();
    void AdjustPlotYRange();

    const QStandardItem* FindFirstConfigOptionItem(const QString& cat, const QString& item, int col = 0, bool recursive = false);
    const QStandardItem* FindFirstConfigOptionGroup(const QString& cat, const QString& group, int col = 0, bool recursive = false);

public slots:
    void OnRefreshPlot();

private:
    std::atomic<double> _lastRecvTime;
    std::atomic<bool> _isNewDataReceived;
    std::unique_ptr<QTimer> _refreshPlotTimer;
    QMutex _graphDataGuard;
    int _refreshInterval_ms;

#ifdef USE_EMUL_DATA
public slots:
    void OnRecvEmul(const DataSeriesEmul& data);

private:
    std::unique_ptr<DataSourceEmul> _dataSource;
#endif
};

#endif // PLOTWINDOW_H
