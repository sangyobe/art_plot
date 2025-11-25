#include "mainwindow.h"
#include "preferencesdlg.h"
#include "ui_mainwindow.h"
#include "ArbitraryStateDataHandler.h"
#include "DualArmDataHandler.h"
#include "LeoQuadDataHandler.h"
#include "LeoQuadWheelDataHandler.h"
#include "MotorAgingDataHandler.h"
#include "G1DataHandler.h"
#include "QuadIPDataHandler.h"
#include "WolyDataHandler.h"
#include "FtSensorDataHandler.h"
#include <QDebug>
#include <QVector>
#include <algorithm> // For std::transform
#include <cctype> // For std::tolower

MainWindow::MainWindow(const std::string &ip, const uint16_t port, const int dnum, QWidget *parent) 
: QMainWindow(parent)
, ui(new Ui::MainWindow)
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

    ui->actionAutoClear->setChecked(_autoClear);
    ui->actionFetchInvisibleData->setChecked(_fetchInvisibleData);

    // connect menu action
    connect(ui->actionNew, &QAction::triggered, this, &MainWindow::OnNewTriggered);
    connect(ui->actionClear, &QAction::triggered, this, &MainWindow::OnClearTriggered);
    connect(ui->actionLoad, &QAction::triggered, this, &MainWindow::OnLoadTriggered);
    connect(ui->actionExit, &QAction::triggered, this, &MainWindow::OnExitTriggered);
    connect(ui->actionPreferences, &QAction::triggered, this, &MainWindow::OnPreferencesTriggered);
    connect(ui->actionAutoClear, &QAction::triggered, this, &MainWindow::OnAutoClearTriggered);
    connect(ui->actionFetchInvisibleData, &QAction::triggered, this, &MainWindow::OnFetchInvisibleDataTriggered);

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
    _debug_data_xy_num = 16; //
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

int MainWindow::GetDebugDataXYNum()
{
    return _debug_data_xy_num;
}

bool MainWindow::IsAutoClear()
{
    return _autoClear;
}

bool MainWindow::IsFetchInvisibleData()
{
    return _fetchInvisibleData;
}

QByteArray MainWindow::SaveOption() const
{
    QString configstr;
    QTextStream str(&configstr);
    str << "File::AutoClear"
        << "," << _autoClear
        << ","
        << "File::FetchInvisibleData"
        << "," << _fetchInvisibleData;
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
        else if ("File::FetchInvisibleData" == items[idx])
        {
            _fetchInvisibleData = items[idx + 1].toInt() == 0 ? false : true;
            ui->actionFetchInvisibleData->setChecked(_fetchInvisibleData);
        }

        idx += 2;
    }

    return false;
}

bool MainWindow::CreatePlotWindows(const std::string &profile_name)
{    
#ifdef USE_TRANSPORT_ECAL
    eCAL::Initialize(0, nullptr, "art_plot::QuadIP::ControlStatus");
    eCAL::Process::SetState(proc_sev_healthy, proc_sev_level1, "proc info");
#endif
#ifdef USE_TRANSPORT_GRPC
#endif

    std::function<std::string(const std::string&)> to_lower = [](const std::string& s) -> std::string {
        std::string lower_s = s;
        std::transform(lower_s.begin(), lower_s.end(), lower_s.begin(),
                       [](unsigned char c){ return std::tolower(c); });
        return lower_s;
    };
    std::string profile_lname = to_lower(profile_name);

    if (profile_lname == "leoquad" ||
        profile_lname == "lq")
    {
        _dataHandler = std::make_unique<LeoQuadDataHandler>(this);
    }
    else if (profile_lname == "leoquad_wheel" || 
             profile_lname == "leoquad-wheel" ||
             profile_lname == "leoquad_w" || 
             profile_lname == "leoquad-w" ||
             profile_lname == "lqw")
    {
        _dataHandler = std::make_unique<LeoQuadWheelDataHandler>(this);
    }
    else if (profile_lname == "dualarm" || 
             profile_lname == "da")
    {
        _dataHandler = std::make_unique<DualArmDataHandler>(this);
    }
    else if (profile_lname == "motoraging" || 
             profile_lname == "ma")
    {
        _dataHandler = std::make_unique<MotorAgingDataHandler>(this);
    }
    else if (profile_lname == "quadip" || 
             profile_lname == "quad_ip" ||
             profile_lname == "quad-ip" ||
             profile_lname == "qi")
    {
        _dataHandler = std::make_unique<QuadIPDataHandler>(this);
    }
    else if (profile_lname == "woly" || 
             profile_lname == "wl")
    {
        _dataHandler = std::make_unique<WolyDataHandler>(this);
    }
    else if (profile_lname == "g1" ||
             profile_lname == "G1")
    {
        _dataHandler = std::make_unique<G1DataHandler>(this);
    }
    else if (profile_lname == "ft" ||
             profile_lname == "Ft" || 
             profile_lname == "FT")
    {
        _dataHandler = std::make_unique<FtSensorDataHandler>(this);
    }
    else
    {
        qDebug() << "Invalid data profile name.";
        return false;
    }

    QString app_name = QString("%1 v%2 - %3").arg(APP_NAME).arg(ARTPLOT_VERSION_STR).arg(_dataHandler->GetProfileName().c_str());
    setWindowTitle(app_name);

    return true;
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

    // qDebug() << "SavePlotConfig called";
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
    // qDebug() << "SavePlotConfig result:" << configstr;
    return configstr.toUtf8();
}

bool MainWindow::RestorePlotConfig(const QByteArray &config, const QString &name)
{
    QString configstr = QString::fromUtf8(config);
    // qDebug() << "RestorePlotConfig for plot:" << name << "config:" << configstr;
    QList<QByteArray> items = config.split(',');
    int idx = 0;
    while (idx < (items.size() - 1))
    {
        QString itemName = QString::fromUtf8(items[idx]);
        int checkState = items[idx + 1].toInt();
        // qDebug() << "Checking config item:" << itemName << "vs plot name:" << name << "state:" << checkState;
        if (name == itemName)
        {
            // qDebug() << "Found match! Setting plot visible:" << (checkState != 0);
            SetPlotVisible(name, checkState == 0 ? false : true);
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
            QStandardItem *item = const_cast<QStandardItem *>(citem);
            item->setCheckState(visible ? Qt::Checked : Qt::Unchecked);
            
            // Also set the actual plot window visibility
            PlotWindow *plotWnd = (PlotWindow *)item->data().value<void *>();
            if (plotWnd)
            {
                plotWnd->setVisible(visible);
                if (visible)
                    plotWnd->activateWindow();
            }
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

void MainWindow::OnFetchInvisibleDataTriggered()
{
    _fetchInvisibleData = ui->actionFetchInvisibleData->isChecked();
}

void MainWindow::OnPreferencesTriggered()
{
    PreferencesDlg *dlg = new PreferencesDlg(this);
    dlg->setModal(true);
    dlg->show();
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
