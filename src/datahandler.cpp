#include "datahandler.h"

DataHandler::DataHandler(MainWindow *plotToolbox)
    : _plotToolbox{plotToolbox}
{
    connect(_plotToolbox, SIGNAL(loadActionTriggered(QString)), this, SLOT(OnLoadTriggered(QString)));
    connect(_plotToolbox, SIGNAL(clearActionTriggered()), this, SLOT(OnClearTriggered()));
}

void DataHandler::RegisterPlot(PlotWindow *plotWnd)
{
    _plotToolbox->AddPlot(plotWnd);
}

void DataHandler::GetServerAddress(std::string &ip, uint16_t &port)
{
    if (!_plotToolbox) return;

    _plotToolbox->GetServerAddress(ip, port);
}

bool DataHandler::ClearPlotData()
{
    if (_plotToolbox && _plotToolbox->IsAutoClear())
    {
        _plotToolbox->clearActionTriggered();
        return true;
    }
    return false;
}

int DataHandler::GetDebugDataNum()
{
    if (!_plotToolbox) return DBGDATA_NUM_DFT;

    return _plotToolbox->GetDebugDataNum();
}

void DataHandler::OnLoadTriggered(QString filename)
{
}

void DataHandler::OnClearTriggered()
{
}