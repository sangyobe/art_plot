#include "datasourceemul.h"
#include <QtMath>
#include <QDebug>
#include <stdlib.h> // rand()
#include <time.h>
#include <random>

#define DATA_EMUL_PUB_PERIOD_MS (1)
#define DATA_EMUL_PUB_PERIOD ((double)DATA_EMUL_PUB_PERIOD_MS * 0.001)

DataSourceEmul::DataSourceEmul(const QString &name, QObject *parent)  : DataSource(name, parent), _time(0)
{
    _pubTimer = new QTimer(this);
    connect(_pubTimer, SIGNAL(timeout()), this, SLOT(OnTimer()));
    _pubTimer->setInterval(DATA_EMUL_PUB_PERIOD_MS);
    _pubTimer->start();

    std::srand((unsigned int)std::time(NULL));
}

DataSourceEmul::~DataSourceEmul()
{
    _pubTimer->stop();
}

void DataSourceEmul::OnTimer()
{
    static const double MAX_AMPLITUDE = 10.0;
    static const double FREQ = 0.1;
    static const double OFFSET = 0.0;

    static std::default_random_engine generator;
    static std::normal_distribution<double> dist(0.0, 0.05);
    static std::function<double()> noise_graussian = []() -> double {
        return dist(generator);
    };
    static std::function<double()> noise_white = []() -> double {
        return 0.1 * ((double)rand() / (double)RAND_MAX - 0.5);
    };

    DataSeriesEmul data;
    data.time = _time;
    data.data[0] = OFFSET + MAX_AMPLITUDE * (qSin(2.0*M_PI*FREQ*data.time) + noise_graussian());
    data.data[1] = OFFSET + MAX_AMPLITUDE * (qSin(2.0*M_PI*FREQ*data.time) * qSin(2.0*M_PI*FREQ*data.time) + noise_graussian());
    data.data[2] = OFFSET + MAX_AMPLITUDE * (qCos(2.0*M_PI*FREQ*data.time) + noise_white());
    data.data[3] = OFFSET + MAX_AMPLITUDE * (qCos(2.0*M_PI*FREQ*data.time) * qCos(2.0*M_PI*FREQ*data.time) + noise_white());
    emit new_data(data);
    _time += DATA_EMUL_PUB_PERIOD;
}
