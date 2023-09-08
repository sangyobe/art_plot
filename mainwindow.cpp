#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QVector>
#include <QDebug>
#include <algorithm>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    _plotListModel = new QStandardItemModel;
    _plotListModel->setColumnCount(1);

    QStandardItem* rt_plot_root = new QStandardItem("Realtime plots");
    QStandardItem* im_plot_root = new QStandardItem("Imported plots");
    _plotListModel->appendRow(rt_plot_root);
    _plotListModel->appendRow(im_plot_root);

    ui->plotlistview->setModel(_plotListModel);

    // connect menu action
    connect(ui->actionNew, &QAction::triggered, this, &MainWindow::OnNewTriggered);
    connect(ui->actionClear, &QAction::triggered, this, &MainWindow::OnClearTriggered);
    connect(ui->actionExit, &QAction::triggered, this, &MainWindow::OnExitTriggered);

    bool connect_success;
    connect_success = connect(_plotListModel, SIGNAL(itemChanged(QStandardItem*)), this, SLOT(OnConfigChanged(QStandardItem*)));
    Q_ASSERT(connect_success);

    connect_success = connect(ui->plotlistview, SIGNAL(clicked(QModelIndex)), this, SLOT(OnItemClicked(QModelIndex)));
    Q_ASSERT(connect_success);


    // restore window geometry (size, position)
    QSettings settings("HMC::ArtTeam", "artPlot");
    restoreGeometry(settings.value("geometry").toByteArray());
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::AddPlot(PlotWindow *plotWnd)
{
    _plotWnds.push_back(plotWnd);
    connect(plotWnd, SIGNAL(widgetHidden(QWidget*)), this, SLOT(OnPlotWndHidden(QWidget*)));
    connect(plotWnd, SIGNAL(widgetClosed(QWidget*)), this, SLOT(OnPlotWndClosed(QWidget*)));

    QStandardItem* item = new QStandardItem(plotWnd->GetWindowTitle());
    if (plotWnd->GetType() == PlotType::RT_PLOT) {
        item->setEditable(false);
        item->setCheckable(true);
        item->setCheckState(Qt::Checked);
    }
    item->setWhatsThis("Plot::Enabled");
    item->setData(QVariant::fromValue((void*)plotWnd));


    QString plot_group_name;
    if (PlotType::RT_PLOT == plotWnd->GetType())
        plot_group_name = "Realtime plots";
    else
        plot_group_name = "Imported plots";

    auto items = _plotListModel->findItems(plot_group_name, Qt::MatchExactly | Qt::MatchRecursive, 0);
    for (QStandardItem* plot_root : items) {
        if (plot_root->parent())
            continue;

        plot_root->appendRow(item);
        ui->plotlistview->expand(plot_root->index());

        break;
    }
}

void MainWindow::OnPlotWndHidden(QWidget* widget)
{
    Q_UNUSED(widget);
}

void MainWindow::OnPlotWndClosed(QWidget* widget)
{
    for (int curIndex = 0; curIndex < _plotListModel->rowCount(); curIndex++) {
        QStandardItem* plot_group = _plotListModel->item(curIndex);
        for (int chiIndex = 0; chiIndex < plot_group->rowCount(); chiIndex++) {
            QStandardItem* plot = plot_group->child(chiIndex);
            if (plot && plot->data() == QVariant::fromValue((void*)widget)) {
                PlotWindow *plotWnd = (PlotWindow*)plot->data().value<void*>();
                if (plotWnd->GetType() == PlotType::IM_PLOT) {
                    plot_group->removeRow(plot->row());
                    _plotWnds.removeOne(plotWnd);
                }
                else {
                    plot->setCheckState(Qt::Unchecked);
                }
            }
        }
    }
}

void MainWindow::OnConfigChanged(QStandardItem *item)
{
    //qDebug() << "MainWindow::OnConfigChanged(" << item->whatsThis() << ")";

    if (item->whatsThis() == "Plot::Enabled") {
        PlotWindow *plotWnd = (PlotWindow*)item->data().value<void*>();
        if (plotWnd) {
            plotWnd->SetWindowTitle(item->data(Qt::DisplayRole).toString());
            if (item->isCheckable()) {
                plotWnd->setVisible((item->checkState() == Qt::Checked ? true : false));
                if (item->checkState() == Qt::Checked)
                    plotWnd->activateWindow();
            }
        }
    }
}

void MainWindow::OnItemClicked(QModelIndex index)
{
    //qDebug() << "MainWindow::OnItemClicked(" << index << ")";
    PlotWindow *plotWnd = (PlotWindow*)index.data(Qt::UserRole + 1).value<void*>();
    if (plotWnd)
        plotWnd->activateWindow();
}

void MainWindow::resizeEvent(QResizeEvent* event)
{
    QMainWindow::resizeEvent(event);
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    // close all plotWindows
    for (auto plotwnd : qAsConst(_plotWnds)) {
        const_cast<PlotWindow*>(plotwnd)->close();
    }
    //QApplication::closeAllWindows();

    // close itself
    QSettings settings("HMC::ArtTeam", "artPlot");
    settings.setValue("geometry", saveGeometry());
    settings.setValue("state", saveState());
    QMainWindow::closeEvent(event);
}

void MainWindow::OnNewTriggered()
{
    PlotWindow* plotWnd;
    plotWnd = new PlotWindow(this, PlotType::IM_PLOT);
    plotWnd->setAttribute(Qt::WA_DeleteOnClose, true);
    plotWnd->SetWindowTitle("New Plot");
    plotWnd->AutoScale(false);
    plotWnd->AutoScroll(false);
    plotWnd->AutoScrollWindow(5.0);
    plotWnd->show();
    AddPlot(plotWnd);
}

void MainWindow::OnExitTriggered()
{
    qApp->exit();
}

void MainWindow::OnClearTriggered()
{
    emit clearActionTriggered();
}

