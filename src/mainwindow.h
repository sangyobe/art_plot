#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "plotwindow.h"

#define DBGDATA_NUM_MAX (256) //<! maximum number of debug_data
#define DBGDATA_NUM_DFT (64)  //<! default number of debug_data
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(const std::string& ip = "127.0.0.1", const uint16_t port = 0, const int dnum = 0, QWidget *parent = nullptr);
    ~MainWindow();

public:
    void AddPlot(PlotWindow* plotWnd);
    void GetServerAddress(std::string& ip, uint16_t& port);
    int GetDebugDataNum();

private:
    QByteArray SavePlotConfig() const;
    bool RestorePlotConfig(const QByteArray & config, const QString& name);
    void SetPlotVisible(const QString &name, bool visible);

public slots:
    void OnPlotWndHidden(QWidget* widget);
    void OnPlotWndClosed(QWidget* widget);
    void OnConfigChanged(QStandardItem* item);
    void OnItemClicked(QModelIndex index);

private slots:
    void OnNewTriggered();
    void OnClearTriggered();
    void OnExitTriggered();

signals:
    void clearActionTriggered();

private:
    void resizeEvent(QResizeEvent*);
    void closeEvent(QCloseEvent*);

private:
    Ui::MainWindow *ui;
    QList<PlotWindow*> _plotWnds;
    QStandardItemModel* _plotListModel;
    std::string _svrIpAddr;
    uint16_t _svrPort;
    int _debug_data_num;
};

#endif // MAINWINDOW_H
