#ifndef PLOTCONFIG_H
#define PLOTCONFIG_H

#include <QDockWidget>

namespace Ui {
class PlotConfig;
}

class PlotConfig : public QDockWidget
{
    Q_OBJECT

public:
    explicit PlotConfig(QWidget *parent = nullptr);
    ~PlotConfig();

private:
    Ui::PlotConfig *ui;
};

#endif // PLOTCONFIG_H
