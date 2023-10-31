#include "datahandler.h"

DataHandler::DataHandler(MainWindow *plotToolbox)
    : _plotToolbox{plotToolbox}
{

}

void DataHandler::RegisterPlot(PlotWindow* plotWnd)
{
    _plotToolbox->AddPlot(plotWnd);
}