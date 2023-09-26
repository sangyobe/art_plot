#include "plotconfig.h"
#include "ui_plotconfig.h"
#include "itemviewdelegate.h"
#include <QSpinBox>
#include <QDebug>



PlotConfig::PlotConfig(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::PlotConfig)
{
    ui->setupUi(this);

    ItemViewDelegate* delegate = new ItemViewDelegate(ui->configview);
    ui->configview->setItemDelegate(delegate);
    ui->configview->setEditTriggers(QAbstractItemView::AllEditTriggers);

//    connect(ui->configview, SIGNAL(itemClicked(QTreeWidgetItem*, int)), SLOT((mySlot_Changed())));
    connect(ui->configview, SIGNAL(clicked(const QModelIndex&)), this, SLOT(OnItemClicked(const QModelIndex&)));
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

void PlotConfig::OnItemClicked(const QModelIndex& index)
{
    if ("Data series::__name__" == index.data(Qt::WhatsThisRole).toString()) {
        emit graphItemClicked(index.data(Qt::DisplayRole).toString(), index.row());
    }
}
