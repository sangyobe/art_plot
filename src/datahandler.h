#ifndef DATAHANDLER_H
#define DATAHANDLER_H

#include <QObject>
#include "mainwindow.h"
#include "plotwindow.h"

class DataHandler : public QObject
{
    Q_OBJECT
public:
    explicit DataHandler(MainWindow *plotToolbox);

protected:
    void RegisterPlot(PlotWindow* plotWnd);

protected:
    MainWindow* _plotToolbox{nullptr};
};

#endif // DATAHANDLER_H
