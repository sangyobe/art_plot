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

private:
    Ui::PlotConfig *ui;
};

#endif // PLOTCONFIG_H
