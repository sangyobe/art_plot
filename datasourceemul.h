#ifndef DATASOURCEEMUL_H
#define DATASOURCEEMUL_H

#include "datasource.h"
#include <QObject>
#include <QTimer>

class DataSeriesEmul
{
public:
    double time;
    double data[10];
};

class DataSourceEmul : public DataSource
{
    Q_OBJECT
public:
    DataSourceEmul(const QString &name, QObject *parent = nullptr);
    virtual ~DataSourceEmul();

public:
signals:
    void new_data(const DataSeriesEmul& data);

private slots:
    void OnTimer();

private:
    QTimer* _pubTimer;
    double _time;
};

#endif // DATASOURCEEMUL_H
