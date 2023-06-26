#include "datasourceemul.h"
#include <QtMath>

#define DATA_EMUL_PUB_PERIOD (0.001)

DataSourceEmul::DataSourceEmul(const QString &name, QObject *parent)  : DataSource(name, parent), _time(0)
{
    _pubTimer = new QTimer(this);
    connect(_pubTimer, SIGNAL(timeout()), this, SLOT(OnTimer()));
    _pubTimer->setInterval(static_cast<int>(1/*DATA_EMUL_PUB_PERIOD*1000*/));
    _pubTimer->start();
}

DataSourceEmul::~DataSourceEmul()
{
    _pubTimer->stop();
}

void DataSourceEmul::OnTimer()
{
    //qDebug("DataSourceEmul::OnTimer");

    static const double MAX_AMPLITUDE = 10.0;
    static const double FREQ = 0.1;

    DataSeriesEmul data;
    data.time = _time;
    data.data[0] = MAX_AMPLITUDE * qSin(2.0*M_PI*FREQ*data.time);
    data.data[1] = MAX_AMPLITUDE * qSin(2.0*M_PI*FREQ*data.time) * qSin(2.0*M_PI*FREQ*data.time);
    data.data[2] = MAX_AMPLITUDE * qCos(2.0*M_PI*FREQ*data.time);
    data.data[3] = MAX_AMPLITUDE * qCos(2.0*M_PI*FREQ*data.time) * qCos(2.0*M_PI*FREQ*data.time);
    emit new_data(data);
    _time += DATA_EMUL_PUB_PERIOD;
}
