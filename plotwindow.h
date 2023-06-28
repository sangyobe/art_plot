#ifndef PLOTWINDOW_H
#define PLOTWINDOW_H

#include <QMainWindow>
#include <QStandardItem>
#include <memory>
#include <chrono>
#include <ecal/ecal.h>
#include <ecal/msg/protobuf/subscriber.h>
#include <dtProto/QuadIP.pb.h>


//#define USE_EMUL_DATA


class PlotConfig;
class RecvThread;

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

public:
    static void OnRecv(const char* topic_name, const art_protocol::quadip::ControlStateTimeStamped& state, const long long time, const long long clock);
    static void OnRecv(const char* topic_name, const art_protocol::quadip::CpgStateTimeStamped& state, const long long time, const long long clock);

private:
    void BuildConfig();

public:
    int AddGraph(const QString &name, const QColor &color);

private slots:
    void OnConfigChanged(QStandardItem* item);

private:
    Ui::PlotWindow *ui;
    PlotConfig* _plotConfig;
    QStandardItemModel* _configModel;
    RecvThread* _recvThread;

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
    eCAL::protobuf::CSubscriber<art_protocol::quadip::ControlStateTimeStamped>* sub_control_state;
    eCAL::protobuf::CSubscriber<art_protocol::quadip::CpgStateTimeStamped>* sub_cpg_state;

private:
    void resizeEvent(QResizeEvent*);

    void AdjustPlotXRange();
    void AdjustPlotYRange();


public slots:
    void OnRefreshPlot();
private:
    double _lastRecvTime;
    std::atomic<bool> _isNewDataReceived;
    std::unique_ptr<QTimer> _refreshPlotTimer;

#ifdef USE_EMUL_DATA
signals:
    void add_data_refresh_plot(DataSeriesEmul* data, uint32_t len);

public slots:
    void OnNewDataArrived(DataSeriesEmul*, uint32_t size);
    void OnRecv(const DataSeriesEmul& data);

private:
    static const int DATA_INDEX_MAX = 1000;
    std::unique_ptr<DataSourceEmul> _dataSource;
    DataSeriesEmul _dataBuffer[PlotWindow::DATA_INDEX_MAX];
    int _dataIndex;
#endif
};

#endif // PLOTWINDOW_H
