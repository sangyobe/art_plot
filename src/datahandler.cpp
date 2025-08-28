#include "datahandler.h"
#include "mainwindow.h"

DataHandler::DataHandler(MainWindow *plotToolbox, const char *profileName)
    : _plotToolbox(plotToolbox)
    , _profileName(profileName)
{
    connect(_plotToolbox, SIGNAL(loadActionTriggered(QString)), this, SLOT(OnLoadTriggered(QString)));
    connect(_plotToolbox, SIGNAL(clearActionTriggered()), this, SLOT(OnClearTriggered()));
}

DataHandler::~DataHandler()
{
}

const std::string& DataHandler::GetProfileName() const 
{ 
    return _profileName; 
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

int DataHandler::GetDebugDataXYNum()
{
    if (!_plotToolbox) return 16;

    return _plotToolbox->GetDebugDataXYNum();
}

bool DataHandler::IsFetchInvisibleData()
{
    if (!_plotToolbox) return false;

    return _plotToolbox->IsFetchInvisibleData();
}

void DataHandler::OnLoadTriggered(QString filename)
{
}

void DataHandler::OnClearTriggered()
{
}