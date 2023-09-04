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

    QStandardItem* item = new QStandardItem(plotWnd->GetWindowTitle());
    item->setEditable(false);
    item->setCheckable(true);
    item->setCheckState(Qt::Checked);
    item->setWhatsThis("Plot::Enabled");
    item->setData(QVariant::fromValue((void*)plotWnd));

//    auto items = _plotListModel->findItems("Realtime plots", Qt::MatchExactly | Qt::MatchRecursive, 0);
//    for (QStandardItem* rt_plot_root : items) {
//        if (rt_plot_root->parent())
//            continue;

//        rt_plot_root->appendRow(item);
//        break;
//    }
    _plotListModel->appendRow(item);
}

void MainWindow::OnPlotWndHidden(QWidget* widget)
{
    QList<QStandardItem*> items = _plotListModel->findItems(((PlotWindow*)widget)->GetWindowTitle());
    std::for_each(items.begin(), items.end(), [](QStandardItem* item) {
        item->setCheckState(Qt::Unchecked);
    });
}

void MainWindow::OnConfigChanged(QStandardItem *item)
{
    //qDebug() << "MainWindow::OnConfigChanged(" << item->whatsThis() << ")";

    if (item->whatsThis() == "Plot::Enabled") {
        PlotWindow *plotWnd = (PlotWindow*)item->data().value<void*>();
        if (plotWnd) {
            plotWnd->SetWindowTitle(item->data(Qt::DisplayRole).toString());
            plotWnd->setVisible((item->checkState() == Qt::Checked ? true : false));
            if (item->checkState() == Qt::Checked)
                plotWnd->activateWindow();
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

void MainWindow::on_actionNew_triggered()
{
    PlotWindow* plotWnd;
    plotWnd = new PlotWindow(this);
    plotWnd->SetWindowTitle("New Plot");
    plotWnd->AutoScale(false);
    plotWnd->AutoScroll(false);
    plotWnd->AutoScrollWindow(5.0);
    plotWnd->show();
    AddPlot(plotWnd);
}

void MainWindow::on_actionExit_triggered()
{
    qApp->exit();
}


void MainWindow::on_actionClear_triggered()
{
    emit clearActionTriggered();
}

