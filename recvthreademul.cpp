#include "recvthreademul.h"
#include <QtMath>



RecvThreadEmul::RecvThreadEmul(QObject *parent)
    : RecvThread(parent)
{
    //connect(this, SIGNAL(), this->parent(), SLOT());
    //connect(this->parent(), SIGNAL(), this, SLOT());

    //connect(this, SIGNAL(new_data_arrived(double*, uint32_t)), this->parent(), SLOT(OnNewDataArrived(double*, uint32_t)));
    //connect(this, &new_data_arrived, this->parent(), &PlotWindow::OnNewDataArrived);
    //connect(_minimap, &Minimap::minimap_scrolled, this, &MapView::on_minimap_scrolled);

    Initialize();

}

RecvThreadEmul::~RecvThreadEmul()
{
    delete _dataSource;
}

void RecvThreadEmul::Initialize()
{
    _dataIndex = 0;

    _dataSource = new DataSourceEmul("Sample Data Series");
    connect(_dataSource, SIGNAL(new_data(const DataSeriesEmul&)), this, SLOT(OnRecv(const DataSeriesEmul&)));

}

void RecvThreadEmul::OnRefreshPlot()
{
    qDebug("RecvThreadEmul::OnRefreshPlot");
    emit add_data_refresh_plot(_dataBuffer, _dataIndex);
    _dataIndex = 0;
}

void RecvThreadEmul::OnRecv(const DataSeriesEmul& data)
{
    //qDebug("RecvThreadEmul::OnRecv");
    if (_dataIndex >= DATA_INDEX_MAX)
    {
        qDebug("Buffer overflow");
        return;
    }
    _dataBuffer[_dataIndex++] = data;
}

void RecvThreadEmul::run()
{
    _refreshPlotTimer = new QTimer(this);
    connect(_refreshPlotTimer, SIGNAL(timeout()), this, SLOT(OnRefreshPlot()));
    _refreshPlotTimer->setInterval(100);
    _refreshPlotTimer->start();
    exec();
    _refreshPlotTimer->stop();
}


