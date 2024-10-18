#include "plotconfig.h"
#include "ui_plotconfig.h"
#include "itemviewdelegate.h"
#include <QStandardItem>
#include <QSpinBox>
#include <QColorDialog>
#include <QMenu>
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
    connect(ui->configview, SIGNAL(doubleClicked(const QModelIndex&)), this, SLOT(OnItemDoubleClicked(const QModelIndex&)));

    // modify context menu
    ui->configview->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->configview, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(OnContextMenu(const QPoint&)));
}

PlotConfig::~PlotConfig()
{
    delete ui;
}

void PlotConfig::setConfigModel(QAbstractItemModel *configModel)
{
    ui->configview->setModel(configModel);
    ui->configview->setColumnWidth(0, 200);
    ui->configview->expandAll();
}

void PlotConfig::expandAll()
{
    ui->configview->expandAll();
}

void PlotConfig::expand(const QModelIndex& index, int depth)
{
    // ui->configview->expandRecursively(index, depth);
    ui->configview->expand(index);
}

void PlotConfig::collapseAll()
{
    ui->configview->collapseAll();
}

void PlotConfig::collapse(const QModelIndex& index)
{
    ui->configview->collapse(index);
}

void PlotConfig::selectItem(const QModelIndex& index)
{
    ui->configview->selectionModel()->select(index, QItemSelectionModel::Select);
}

void PlotConfig::clearSelection()
{
    ui->configview->selectionModel()->clearSelection();
}

void PlotConfig::OnItemClicked(const QModelIndex& index)
{
    // qDebug() << "PlotConfig::OnItemClicked";

    if ("Data series::Visible" == index.data(Qt::WhatsThisRole).toString()) 
    {
        emit graphItemClicked(index.data(Qt::DisplayRole).toString(), -1/*index.row()*/);
    }
    else if ("Data series::Color" == index.data(Qt::WhatsThisRole).toString()) 
    {
        QStandardItem* item_title = (QStandardItem*)(index.data(Qt::UserRole + 3).value<void*>());
        emit graphItemClicked(item_title->data(Qt::DisplayRole).toString(), -1/*index.row()*/);
    }
}

void PlotConfig::OnItemDoubleClicked(const QModelIndex& index)
{
    // qDebug() << "PlotConfig::OnItemDoubleClicked";

    if ("Data series::Color" == index.data(Qt::WhatsThisRole).toString()) 
    {
        QColorDialog colorPicker;
        colorPicker.setWindowTitle("Select graph line color");
        colorPicker.setOption(QColorDialog::ShowAlphaChannel, false);
        colorPicker.setModal(true);
        QColor selectedColor = colorPicker.getColor();
        if (selectedColor.isValid())
        {
            // qDebug() << "selected color = " << selectedColor.red() << "," << selectedColor.green() << "," << selectedColor.blue();
            QStandardItem* item_title = (QStandardItem*)(index.data(Qt::UserRole + 3).value<void*>());
            emit graphColorSelected(item_title->data(Qt::DisplayRole).toString(), index.row(), selectedColor);
        }
    }
}

void PlotConfig::OnContextMenu(const QPoint& pos)
{
    // qDebug() << "PlotConfig::OnContextMenu";

    QModelIndexList selected = ui->configview->selectionModel()->selectedIndexes();
    if (selected.size() > 0)
    {
        QModelIndex index = selected.at(0);
        // qDebug() << "selected index : " << index.row() << " , " << index.column();

        if ("Data series::Visible" == index.data(Qt::WhatsThisRole).toString()) 
        {
            QMenu* menu = new QMenu(this);

            if (index.data(Qt::DisplayRole).toString() != index.data(Qt::UserRole + 2).toString())
            {
                QAction* action_restore_name = menu->addAction("Restore data name");
                connect(action_restore_name, SIGNAL(triggered()), this, SLOT(RestoreName()));
            }
            QAction* action_restore_name_all = menu->addAction("Restore data name (All)");
            connect(action_restore_name_all, SIGNAL(triggered()), this, SLOT(RestoreNameAll()));
            // -------------------------------
            menu->addSeparator();
            // -------------------------------
            QAction* action_data_info = menu->addAction("Info...");
            connect(action_data_info, SIGNAL(triggered()), this, SLOT(ShowDataInfo()));

            menu->popup(ui->configview->viewport()->mapToGlobal(pos));
        }
    }
}

void PlotConfig::RestoreName()
{
    // qDebug() << "PlotConfig::RestoreName";

    QModelIndexList selected = ui->configview->selectionModel()->selectedIndexes();
    if (selected.size() > 0)
    {
        QModelIndex index = selected.at(0);
        // qDebug() << "selected index : " << index.row() << " , " << index.column();

        emit graphRestoreNameActionSelected(index.data(Qt::DisplayRole).toString(), index.row());
    }
}

void PlotConfig::RestoreNameAll()
{
    // qDebug() << "PlotConfig::RestoreNameAll";

    emit graphRestoreNameActionSelected("__all__", -1);
}

void PlotConfig::ShowDataInfo()
{
    QModelIndexList selected = ui->configview->selectionModel()->selectedIndexes();
    if (selected.size() > 0)
    {
        QModelIndex index = selected.at(0);
        emit graphShowDataInfoActionSelected(index.data(Qt::DisplayRole).toString(), index.row());
    }
}