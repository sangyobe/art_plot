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
    item->setCheckable(true);
    item->setCheckState(Qt::Checked);
    item->setWhatsThis("Plot::Enabled");
    item->setData(QVariant::fromValue((void*)plotWnd));
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
    QSettings settings("HMC::ArtTeam", "artPlot");
    settings.setValue("geometry", saveGeometry());
    settings.setValue("state", saveState());
    QMainWindow::closeEvent(event);
}

void MainWindow::on_actionNew_triggered()
{
    emit newActionTriggered();
}

void MainWindow::on_actionExit_triggered()
{
    qApp->exit();
}

