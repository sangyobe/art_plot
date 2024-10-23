#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QVector>
#include <algorithm>

MainWindow::MainWindow(const std::string &ip, const uint16_t port, const int dnum, QWidget *parent) : QMainWindow(parent),
                                                                                                      ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    _plotListModel = new QStandardItemModel;
    _plotListModel->setColumnCount(1);

    QStandardItem *rt_plot_root = new QStandardItem("Realtime plots");
    QStandardItem *im_plot_root = new QStandardItem("Imported plots");
    rt_plot_root->setEditable(false);
    im_plot_root->setEditable(false);
    _plotListModel->appendRow(rt_plot_root);
    _plotListModel->appendRow(im_plot_root);

    ui->plotlistview->setModel(_plotListModel);

    // connect menu action
    connect(ui->actionNew, &QAction::triggered, this, &MainWindow::OnNewTriggered);
    connect(ui->actionClear, &QAction::triggered, this, &MainWindow::OnClearTriggered);
    connect(ui->actionAutoClear, &QAction::triggered, this, &MainWindow::OnAutoClearTriggered);
    connect(ui->actionLoad, &QAction::triggered, this, &MainWindow::OnLoadTriggered);
    connect(ui->actionExit, &QAction::triggered, this, &MainWindow::OnExitTriggered);

    bool connect_success;
    connect_success = connect(_plotListModel, SIGNAL(itemChanged(QStandardItem *)), this, SLOT(OnConfigChanged(QStandardItem *)));
    Q_ASSERT(connect_success);

    connect_success = connect(ui->plotlistview, SIGNAL(clicked(QModelIndex)), this, SLOT(OnItemClicked(QModelIndex)));
    Q_ASSERT(connect_success);

    // restore window geometry (size, position) & program options
    QSettings settings("hmc", "artPlot");
    restoreGeometry(settings.value("geometry").toByteArray());
    RestoreOption(settings.value("option").toByteArray());

    // initialize data server address
    _svrIpAddr = ip;
    _svrPort = port;
    if (_svrIpAddr == "") _svrIpAddr = "127.0.0.1";
    if (_svrPort == 0) _svrPort = 50051; // default grpc listening port
    ui->statusBar->showMessage(
        QString("server[%1:%2]")
            .arg(QString(_svrIpAddr.c_str()))
            .arg(_svrPort),
        0);

    // store params
    _debug_data_num = (dnum > 0 && dnum < DBGDATA_NUM_MAX ? dnum : DBGDATA_NUM_DFT);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::GetServerAddress(std::string &ip, uint16_t &port)
{
    ip = _svrIpAddr;
    port = _svrPort;
}

int MainWindow::GetDebugDataNum()
{
    return _debug_data_num;
}

bool MainWindow::IsAutoClear()
{
    return _autoClear;
}

QByteArray MainWindow::SaveOption() const
{
    QString configstr;
    QTextStream str(&configstr);
    str << "File::AutoClear"
        << "," << _autoClear;
    return configstr.toUtf8();
}

bool MainWindow::RestoreOption(const QByteArray &config)
{
    QString configstr = QString::fromUtf8(config);
    QList<QByteArray> items = config.split(',');
    int idx = 0;
    while (idx < (items.size() - 1))
    {
        if ("File::AutoClear" == items[idx])
        {
            _autoClear = items[idx + 1].toInt() == 0 ? false : true;
            ui->actionAutoClear->setChecked(_autoClear);
        }

        idx += 2;
    }

    return false;
}

void MainWindow::AddPlot(PlotWindow *plotWnd)
{
    _plotWnds.push_back(plotWnd);
    connect(plotWnd, SIGNAL(widgetHidden(QWidget *)), this, SLOT(OnPlotWndHidden(QWidget *)));
    connect(plotWnd, SIGNAL(widgetClosed(QWidget *)), this, SLOT(OnPlotWndClosed(QWidget *)));

    QStandardItem *item = new QStandardItem(plotWnd->GetWindowTitle());
    if (plotWnd->GetType() == PlotType::RT_PLOT)
    {
        item->setEditable(false);
        item->setCheckable(true);
        item->setCheckState(Qt::Checked);
    }
    item->setWhatsThis("Plot::Enabled");
    item->setData(QVariant::fromValue((void *)plotWnd));

    QString plot_group_name;
    if (PlotType::RT_PLOT == plotWnd->GetType())
        plot_group_name = "Realtime plots";
    else
        plot_group_name = "Imported plots";

    auto items = _plotListModel->findItems(plot_group_name, Qt::MatchExactly | Qt::MatchRecursive, 0);
    for (QStandardItem *plot_root : items)
    {
        if (plot_root->parent())
            continue;

        plot_root->appendRow(item);
        ui->plotlistview->expand(plot_root->index());

        break;
    }

    if (PlotType::RT_PLOT == plotWnd->GetType())
    {
        QSettings settings("hmc", "artPlot");
        RestorePlotConfig(settings.value("plotConfig").toByteArray(), plotWnd->GetWindowTitle());
    }
}

QByteArray MainWindow::SavePlotConfig() const
{
    QString configstr;
    QTextStream str(&configstr);

    // qDebug() << "SavePlotConfig";
    auto items = _plotListModel->findItems("Realtime plots", Qt::MatchExactly | Qt::MatchRecursive, 0);
    for (QStandardItem *plot_root : items)
    {
        if (plot_root->parent())
            continue;

        for (int chiIndex = 0; chiIndex < plot_root->rowCount(); chiIndex++)
        {
            QStandardItem *plot = plot_root->child(chiIndex);
            str << plot->data(Qt::DisplayRole).toString() << ",";
            str << (plot->checkState() == Qt::Checked ? 1 : 0) << ",";
        }
        break;
    }
    // qDebug() << configstr;
    return configstr.toUtf8();
}

bool MainWindow::RestorePlotConfig(const QByteArray &config, const QString &name)
{
    QString configstr = QString::fromUtf8(config);
    // qDebug() << "RestorePlotConfig: " << configstr;
    QList<QByteArray> items = config.split(',');
    int idx = 0;
    while (idx < (items.size() - 1))
    {
        if (name == items[idx])
        {
            SetPlotVisible(name, items[idx + 1].toInt() == 0 ? false : true);
            return true;
        }

        idx += 2;
    }
    return false;
}

void MainWindow::SetPlotVisible(const QString &name, bool visible)
{
    auto items = _plotListModel->findItems(name, Qt::MatchExactly | Qt::MatchRecursive, 0);
    foreach (const QStandardItem *citem, items)
    {
        if (citem->parent() && citem->parent()->text() == "Realtime plots")
        {
            const_cast<QStandardItem *>(citem)->setCheckState(visible ? Qt::Checked : Qt::Unchecked);
            return;
        }
    }
}

void MainWindow::OnPlotWndHidden(QWidget *widget)
{
    Q_UNUSED(widget);
}

void MainWindow::OnPlotWndClosed(QWidget *widget)
{
    for (int curIndex = 0; curIndex < _plotListModel->rowCount(); curIndex++)
    {
        QStandardItem *plot_group = _plotListModel->item(curIndex);
        for (int chiIndex = 0; chiIndex < plot_group->rowCount(); chiIndex++)
        {
            QStandardItem *plot = plot_group->child(chiIndex);
            if (plot && plot->data() == QVariant::fromValue((void *)widget))
            {
                PlotWindow *plotWnd = (PlotWindow *)plot->data().value<void *>();
                if (plotWnd->GetType() == PlotType::IM_PLOT)
                {
                    plot_group->removeRow(plot->row());
                    _plotWnds.removeOne(plotWnd);
                }
                else
                {
                    plot->setCheckState(Qt::Unchecked);
                }
            }
        }
    }
}

void MainWindow::OnConfigChanged(QStandardItem *item)
{
    // qDebug() << "MainWindow::OnConfigChanged(" << item->whatsThis() << ")";
    if (item->whatsThis() == "Plot::Enabled")
    {
        PlotWindow *plotWnd = (PlotWindow *)item->data().value<void *>();
        if (plotWnd)
        {
            plotWnd->SetWindowTitle(item->data(Qt::DisplayRole).toString());
            if (item->isCheckable())
            {
                plotWnd->setVisible((item->checkState() == Qt::Checked ? true : false));
                if (item->checkState() == Qt::Checked)
                    plotWnd->activateWindow();
            }
        }
    }
}

void MainWindow::OnItemClicked(QModelIndex index)
{
    // qDebug() << "MainWindow::OnItemClicked(" << index << ")";
    PlotWindow *plotWnd = (PlotWindow *)index.data(Qt::UserRole + 1).value<void *>();
    if (plotWnd)
        plotWnd->activateWindow();
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    // save app config
    QSettings settings("hmc", "artPlot");
    settings.setValue("geometry", saveGeometry());
    settings.setValue("state", saveState());
    settings.setValue("plotConfig", SavePlotConfig());
    settings.setValue("option", SaveOption());

    // close all plotWindows
    for (auto plotwnd : const_cast<const QList<PlotWindow*>& >(_plotWnds))
    {
        const_cast<PlotWindow *>(plotwnd)->close();
    }
    // QApplication::closeAllWindows();

    // close itself
    QMainWindow::closeEvent(event);
}

void MainWindow::OnNewTriggered()
{
    PlotWindow *plotWnd;
    plotWnd = new PlotWindow(this, PlotType::IM_PLOT);
    plotWnd->setAttribute(Qt::WA_DeleteOnClose, true);
    plotWnd->SetWindowTitle("New Plot");
    plotWnd->AutoScale(false);
    plotWnd->AutoScroll(false);
    plotWnd->AutoScrollWindow(5.0);
    plotWnd->show();
    AddPlot(plotWnd);
}

void MainWindow::OnClearTriggered()
{
    emit clearActionTriggered();
}

void MainWindow::OnAutoClearTriggered()
{
    _autoClear = ui->actionAutoClear->isChecked();
}

void MainWindow::OnLoadTriggered()
{
    QString filename = QFileDialog::getOpenFileName(this, "Select ControlState MCAP file to load LeoQuad control state from...", QDir::currentPath(), tr("MCAP Files (*.mcap)")); //;;All Files (*.*)"));
    if (filename.isEmpty())
        return;

    // qDebug() << "load file: " << filename;
    emit loadActionTriggered(filename);
}

void MainWindow::OnExitTriggered()
{
    qApp->exit();
}
