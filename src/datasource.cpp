#include "datasource.h"

DataSource::DataSource(const QString &name, QObject *parent) : QObject(parent), _name(name)
{
}

DataSource::~DataSource() {}
