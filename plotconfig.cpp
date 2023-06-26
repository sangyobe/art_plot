#include "plotconfig.h"
#include "ui_plotconfig.h"

PlotConfig::PlotConfig(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::PlotConfig)
{
    ui->setupUi(this);
}

PlotConfig::~PlotConfig()
{
    delete ui;
}
