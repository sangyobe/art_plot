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
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public:
    void AddPlot(PlotWindow* plotWnd);

public slots:
    void OnPlotWndHidden(QWidget* widget);
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
};

#endif // MAINWINDOW_H
