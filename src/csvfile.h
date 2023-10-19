#ifndef CSVFILE_H
#define CSVFILE_H

#include <QFile>
#include <QObject>

class CSVFile : public QFile
{
    Q_OBJECT
public:
    explicit CSVFile(const QString &name);
};

#endif // CSVFILE_H
