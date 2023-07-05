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
    ui->configview->expandAll();
}

//void PlotConfig::BuildConfigUI()
//{
//    ui->propGrid->setColumnCount(2);
//    ui->propGrid->setColumnWidth(0, 150);

//    // header
//    QStringList header_labels;
//    header_labels << "Property" << "Value";
//    ui->propGrid->setHeaderLabels(header_labels);

//    //
//    QTreeWidgetItem* xaxis_root = new QTreeWidgetItem(ui->propGrid);
//    xaxis_root->setText(0, "x-Axis");
//    QTreeWidgetItem* xaxis_auto_scale = new QTreeWidgetItem;
//    xaxis_auto_scale->setText(0, "Auto scale");
//    xaxis_auto_scale->setWhatsThis(1, "x-Axis::AutoScroll");
//    xaxis_auto_scale->setFlags(xaxis_auto_scale->flags() | Qt::ItemIsUserCheckable);
//    xaxis_auto_scale->setCheckState(1, Qt::Checked);
//    xaxis_root->addChild(xaxis_auto_scale);
//    QTreeWidgetItem* xaxis_range = new QTreeWidgetItem;
//    xaxis_range->setText(0, "Range");
//    QTreeWidgetItem* xaxis_range_lb = new QTreeWidgetItem;
//    xaxis_range_lb->setText(0, "Begin (sec)");
//    xaxis_range_lb->setWhatsThis(1, "x-Axis::LBound");
//    xaxis_range_lb->setText(1, "0");
//    xaxis_range_lb->setFlags(xaxis_range_lb->flags() | Qt::ItemIsEditable);
//    QTreeWidgetItem* xaxis_range_ub = new QTreeWidgetItem;
//    xaxis_range_ub->setText(0, "End (sec)");
//    xaxis_range_ub->setWhatsThis(1, "x-Axis::UBound");
//    xaxis_range_ub->setText(1, "10");
//    xaxis_range_ub->setFlags(xaxis_range_ub->flags() | Qt::ItemIsEditable);
//    xaxis_range->addChild(xaxis_range_lb);
//    xaxis_range->addChild(xaxis_range_ub);
//    xaxis_root->addChild(xaxis_range);
//    //ui->propGrid->setItemWidget(xaxis_range_lb, 1, new QSpinBox());
//    //ui->propGrid->setItemWidget(xaxis_range_ub, 1, new QSpinBox());


//    QTreeWidgetItem* yaxis_root = new QTreeWidgetItem(ui->propGrid);
//    yaxis_root->setText(0, "y-Axis");
//    QTreeWidgetItem* yaxis_auto_scale = new QTreeWidgetItem;
//    yaxis_auto_scale->setText(0, "Auto scale");
//    yaxis_auto_scale->setWhatsThis(1, "y-Axis::AutoScroll");
//    yaxis_auto_scale->setFlags(yaxis_auto_scale->flags() | Qt::ItemIsUserCheckable);
//    yaxis_auto_scale->setCheckState(1, Qt::Checked);
//    yaxis_root->addChild(yaxis_auto_scale);
//    QTreeWidgetItem* yaxis_range = new QTreeWidgetItem;
//    yaxis_range->setText(0, "Range");
//    QTreeWidgetItem* yaxis_range_lb = new QTreeWidgetItem;
//    yaxis_range_lb->setText(0, "Begin (sec)");
//    yaxis_range_lb->setWhatsThis(1, "y-Axis::LBound");
//    yaxis_range_lb->setText(1, "-1.0");
//    yaxis_range_lb->setFlags(yaxis_range_lb->flags() | Qt::ItemIsEditable);
//    QTreeWidgetItem* yaxis_range_ub = new QTreeWidgetItem;
//    yaxis_range_ub->setText(0, "End (sec)");
//    yaxis_range_ub->setWhatsThis(1, "y-Axis::UBound");
//    yaxis_range_ub->setText(1, "1.0");
//    yaxis_range_ub->setFlags(yaxis_range_ub->flags() | Qt::ItemIsEditable);
//    yaxis_range->addChild(yaxis_range_lb);
//    yaxis_range->addChild(yaxis_range_ub);
//    yaxis_root->addChild(yaxis_range);
//    //ui->propGrid->setItemWidget(yaxis_range_lb, 1, new QSpinBox());
//    //ui->propGrid->setItemWidget(yaxis_range_ub, 1, new QSpinBox());


//    ui->propGrid->addTopLevelItem(xaxis_root);

//}

//void PlotConfig::on_propGrid_itemChanged(QTreeWidgetItem *item, int column)
//{
//    if (item->whatsThis(1) == "x-Axis::AutoScroll") {
//        qDebug() << item->whatsThis(1) << "=" << item->checkState(1);
//    }
//    else if (item->whatsThis(1) == "y-Axis::AutoScroll") {
//        qDebug() << item->whatsThis(1) << "=" << item->checkState(1);
//    }
//    else {
//        qDebug() << item->whatsThis(1) << "=" << item->text(1);
//    }
//}

