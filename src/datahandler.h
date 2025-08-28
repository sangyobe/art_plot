#ifndef DATAHANDLER_H
#define DATAHANDLER_H

#include <QObject>
#include "plotwindow.h"

class MainWindow;
class DataHandler : public QObject
{
    Q_OBJECT
public:
    explicit DataHandler(MainWindow *plotToolbox);
    virtual ~DataHandler();

private slots:
    virtual void OnLoadTriggered(QString filename);
    virtual void OnClearTriggered();

protected:
    void RegisterPlot(PlotWindow* plotWnd);
    void GetServerAddress(std::string& ip, uint16_t& port);
    bool ClearPlotData();
    int GetDebugDataNum();
    int GetDebugDataXYNum();
    bool IsFetchInvisibleData();

protected:
    MainWindow* _plotToolbox{nullptr};
};

#endif // DATAHANDLER_H
