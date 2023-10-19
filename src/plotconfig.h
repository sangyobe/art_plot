#ifndef PLOTCONFIG_H
#define PLOTCONFIG_H

#include <QDockWidget>
#include <QTreeWidgetItem>
#include <QAbstractItemModel>

namespace Ui {
class PlotConfig;
}

class PlotConfig : public QDockWidget
{
    Q_OBJECT

public:
    explicit PlotConfig(QWidget *parent = nullptr);
    ~PlotConfig();

    void setConfigModel(QAbstractItemModel* configModel);

private slots:
    void OnItemClicked(const QModelIndex& index);
signals:
    void graphItemClicked(QString name, int index);

private:
    Ui::PlotConfig *ui;
};

#endif // PLOTCONFIG_H
