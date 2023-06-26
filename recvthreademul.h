#ifndef RECVTHREADEMUL_H
#define RECVTHREADEMUL_H

#include "recvthread.h"
#include "datasourceemul.h"
#include <QTimer>

class RecvThreadEmul : public RecvThread
{
    Q_OBJECT
public:
    explicit RecvThreadEmul(QObject *parent = nullptr);
    virtual ~RecvThreadEmul();

    void run();

public:
    void Initialize();

signals:
    void add_data_refresh_plot(DataSeriesEmul* data, uint32_t len);

public slots:
    void OnRefreshPlot();
    void OnRecv(const DataSeriesEmul& data);

private:
    static const int DATA_INDEX_MAX = 1000;

    QTimer* _refreshPlotTimer;
    DataSourceEmul *_dataSource;
    DataSeriesEmul _dataBuffer[RecvThreadEmul::DATA_INDEX_MAX];
    int _dataIndex;
};


#endif // RECVTHREADEMUL_H
