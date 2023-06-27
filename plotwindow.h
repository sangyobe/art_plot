#ifndef PLOTWINDOW_H
#define PLOTWINDOW_H

#include <QMainWindow>
#include <memory>
#include <QStandardItem>

class PlotConfig;
class RecvThread;
class RecvThreadECal;
class RecvThreadEmul;
class DataSeriesEmul;

namespace Ui {
class PlotWindow;
}

class PlotWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit PlotWindow(QWidget *parent = nullptr);
    ~PlotWindow();

private:
    void BuildConfig();

public:
    int AddGraph(const QString &name, const QColor &color);

private slots:
    void OnNewDataArrived(DataSeriesEmul*, uint32_t size);
    void OnConfigChanged(QStandardItem* item);

private:
    Ui::PlotWindow *ui;
    PlotConfig* _plotConfig;
    QStandardItemModel* _configModel;
    std::unique_ptr<RecvThreadEmul> _recvThread;

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
    };
    ConfigOption _configOption;

private:
    void resizeEvent(QResizeEvent*);

    void AdjustPlotXRange();
    void AdjustPlotYRange();
};

#endif // PLOTWINDOW_H
