#include "plotconfig.h"
#include "ui_plotconfig.h"
#include <QSpinBox>
#include <QDebug>



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

void PlotConfig::setConfigModel(QAbstractItemModel *configModel)
{
    ui->configview->setModel(configModel);
    ui->configview->setColumnWidth(0, 150);
    ui->configview->expandAll();
}
