#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "plotwindow.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(const std::string& ip = "127.0.0.1", const uint16_t port = 0, QWidget *parent = nullptr);
    ~MainWindow();

public:
    void AddPlot(PlotWindow* plotWnd);
    void GetServerAddress(std::string& ip, uint16_t& port);

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
};

#endif // MAINWINDOW_H
