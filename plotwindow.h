#ifndef PLOTWINDOW_H
#define PLOTWINDOW_H

#include <QMainWindow>
#include <memory>

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

public:
    int AddGraph(const QString &name);

private slots:
    void OnNewDataArrived(DataSeriesEmul*, uint32_t size);

private:
    Ui::PlotWindow *ui;
    PlotConfig* _plotConfig;
    std::unique_ptr<RecvThreadEmul> _recvThread;

private:
    void resizeEvent(QResizeEvent*);
};

#endif // PLOTWINDOW_H
