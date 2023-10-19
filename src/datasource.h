#ifndef DATASOURCE_H
#define DATASOURCE_H

#include <QObject>

class DataSource : public QObject
{
    Q_OBJECT
public:
    explicit DataSource(const QString &name, QObject *parent = nullptr);
    virtual ~DataSource();

signals:

public slots:

private:
    QString _name;
};

#endif // DATASOURCE_H
