#include "datahandler.h"

DataHandler::DataHandler(MainWindow *plotToolbox)
    : _plotToolbox{plotToolbox}
{
}

void DataHandler::RegisterPlot(PlotWindow* plotWnd)
{
    _plotToolbox->AddPlot(plotWnd);
}

void DataHandler::GetServerAddress(std::string& ip, uint16_t& port)
{
    if (!_plotToolbox) return;

    _plotToolbox->GetServerAddress(ip, port);
}