#include "plotwindow.h"
#include "itemviewdelegate.h"
#include "pconstants.h"
#include "plotconfig.h"
#include "datainfodlg.h"
#include "ui_plotwindow.h"
#include <cassert>
#include <iostream>
#include <memory>

#define REFRESH_INTERVAL_FAST (100)  // (ms)
#define REFRESH_INTERVAL_NORM (500)  // (ms)
#define REFRESH_INTERVAL_SLOW (1000) // (ms)

PlotWindow::ConfigOption::ConfigOption() : x_axis_auto_scroll(true),
                                           x_axis_auto_scroll_window(10.0),
                                           x_axis_begin_sec(0.0),
                                           x_axis_end_sec(1.0),
                                           y_axis_auto_scale(true),
                                           y_axis_lbound(-1.0),
                                           y_axis_ubound(1.0),
                                           legend_visible(true),
                                           legend_location(Qt::AlignRight | Qt::AlignTop),
                                           style_line_width(1)
{
}

PlotWindow::PlotWindow(QWidget *parent, PlotType type) : QMainWindow(parent),
                                                         ui(new Ui::PlotWindow),
                                                         _plotType(type),
                                                         _configModel(nullptr)
{
    ui->setupUi(this);

    //------------------------------------------------------------------
    // Config menu
    //
    if (_plotType == PlotType::RT_PLOT)
    {
        ui->actionImport->setDisabled(true);
        ui->actionAppend->setDisabled(true);
        ui->actionReset->setDisabled(true);
    }
    else if (_plotType == PlotType::IM_PLOT)
    {
        // ui->actionExport->setDisabled(true);
#ifdef USE_EMUL_DATA
        ui->actionImport->setDisabled(true);
        ui->actionReset->setDisabled(true);
#endif
    }
    connect(ui->actionClear, &QAction::triggered, this, &PlotWindow::OnClearTriggered);
    connect(ui->actionReset, &QAction::triggered, this, &PlotWindow::OnResetTriggered);
    connect(ui->actionImport, &QAction::triggered, this, &PlotWindow::OnImportTriggered);
    connect(ui->actionAppend, &QAction::triggered, this, &PlotWindow::OnAppendTriggered);
    connect(ui->actionExport, &QAction::triggered, this, &PlotWindow::OnExportTriggered);
    connect(ui->actionClose, &QAction::triggered, this, &PlotWindow::OnCloseTriggered);
    connect(ui->actionExtend_All, &QAction::triggered, this, &PlotWindow::OnExtendAllTriggered);
    connect(ui->actionRefreshRateFast, &QAction::triggered, this, &PlotWindow::OnRefreshRateFastTriggered);
    connect(ui->actionRefreshRateNormal, &QAction::triggered, this, &PlotWindow::OnRefreshRateNormalTriggered);
    connect(ui->actionRefreshRateSlow, &QAction::triggered, this, &PlotWindow::OnRefreshRateSlowTriggered);

    //------------------------------------------------------------------
    // Creat and set-up plot configuration(properties) panel
    //
    _plotConfig = new PlotConfig(this);
    _plotConfig->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    addDockWidget(Qt::LeftDockWidgetArea, _plotConfig);
    connect(_plotConfig, SIGNAL(graphItemClicked(QString, int)), this, SLOT(OnConfigItemGraphClicked(QString, int)));
    connect(_plotConfig, SIGNAL(graphColorSelected(QString, int, QColor)), this, SLOT(OnConfigItemGraphColorSelected(QString, int, QColor)));
    connect(_plotConfig, SIGNAL(graphRestoreNameActionSelected(QString, int)), this, SLOT(OnConfigItemGraphRestoreNameActionSelected(QString, int)));
    connect(_plotConfig, SIGNAL(graphShowDataInfoActionSelected(QString, int)), this, SLOT(OnConfigItemGraphShowDataInfoActionSelected(QString, int)));

    BuildConfig();

    //------------------------------------------------------------------
    // Config plot window
    //
    ui->plotwidget->setPlottingHints(QCP::phNone | QCP::phFastPolylines | QCP::phImmediateRefresh
                                     //  | QCP::phCacheLabels
    );

    // legend
    ui->plotwidget->legend->setVisible(_configOption.legend_visible);
    QFont legendFont = font();
    legendFont.setPointSize(9);
    ui->plotwidget->legend->setFont(legendFont);
    ui->plotwidget->legend->setBrush(QBrush(QColor(255, 255, 255, 230)));

    // axis
    ui->plotwidget->xAxis->setLabel("x");
    ui->plotwidget->yAxis->setLabel("y");
    ui->plotwidget->xAxis->setNumberFormat("gbc");
    // ui->plotwidget->xAxis->setNumberPrecision(16);
    // configure right and top axis to show ticks but no labels:
    // (see QCPAxisRect::setupFullAxesBox for a quicker method to do this)
    ui->plotwidget->xAxis2->setVisible(true);
    ui->plotwidget->xAxis2->setTickLabels(false);
    ui->plotwidget->yAxis2->setVisible(true);
    ui->plotwidget->yAxis2->setTickLabels(false);

    //
    // set user interaction options
    //
    // enable dragging using mouse
    ui->plotwidget->setInteraction(QCP::iRangeDrag, true);
    // sellect enabled dragging direction
    ui->plotwidget->axisRect(0)->setRangeDrag(Qt::Vertical | Qt::Horizontal);

    // set initial dragging mode
    ui->plotwidget->setSelectionRectMode(QCP::srmNone);
    connect(ui->plotwidget, SIGNAL(mousePress(QMouseEvent *)), this, SLOT(OnMousePressed(QMouseEvent *)));
    connect(ui->plotwidget, SIGNAL(mouseRelease(QMouseEvent *)), this, SLOT(OnMouseReleased(QMouseEvent *)));

    // enable zoom (by mouse wheel)
    ui->plotwidget->setInteraction(QCP::iRangeZoom, true);
    ui->plotwidget->axisRect(0)->setRangeZoom(Qt::Vertical | Qt::Horizontal);
    ui->plotwidget->axisRect(0)->setRangeZoomFactor(1.1, 1.1);

    // enable clicking items
    ui->plotwidget->setInteraction(QCP::iSelectPlottables);
    ui->plotwidget->setInteraction(QCP::iSelectLegend, true);
    ui->plotwidget->setInteraction(QCP::iMultiSelect, false);
    // ui->plotwidget->setMultiSelectModifier(Qt::ControlModifier);
    connect(ui->plotwidget, SIGNAL(selectionChangedByUser()), this, SLOT(OnSelectionChangedByUser()));

    // scroll-bars
    ui->horizontalScrollBar->setRange(0, 100000);
    ui->verticalScrollBar->setRange(-100000, 100000);
    connect(ui->horizontalScrollBar, SIGNAL(valueChanged(int)), this, SLOT(OnHorzScrollBarChanged(int)));
    connect(ui->verticalScrollBar, SIGNAL(valueChanged(int)), this, SLOT(OnVertScrollBarChanged(int)));
    connect(ui->plotwidget->xAxis, SIGNAL(rangeChanged(QCPRange)), this, SLOT(OnXAxisRangeChanged(QCPRange)));
    connect(ui->plotwidget->yAxis, SIGNAL(rangeChanged(QCPRange)), this, SLOT(OnYAxisRangeChanged(QCPRange)));

    ResetPlot();

    //------------------------------------------------------------------
    // initialize the plot refresh timer
    //
    _refreshPlotTimer = std::unique_ptr<QTimer>(new QTimer(this));
    connect(_refreshPlotTimer.get(), SIGNAL(timeout()), this, SLOT(OnRefreshPlot()));
    _refreshPlotTimer->setInterval(500); // plot refresh time interval in milli-seconds
    _refreshPlotTimer->start();

    //------------------------------------------------------------------
    // add refresh rate menu
    //
    QActionGroup *refreshRateActionGroup = new QActionGroup(this);
    refreshRateActionGroup->addAction(ui->actionRefreshRateFast);
    refreshRateActionGroup->addAction(ui->actionRefreshRateNormal);
    refreshRateActionGroup->addAction(ui->actionRefreshRateSlow);
    refreshRateActionGroup->setExclusive(true);
    ui->actionRefreshRateNormal->setChecked(true);
    ui->actionRefreshRateNormal->setData(QVariant::fromValue(REFRESH_INTERVAL_FAST));

    //------------------------------------------------------------------
    // add checkable menu option for showing or hiding plot config window
    //
    ui->menuView->addSeparator();
    ui->menuView->addAction(_plotConfig->toggleViewAction());

    //------------------------------------------------------------------
    // drag & drop CSV file
    //
    if (_plotType == PlotType::IM_PLOT)
        setAcceptDrops(true);

    //------------------------------------------------------------------
    // other initializations
    //
    connect(this->parent(), SIGNAL(clearActionTriggered()), this, SLOT(OnClearTriggered()));
    // connect(ui->plotwidget->yAxis, SIGNAL(rangeChanged(const QCPRange&, const QCPRange&)), this, SLOT(OnRangeChanged(const QCPRange&, const QCPRange&)));

#ifdef USE_EMUL_DATA
    this->AddGraph("Sin", LineColor<0>(), "", 1, LineScatterShape::ssCircle, 1000, false);
    this->AddGraph("Sin^2", LineColor<1>(), "", 1);
    this->AddGraph("Cos", LineColor<2>(), "", 2);
    this->AddGraph("Cos^2", LineColor<3>(), "", 2, LineScatterShape::ssTriangle, 500);
    this->SetGraphVisible("Cos^2", false);

    _dataSource = std::unique_ptr<DataSourceEmul>(new DataSourceEmul("Sample Data Series"));
    connect(_dataSource.get(), SIGNAL(new_data(const DataSeriesEmul &)), this, SLOT(OnRecvEmul(const DataSeriesEmul &)));
#endif
}

PlotWindow::~PlotWindow()
{
    _refreshPlotTimer->stop();

    delete ui;
}

PlotWidget *PlotWindow::plot() const
{
    return ui->plotwidget;
}

QCPGraph *PlotWindow::graph(int index) const
{
    return ui->plotwidget->graph(index);
}

int PlotWindow::AddGraph(const QString &name, const QColor &color, const QString group_name, int line_width, int scatter_shape, int scatter_skip, bool visible)
{
    QCPGraph *graph = ui->plotwidget->addGraph();
    graph->setName(name);
    // line style
    graph->setLineStyle((line_width == 0 ? QCPGraph::lsNone : QCPGraph::lsLine /*lsStepRight*/));
    QPen pen(color);
    pen.setWidth(line_width);
    graph->setPen(pen);
    QCPScatterStyle sstyle((QCPScatterStyle::ScatterShape)scatter_shape);
    graph->setScatterStyle(sstyle);
    graph->setScatterSkip(scatter_skip);
    graph->setAdaptiveSampling(true);
    graph->setSelectable(QCP::stWhole);
    graph->setVisible(visible);

    auto items = _configModel->findItems("Data series", Qt::MatchExactly | Qt::MatchRecursive, 0);
    QList<QStandardItem *> item_graph_visible;
    QStandardItem *item_title = nullptr;
    QStandardItem *item_option = nullptr;
    QStandardItem *group_title = nullptr;
    foreach (QStandardItem *data_series_root, items)
    {
        if (data_series_root->parent()) // Data series 가 최상위 노드가 아니면(데이터 이름일 수 있다) skip!
            continue;

        item_title = new QStandardItem(name);
        item_option = new QStandardItem();

        item_title->setEditable(true);
        item_title->setSelectable(true);
        item_title->setWhatsThis(QString("Data series::Visible"));
        item_title->setData(QVariant::fromValue((void *)graph));
        item_title->setData(QVariant::fromValue(name), Qt::UserRole + 2);
        item_title->setData(QVariant::fromValue((void *)item_option), Qt::UserRole + 3);
        item_title->setCheckable(true);
        item_title->setCheckState(visible ? Qt::Checked : Qt::Unchecked);

        item_option->setEditable(false);
        // item_option->setWhatsThis(QString("Data series::%s").arg(name));
        item_option->setWhatsThis(QString("Data series::Color"));
        item_option->setSelectable(false);
        item_option->setData(color, Qt::BackgroundRole);
        item_option->setData(QVariant::fromValue(name), Qt::UserRole + 2);
        item_option->setData(QVariant::fromValue((void *)item_title), Qt::UserRole + 3);

        item_graph_visible.append(item_title);
        item_graph_visible.append(item_option);

        // process grouping items
        if (!group_name.isEmpty())
        {
            group_title = const_cast<QStandardItem*>(FindFirstConfigOptionGroup("Data series", group_name, 0));
            if (!group_title)
            {
                group_title = new QStandardItem(group_name);
                group_title->setEditable(false);
                group_title->setSelectable(false);
                group_title->setWhatsThis(QString("Data series::Visible_Group"));
                group_title->setCheckable(true);
                group_title->setCheckState(Qt::Checked);

                data_series_root->appendRow(group_title);
            }
            group_title->appendRow(item_graph_visible);
            _plotConfig->expand(_configModel->indexFromItem(group_title));
        }
        else
        {
            data_series_root->appendRow(item_graph_visible);
        }
        
        _plotConfig->expand(_configModel->indexFromItem(data_series_root));
        break;
    }

    QSettings settings("hmc", "artPlot");
    QString alias;
    QColor bgcolor;
    RestoreDataSeriesConfig(settings.value(windowTitle() + "/dataSeriesConfig_v2").toByteArray(), name, alias, bgcolor);
    if (name != alias && !alias.isEmpty())
    {
        item_title->setText(alias);
        graph->setName(alias);
    }
    if (bgcolor.isValid())
    {
        item_option->setData(bgcolor, Qt::BackgroundRole);
        QPen bgpen(bgcolor);
        graph->setPen(bgpen);
    }

    return ui->plotwidget->graphCount();
}

int PlotWindow::GetGraphCount()
{
    return ui->plotwidget->graphCount();
}

void PlotWindow::AddData(int gid, double key, double value)
{
    QMutexLocker locker(&_graphDataGuard);
    ui->plotwidget->graph(gid)->addData(key, value);
}

void PlotWindow::SetGraphVisible(const QString &name, bool visible)
{
    const QStandardItem *item = FindFirstConfigOptionItem("Data series", name, 0, true);
    if (item)
    {
        const_cast<QStandardItem *>(item)->setCheckState(visible ? Qt::Checked : Qt::Unchecked);
    }
}

void PlotWindow::SelectGraph(const QString &name, int index)
{
    int foundItemIndex = -1;

    if (index >= 0 && index < ui->plotwidget->graphCount())
    {
        if (ui->plotwidget->graph(index)->name() == name)
            foundItemIndex = index;
    }
    else
    {
        for (int i = 0; i < ui->plotwidget->graphCount(); i++)
        {
            if (ui->plotwidget->graph(i)->name() == name)
            {
                foundItemIndex = i;
                break;
            }
        }
    }

    if (foundItemIndex >= 0)
    {
        ui->plotwidget->graph(foundItemIndex)->setSelectable(QCP::stWhole);
        QCPDataSelection selection;
        QCPDataRange range(0, 1);
        selection.addDataRange(range);
        ui->plotwidget->graph(foundItemIndex)->setSelection(selection);

        // select legend item
        ui->plotwidget->legend->item(foundItemIndex)->setSelected(true);
    }
}

void PlotWindow::DeselectAllGraphs()
{
    ui->plotwidget->deselectAll();
}

void PlotWindow::ResetPlot()
{
    ResetData();

    for (int g = ui->plotwidget->graphCount(); g > 0; g--)
    {
        ui->plotwidget->removeGraph(g - 1);
    }

    auto items = _configModel->findItems("Data series", Qt::MatchExactly | Qt::MatchRecursive, 0);
    foreach (QStandardItem *data_series_root, items)
    {
        if (data_series_root->parent())
            continue;

        data_series_root->removeRows(0, data_series_root->rowCount());
    }
}

void PlotWindow::ResetData()
{
    QMutexLocker locker(&_graphDataGuard);

    for (int g = 0; g < ui->plotwidget->graphCount(); g++)
    {
        ui->plotwidget->graph(g)->data().data()->clear();
    }

    _lastRecvTime = -1.0;
    _isNewDataReceived = false;

    ui->plotwidget->xAxis->setRange(
        _configOption.x_axis_begin_sec,
        _configOption.x_axis_end_sec);
    ui->plotwidget->yAxis->setRange(
        _configOption.y_axis_lbound,
        _configOption.y_axis_ubound);
    ui->plotwidget->replot();
}

void PlotWindow::Replot()
{
    QMutexLocker locker(&_graphDataGuard);
    ui->plotwidget->replot();
}

void PlotWindow::SetWindowTitle(const QString &title)
{
    setWindowTitle(title);
    ui->plotwidget->setWindowTitle(title);

    // restore window geometry (size, position)
    QSettings settings("hmc", "artPlot");
    restoreGeometry(settings.value(title + "/geometry").toByteArray());
    restoreState(settings.value(title + "/windowState").toByteArray());
    RestorePlotConfig(settings.value(title + "/plotConfig").toByteArray());
}

QString PlotWindow::GetWindowTitle() const
{
    return windowTitle();
}

void PlotWindow::AutoScroll(bool on)
{
    const QStandardItem *item = FindFirstConfigOptionItem("x-Axis", "Auto scroll", 1);
    if (!item)
        return;

    const_cast<QStandardItem *>(item)->setCheckState(on ? Qt::Checked : Qt::Unchecked);
}

void PlotWindow::AutoScrollWindow(double dt_sec)
{
    const QStandardItem *item = FindFirstConfigOptionItem("x-Axis", "Scroll Window(s)", 1);
    if (!item)
        return;

    const_cast<QStandardItem *>(item)->setData(dt_sec, Qt::EditRole);
}

void PlotWindow::SetXRange(double ti, double tf)
{
    SetXBegin(ti);
    SetXEnd(tf);
}

void PlotWindow::SetXBegin(double ti)
{
    QStandardItem const *item = FindFirstConfigOptionItem("x-Axis", "Begin(s)", 1);
    if (item)
    {
        const_cast<QStandardItem *>(item)->setData(ti, Qt::EditRole);
    }
}

void PlotWindow::SetXEnd(double tf)
{
    QStandardItem const *item = FindFirstConfigOptionItem("x-Axis", "End(s)", 1);
    if (item)
    {
        const_cast<QStandardItem *>(item)->setData(tf, Qt::EditRole);
    }
}

void PlotWindow::AutoScale(bool on)
{
    const QStandardItem *item = FindFirstConfigOptionItem("y-Axis", "Auto scale", 1);
    if (!item)
        return;

    const_cast<QStandardItem *>(item)->setCheckState(on ? Qt::Checked : Qt::Unchecked);
}

void PlotWindow::SetYRange(double lbound, double ubound)
{
    SetYLBound(lbound);
    SetYUBound(ubound);
}

void PlotWindow::SetYLBound(double lbound)
{
    QStandardItem const *item = FindFirstConfigOptionItem("y-Axis", "Lower bound", 1);
    if (item)
    {
        const_cast<QStandardItem *>(item)->setData(lbound, Qt::EditRole);
    }
}

void PlotWindow::SetYUBound(double ubound)
{
    QStandardItem const *item = FindFirstConfigOptionItem("y-Axis", "Upper bound", 1);
    if (item)
    {
        const_cast<QStandardItem *>(item)->setData(ubound, Qt::EditRole);
    }
}

void PlotWindow::ShowLegend(bool show)
{
    const QStandardItem *item = FindFirstConfigOptionItem("Legend", "Visible", 1);
    if (!item)
        return;

    const_cast<QStandardItem *>(item)->setCheckState(show ? Qt::Checked : Qt::Unchecked);
}

void PlotWindow::SetLegendLocation(QFlags<Qt::AlignmentFlag> flag)
{
    const QStandardItem *item = FindFirstConfigOptionItem("Legend", "Location", 1);
    if (!item)
        return;

    switch (flag)
    {
    case Qt::AlignLeft | Qt::AlignTop:
        const_cast<QStandardItem *>(item)->setData("Left-Top", Qt::EditRole);
        break;
    case Qt::AlignLeft | Qt::AlignVCenter:
        const_cast<QStandardItem *>(item)->setData("Left-Middle", Qt::EditRole);
        break;
    case Qt::AlignLeft | Qt::AlignBottom:
        const_cast<QStandardItem *>(item)->setData("Left-Bottom", Qt::EditRole);
        break;
    case Qt::AlignRight | Qt::AlignTop:
        const_cast<QStandardItem *>(item)->setData("Right-Top", Qt::EditRole);
        break;
    case Qt::AlignRight | Qt::AlignVCenter:
        const_cast<QStandardItem *>(item)->setData("Right-Middle", Qt::EditRole);
        break;
    case Qt::AlignRight | Qt::AlignBottom:
        const_cast<QStandardItem *>(item)->setData("Right-Bottom", Qt::EditRole);
        break;
    }
}

void PlotWindow::SetLineWidth(int w)
{
    QStandardItem const *item = FindFirstConfigOptionItem("Style", "Line width(1~5)");
    if (item)
    {
        const_cast<QStandardItem *>(item)->setData(w, Qt::EditRole);
    }
}

void PlotWindow::DataUpdated(double recv_time)
{
    _lastRecvTime = qMax(_lastRecvTime.load(), recv_time);
    _isNewDataReceived = true;
}

void PlotWindow::hideEvent(QHideEvent *event)
{
    QMainWindow::hideEvent(event);
    emit widgetHidden(this);
}

void PlotWindow::closeEvent(QCloseEvent *event)
{
    // save window geometry
    QSettings settings("hmc", "artPlot");
    settings.setValue(windowTitle() + "/geometry", saveGeometry());
    settings.setValue(windowTitle() + "/windowState", saveState());
    settings.setValue(windowTitle() + "/plotConfig", SavePlotConfig());
    settings.setValue(windowTitle() + "/dataSeriesConfig_v2", SaveDataSeriesConfig());
    QMainWindow::closeEvent(event);
    emit widgetClosed(this);
}

void PlotWindow::resizeEvent(QResizeEvent *event)
{
    // qDebug() << "Width : " << this->width() << ", Height : " <<  this->height();
    RecalculatePlotLayout();
    QMainWindow::resizeEvent(event);
}

void PlotWindow::keyPressEvent(QKeyEvent *event)
{
    switch (event->key())
    {
    case Qt::Key_Space:
        ExtendAll();
        return;
    default:
        QMainWindow::keyPressEvent(event);
    }
}

void PlotWindow::BuildConfig()
{
    QList<QStandardItem *> items;
    QStandardItem *item_title;
    QStandardItem *item;

    _configModel = new QStandardItemModel;
    _configModel->setColumnCount(2);

    QStringList header_labels;
    header_labels << "Property"
                  << "Value";
    _configModel->setHeaderData(0, Qt::Horizontal, header_labels[0]);
    _configModel->setHeaderData(1, Qt::Horizontal, header_labels[1]);

    // x-Axis
    QStandardItem *x_axis_root = new QStandardItem("x-Axis");
    x_axis_root->setEditable(false);
    _configModel->appendRow(x_axis_root);

    items.clear();
    item_title = new QStandardItem("Auto scroll");
    item_title->setEditable(false);
    item = new QStandardItem();
    item->setEditable(false);
    item->setCheckable(true);
    item->setCheckState(_configOption.x_axis_auto_scroll ? Qt::Checked : Qt::Unchecked);
    item->setWhatsThis("x-Axis::AutoScroll");
    items.append(item_title);
    items.append(item);
    x_axis_root->appendRow(items);

    items.clear();
    item_title = new QStandardItem("Scroll Window(s)");
    item_title->setEditable(false);
    item = new QStandardItem();
    item->setEditable(true);
    item->setData(_configOption.x_axis_auto_scroll_window, Qt::EditRole);
    item->setWhatsThis("x-Axis::AutoScrollWindow");
    items.append(item_title);
    items.append(item);
    x_axis_root->appendRow(items);

    items.clear();
    item_title = new QStandardItem("Begin(s)");
    item_title->setEditable(false);
    item = new QStandardItem();
    item->setEditable(true);
    item->setData(_configOption.x_axis_begin_sec, Qt::EditRole);
    item->setWhatsThis("x-Axis::Begin");
    item->setEnabled(!_configOption.x_axis_auto_scroll);
    items.append(item_title);
    items.append(item);
    x_axis_root->appendRow(items);

    items.clear();
    item_title = new QStandardItem("End(s)");
    item_title->setEditable(false);
    item = new QStandardItem();
    item->setEditable(true);
    item->setData(_configOption.x_axis_end_sec, Qt::EditRole);
    item->setWhatsThis("x-Axis::End");
    item->setEnabled(!_configOption.x_axis_auto_scroll);
    items.append(item_title);
    items.append(item);
    x_axis_root->appendRow(items);

    // y-Axis
    QStandardItem *y_axis_root = new QStandardItem("y-Axis");
    y_axis_root->setEditable(false);
    _configModel->appendRow(y_axis_root);

    items.clear();
    item_title = new QStandardItem("Auto scale");
    item_title->setEditable(false);
    item = new QStandardItem();
    item->setEditable(false);
    item->setCheckable(true);
    item->setCheckState(_configOption.y_axis_auto_scale ? Qt::Checked : Qt::Unchecked);
    item->setWhatsThis("y-Axis::AutoScale");
    items.append(item_title);
    items.append(item);
    y_axis_root->appendRow(items);

    items.clear();
    item_title = new QStandardItem("Lower bound");
    item_title->setEditable(false);
    item = new QStandardItem();
    item->setEditable(true);
    item->setData(_configOption.y_axis_lbound, Qt::EditRole);
    item->setWhatsThis("y-Axis::LBound");
    item->setEnabled(!_configOption.y_axis_auto_scale);
    items.append(item_title);
    items.append(item);
    y_axis_root->appendRow(items);

    items.clear();
    item_title = new QStandardItem("Upper bound");
    item_title->setEditable(false);
    item = new QStandardItem();
    item->setEditable(true);
    item->setData(_configOption.y_axis_ubound, Qt::EditRole);
    item->setWhatsThis("y-Axis::UBound");
    item->setEnabled(!_configOption.y_axis_auto_scale);
    items.append(item_title);
    items.append(item);
    y_axis_root->appendRow(items);

    // legend
    QStandardItem *legend_root = new QStandardItem("Legend");
    legend_root->setEditable(false);
    _configModel->appendRow(legend_root);

    items.clear();
    item_title = new QStandardItem("Visible");
    item_title->setEditable(false);
    item = new QStandardItem();
    item->setEditable(false);
    item->setCheckable(true);
    item->setCheckState(_configOption.legend_visible ? Qt::Checked : Qt::Unchecked);
    item->setWhatsThis("Legend::Visible");
    items.append(item_title);
    items.append(item);
    legend_root->appendRow(items);

    items.clear();
    item_title = new QStandardItem("Location");
    item_title->setEditable(false);
    QStringList loc_options = {"Left-Top", "Left-Middle", "Left-Bottom", "Right-Top", "Right-Middle", "Right-Bottom"};
    item = new QStandardItem(loc_options[3]);
    item->setEditable(true);
    item->setWhatsThis("Legend::Location");
    item->setData(QVariant(loc_options), static_cast<int>(ItemViewDelegate::Role::ComboBox));
    items.append(item_title);
    items.append(item);
    legend_root->appendRow(items);

    // style
    QStandardItem *style_root = new QStandardItem("Style");
    style_root->setEditable(false);
    _configModel->appendRow(style_root);

    items.clear();
    item_title = new QStandardItem("Line width(1~5)");
    item_title->setEditable(false);
    item = new QStandardItem();
    item->setEditable(true);
    item->setData(_configOption.style_line_width, Qt::EditRole);
    item->setWhatsThis("Style::LineWidth");
    items.append(item_title);
    items.append(item);
    style_root->appendRow(items);

    _plotConfig->setConfigModel(_configModel);

    // data series
    // QStandardItem* data_series_root = new QStandardItem("Data series");
    QList<QStandardItem *> data_series_root;

    // data_series_root->setEditable(false);
    QStandardItem *data_series_root_title = new QStandardItem("Data series");
    QStandardItem *data_series_root_option = new QStandardItem();
    data_series_root_title->setEditable(false);
    data_series_root_title->setCheckable(true);
    data_series_root_title->setCheckState(Qt::Unchecked);
    data_series_root_title->setWhatsThis(QString("Data series::Visible_All"));
    data_series_root_option->setEditable(false);
    data_series_root_option->setSelectable(false);
    data_series_root.append(data_series_root_title);
    data_series_root.append(data_series_root_option);
    _configModel->appendRow(data_series_root);

    // connect signal
    connect(_configModel, SIGNAL(itemChanged(QStandardItem *)), this, SLOT(OnConfigChanged(QStandardItem *)));
}

void PlotWindow::OnConfigChanged(QStandardItem *item)
{
    // qDebug() << "PlotWindow::OnConfigChanged(" << item->whatsThis() << ")";

    if (item->whatsThis() == "x-Axis::AutoScroll")
    {
        _configOption.x_axis_auto_scroll = (item->checkState() == Qt::Checked ? true : false);

        const QStandardItem *item = nullptr;
        if (_configOption.x_axis_auto_scroll)
        {
            item = FindFirstConfigOptionItem("x-Axis", "Begin(s)", 1);
            if (item)
            {
                const_cast<QStandardItem *>(item)->setEnabled(false);
            }
            item = FindFirstConfigOptionItem("x-Axis", "End(s)", 1);
            if (item)
            {
                const_cast<QStandardItem *>(item)->setEnabled(false);
            }

            // AutoScroll on 시 여기서는 플래그만 셋하고, 실제 새로운 데이터 수신시에 그래프를 새로 그린다.
        }
        else
        {
            double ti = ui->plotwidget->xAxis->range().lower;
            double tf = ui->plotwidget->xAxis->range().upper;

            item = FindFirstConfigOptionItem("x-Axis", "Begin(s)", 1);
            if (item)
            {
                const_cast<QStandardItem *>(item)->setEnabled(true);
                const_cast<QStandardItem *>(item)->setData(ti, Qt::EditRole);
            }
            item = FindFirstConfigOptionItem("x-Axis", "End(s)", 1);
            if (item)
            {
                const_cast<QStandardItem *>(item)->setEnabled(true);
                const_cast<QStandardItem *>(item)->setData(tf, Qt::EditRole);
            }
        }
    }
    else if (item->whatsThis() == "x-Axis::AutoScrollWindow")
    {
        _configOption.x_axis_auto_scroll_window = item->data(Qt::EditRole).toDouble();
        // 여기서는 데이터만 셋하고, 실제 새로운 데이터 수신시에 그래프를 새로 그린다.
    }
    else if (item->whatsThis() == "x-Axis::Begin")
    {
        _configOption.x_axis_begin_sec = item->data(Qt::EditRole).toDouble();
        AdjustPlotXRange();
    }
    else if (item->whatsThis() == "x-Axis::End")
    {
        _configOption.x_axis_end_sec = item->data(Qt::EditRole).toDouble();
        AdjustPlotXRange();
    }
    else if (item->whatsThis() == "y-Axis::AutoScale")
    {
        _configOption.y_axis_auto_scale = (item->checkState() == Qt::Checked ? true : false);

        const QStandardItem *item = nullptr;
        if (_configOption.y_axis_auto_scale)
        {
            item = FindFirstConfigOptionItem("y-Axis", "Lower bound", 1);
            if (item)
            {
                const_cast<QStandardItem *>(item)->setEnabled(false);
            }
            item = FindFirstConfigOptionItem("y-Axis", "Upper bound", 1);
            if (item)
            {
                const_cast<QStandardItem *>(item)->setEnabled(false);
            }
            ui->plotwidget->yAxis->rescale(true);
        }
        else
        {
            double lb = ui->plotwidget->yAxis->range().lower;
            double ub = ui->plotwidget->yAxis->range().upper;

            item = FindFirstConfigOptionItem("y-Axis", "Lower bound", 1);
            if (item)
            {
                const_cast<QStandardItem *>(item)->setEnabled(true);
                const_cast<QStandardItem *>(item)->setData(lb, Qt::EditRole);
            }
            item = FindFirstConfigOptionItem("y-Axis", "Upper bound", 1);
            if (item)
            {
                const_cast<QStandardItem *>(item)->setEnabled(true);
                const_cast<QStandardItem *>(item)->setData(ub, Qt::EditRole);
            }
        }
    }
    else if (item->whatsThis() == "y-Axis::LBound")
    {
        _configOption.y_axis_lbound = item->data(Qt::EditRole).toDouble();
        AdjustPlotYRange();
    }
    else if (item->whatsThis() == "y-Axis::UBound")
    {
        _configOption.y_axis_ubound = item->data(Qt::EditRole).toDouble();
        AdjustPlotYRange();
    }
    else if (item->whatsThis() == "Legend::Visible")
    {
        _configOption.legend_visible = (item->checkState() == Qt::Checked ? true : false);
        ui->plotwidget->legend->setVisible(_configOption.legend_visible);
    }
    else if (item->whatsThis() == "Legend::Location")
    {
        QString loc = item->data(Qt::EditRole).toString();
        if (loc == "Left-Top")
        {
            _configOption.legend_location = Qt::AlignLeft | Qt::AlignTop;
        }
        else if (loc == "Left-Middle")
        {
            _configOption.legend_location = Qt::AlignLeft | Qt::AlignVCenter;
        }
        else if (loc == "Left-Bottom")
        {
            _configOption.legend_location = Qt::AlignLeft | Qt::AlignBottom;
        }
        else if (loc == "Right-Top")
        {
            _configOption.legend_location = Qt::AlignRight | Qt::AlignTop;
        }
        else if (loc == "Right-Middle")
        {
            _configOption.legend_location = Qt::AlignRight | Qt::AlignVCenter;
        }
        else if (loc == "Right-Bottom")
        {
            _configOption.legend_location = Qt::AlignRight | Qt::AlignBottom;
        }
        ui->plotwidget->axisRect()->insetLayout()->setInsetAlignment(0, _configOption.legend_location);
    }
    else if (item->whatsThis() == "Style::LineWidth")
    {
        _configOption.style_line_width = item->data(Qt::EditRole).toUInt();

        if (_configOption.style_line_width >= 1 && _configOption.style_line_width <= 5)
        {
            QPen pen;
            for (int i = 0; i < ui->plotwidget->graphCount(); ++i)
            {
                pen = ui->plotwidget->graph(i)->pen();
                pen.setWidth(_configOption.style_line_width);
                ui->plotwidget->graph(i)->setPen(pen);
            }
        }
        else
        {
            if (_configOption.style_line_width < 1)
                _configOption.style_line_width = 1;
            else if (_configOption.style_line_width > 5)
                _configOption.style_line_width = 5;

            QStandardItem const *item;
            item = FindFirstConfigOptionItem("Style", "Line width(1~5)", 1);
            if (item)
            {
                const_cast<QStandardItem *>(item)->setData(_configOption.style_line_width, Qt::EditRole);
            }
        }
    }
    else if (item->whatsThis() == "Data series::Visible")
    {
        QCPGraph *graph = (QCPGraph *)item->data().value<void *>();
        if (graph)
        {
            graph->setVisible(
                (item->checkState() == Qt::Checked ? true : false));

            if (item->data(Qt::DisplayRole).toString() != item->data(Qt::UserRole + 2).toString())
            {
                graph->setName(item->data(Qt::DisplayRole).toString());
            }
        }
    }
    else if (item->whatsThis() == "Data series::Visible_All" || item->whatsThis() == "Data series::Visible_Group")
    {
        for (int row = 0; row < item->rowCount(); row++)
        {
            QStandardItem *graph_visible_item = item->child(row, 0);
            if (!graph_visible_item)
                continue;

            graph_visible_item->setCheckState(item->checkState());
        }
    }
}

void PlotWindow::OnConfigItemGraphClicked(QString name, int index)
{
    QStandardItem const *item = FindFirstConfigOptionItem("Data series", name, 0, true);
    if (item)
    {
        if (const_cast<QStandardItem *>(item)->checkState() == Qt::Checked)
        {
            // deselect all
            DeselectAllGraphs();

            // select new
            SelectGraph(name, index);
        }
        else
        {
            // do nothing
            return;
        }
    }
}

void PlotWindow::OnConfigItemGraphColorSelected(QString name, int index, QColor color)
{
    // qDebug() << "selected color = " << color.red() << "," << color.green() << "," << color.blue();

    QStandardItem const *item_title = FindFirstConfigOptionItem("Data series", name, 0, true);
    if (item_title)
    {
        QStandardItem* item_option = (QStandardItem*)(item_title->data(Qt::UserRole + 3).value<void*>());
        QCPGraph* graph = (QCPGraph*)(item_title->data().value<void*>());

        if (item_option && graph)
        {
            item_option->setData(color, Qt::BackgroundRole);

            QPen pen(color); // TODO: 현재 설정된 line width 등 세팅을 가져와야 함.
            graph->setPen(pen);
        }
    }
}

void PlotWindow::OnConfigItemGraphRestoreNameActionSelected(QString name, int index)
{
    if (index == -1) // special case! : restore data names for all
    {
        auto items = _configModel->findItems("Data series", Qt::MatchExactly | Qt::MatchRecursive, 0);
        const QStandardItem *parent;
        const QStandardItem *child;
        foreach (const QStandardItem *citem, items)
        {
            if (citem->parent())
                continue;

            for (int row = 0; row < citem->rowCount(); row++)
            {
                child = citem->child(row, 0);
                // qDebug() << child->data(Qt::DisplayRole).toString();

                if (child->whatsThis() == "Data series::Visible")
                {
                    QStandardItem*item_title = const_cast<QStandardItem*>(child);
                    QCPGraph* graph = (QCPGraph*)(item_title->data().value<void*>());
                    if (graph) 
                    {
                        QString original_name = item_title->data(Qt::UserRole + 2).toString();
                        item_title->setText(original_name);
                        graph->setName(original_name);
                    }
                }
                else if (child->whatsThis() == "Data series::Visible_Group")
                {
                    for (int row = 0; row < child->rowCount(); row++)
                    {
                        QStandardItem*item_title = const_cast<QStandardItem*>(child->child(row, 0));
                        // qDebug() << item_title->data(Qt::DisplayRole).toString();

                        if (item_title->whatsThis() == "Data series::Visible")
                        {
                            QCPGraph* graph = (QCPGraph*)(item_title->data().value<void*>());
                            if (graph) 
                            {
                                QString original_name = item_title->data(Qt::UserRole + 2).toString();
                                item_title->setText(original_name);
                                graph->setName(original_name);
                            }
                        }
                    }
                }
            }
        }
    }
    else
    {
        QStandardItem* item_title = const_cast<QStandardItem*>(FindFirstConfigOptionItem("Data series", name, 0, true));
        if (item_title)
        {
            QCPGraph* graph = (QCPGraph*)(item_title->data().value<void*>());
            if (graph) 
            {
                QString original_name = item_title->data(Qt::UserRole + 2).toString();
                item_title->setText(original_name);
                graph->setName(original_name);
            }
        }
    }
}

void PlotWindow::OnConfigItemGraphShowDataInfoActionSelected(QString name, int index)
{
    QStandardItem* item_title = const_cast<QStandardItem*>(FindFirstConfigOptionItem("Data series", name, 0, true));
    if (item_title)
    {
        const QCPGraph* graph = (const QCPGraph*)(item_title->data().value<void*>());
        if (graph) 
        {
            // qDebug() << "PlotWindow::OnConfigItemGraphShowDataInfoActionSelected";
            // qDebug() << graph->dataCount();

            DataInfoDlg* dlg = new DataInfoDlg(graph, this);
            dlg->setModal(false);
            dlg->show();
        }
    }
}

QByteArray PlotWindow::SavePlotConfig() const
{
    // qDebug() << "SavePlotConfig";
    QString configstr;
    QTextStream str(&configstr);
    str << "x-Axis::AutoScroll"
        << "," << _configOption.x_axis_auto_scroll << ","
        << "x-Axis::AutoScrollWindow"
        << "," << _configOption.x_axis_auto_scroll_window << ","
        << "x-Axis::Begin"
        << "," << _configOption.x_axis_begin_sec << ","
        << "x-Axis::End"
        << "," << _configOption.x_axis_end_sec << ","
        << "y-Axis::AutoScale"
        << "," << _configOption.y_axis_auto_scale << ","
        << "y-Axis::LBound"
        << "," << _configOption.y_axis_lbound << ","
        << "y-Axis::UBound"
        << "," << _configOption.y_axis_ubound << ","
        << "Legend::Visible"
        << "," << _configOption.legend_visible << ","
        << "Legend::Location"
        << "," << _configOption.legend_location << ","
        << "Style::LineWidth"
        << "," << _configOption.style_line_width;
    // qDebug() << configstr;
    return configstr.toUtf8();
}

bool PlotWindow::RestorePlotConfig(const QByteArray &config)
{
    QString configstr = QString::fromUtf8(config);
    // qDebug() << "RestorePlotConfig: " << configstr;
    QList<QByteArray> items = config.split(',');
    int idx = 0;
    while (idx < (items.size() - 1))
    {
        if ("x-Axis::AutoScroll" == items[idx])
        {
            AutoScroll(items[idx + 1].toInt() == 0 ? false : true);
        }
        else if ("x-Axis::AutoScrollWindow" == items[idx])
        {
            AutoScrollWindow(items[idx + 1].toDouble());
        }
        //        else if ("x-Axis::Begin" == items[idx]) {
        //            SetXBegin(items[idx+1].toDouble());
        //        }
        //        else if ("x-Axis::End" == items[idx]) {
        //            SetXEnd(items[idx+1].toDouble());
        //        }
        else if ("y-Axis::AutoScale" == items[idx])
        {
            AutoScale(items[idx + 1].toInt() == 0 ? false : true);
        }
        else if ("y-Axis::LBound" == items[idx])
        {
            SetYLBound(items[idx + 1].toDouble());
        }
        else if ("y-Axis::UBound" == items[idx])
        {
            SetYUBound(items[idx + 1].toDouble());
        }
        else if ("Legend::Visible" == items[idx])
        {
            ShowLegend(items[idx + 1].toInt() == 0 ? false : true);
        }
        else if ("Legend::Location" == items[idx])
        {
            SetLegendLocation(QFlags<Qt::AlignmentFlag>(items[idx + 1].toInt()));
        }
        else if ("Style::LineWidth" == items[idx])
        {
            SetLineWidth(items[idx + 1].toInt());
        }

        idx += 2;
    }

    return false;
}

QByteArray PlotWindow::SaveDataSeriesConfig() const
{
    QString configstr;
    QTextStream str(&configstr);
    QStandardItem *child_name = nullptr;
    QStandardItem *data_name = nullptr;
    QStandardItem *data_color = nullptr;

    // qDebug() << "SaveDataSeriesConfig";
    auto items = _configModel->findItems("Data series", Qt::MatchExactly | Qt::MatchRecursive, 0);
    QList<QStandardItem *> item_graph_visible;
    foreach (QStandardItem *data_series_root, items)
    {
        if (data_series_root->parent())
            continue;

        for (int chiIndex = 0; chiIndex < data_series_root->rowCount(); chiIndex++)
        {
            child_name = data_series_root->child(chiIndex, 0);

            if (child_name->whatsThis() == "Data series::Visible_Group")
            {
                for (int subIndex = 0; subIndex < child_name->rowCount(); subIndex++)
                {
                    data_name = child_name->child(subIndex, 0);
                    data_color = child_name->child(subIndex, 1);
                    QBrush bg_brush = data_color->background();
                    str << data_name->data(Qt::UserRole + 2).toString() << ",";
                    str << (data_name->checkState() == Qt::Checked ? 1 : 0) << ",";
                    str << data_name->data(Qt::DisplayRole).toString() << ",";              // alias(display) name
                    str << bg_brush.color().red() << "," << bg_brush.color().green() << "," << bg_brush.color().blue() << ",";
                }
            }
            else
            {
                data_name = child_name;
                data_color = data_series_root->child(chiIndex, 1);
                QBrush bg_brush = data_color->background();
                str << data_name->data(Qt::UserRole + 2).toString() << ",";
                str << (data_name->checkState() == Qt::Checked ? 1 : 0) << ",";
                str << data_name->data(Qt::DisplayRole).toString() << ",";              // alias(display) name
                str << bg_brush.color().red() << "," << bg_brush.color().green() << "," << bg_brush.color().blue() << ",";
            }
        }
        break;
    }
    // qDebug() << configstr;
    return configstr.toUtf8();
}

bool PlotWindow::RestoreDataSeriesConfig(const QByteArray &config, const QString &name, QString& alias, QColor& color)
{
    QString configstr = QString::fromUtf8(config);
    // qDebug() << "RestoreDataSeriesConfig: " << configstr;
    QList<QByteArray> items = config.split(',');
    int idx = 0;
    while (idx < (items.size() - 5))
    {
        if (name == items[idx])
        {
            SetGraphVisible(name, items[idx + 1].toInt() == 0 ? false : true);
            alias = QString(items[idx + 2].toStdString().c_str());
            color = QColor(items[idx + 3].toInt(), items[idx + 4].toInt(), items[idx + 5].toInt());
            return true;
        }

        idx += 6;
    }
    return false;
}

void PlotWindow::OnHorzScrollBarChanged(int value)
{
    // qDebug() << "OnHorzScrollBarChanged: value=" << value;
    if (qAbs(ui->plotwidget->xAxis->range().center() - value / 100.0) > 0.01) // if user is dragging plot, we don't want to replot twice
    {
        ui->plotwidget->xAxis->setRange(value / 100.0, ui->plotwidget->xAxis->range().size(), Qt::AlignCenter);
        // ui->plotwidget->replot();
    }
}

void PlotWindow::OnVertScrollBarChanged(int value)
{
    // qDebug() << "OnVertScrollBarChanged: value=" << value;
    if (qAbs(ui->plotwidget->yAxis->range().center() + value / 100.0) > 0.01) // if user is dragging plot, we don't want to replot twice
    {
        ui->plotwidget->yAxis->setRange(-value / 100.0, ui->plotwidget->yAxis->range().size(), Qt::AlignCenter);
        // ui->plotwidget->replot();
    }
}

void PlotWindow::OnXAxisRangeChanged(QCPRange range)
{
    // qDebug() << "OnXAxisRangeChanged: center=" << range.center() << ", size=" << range.size();
    ui->horizontalScrollBar->setValue(qRound(range.center() * 100.0));  // adjust position of scroll bar slider
    ui->horizontalScrollBar->setPageStep(qRound(range.size() * 100.0)); // adjust size of scroll bar slider
}

void PlotWindow::OnYAxisRangeChanged(QCPRange range)
{
    // qDebug() << "OnYAxisRangeChanged: center=" << range.center() << "/" << qRound(-range.center()*100.0) << ", size=" << range.size();
    ui->verticalScrollBar->setValue(qRound(-range.center() * 100.0)); // adjust position of scroll bar slider
    ui->verticalScrollBar->setPageStep(qRound(range.size() * 100.0)); // adjust size of scroll bar slider
}

void PlotWindow::OnSelectionChangedByUser()
{
    // qDebug() << "OnSelectionChangedByUser()";
    _plotConfig->clearSelection();

    const QList<QCPGraph *> graphs = ui->plotwidget->selectedGraphs();
    for (auto graph : qAsConst(graphs))
    {
        // Q_UNUSED(graph); // (void)graph;
        // qDebug() << "graph selected : " << graph->name();

        for (int i = 0; i < ui->plotwidget->legend->itemCount(); i++)
        {
            QCPPlottableLegendItem *item = dynamic_cast<QCPPlottableLegendItem *>(ui->plotwidget->legend->item(i));
            if (item && item->plottable()->name() == graph->name())
            {
                item->setSelected(true);
                break;
            }
        }
    }
    const QList<QCPLegend *> legends = ui->plotwidget->selectedLegends();
    for (auto legend : qAsConst(legends))
    {
        for (int i = 0; i < legend->itemCount(); i++)
        {
            QCPPlottableLegendItem *item = dynamic_cast<QCPPlottableLegendItem *>(legend->item(i));
            if (item && item->selected())
            {
                SelectGraph(item->plottable()->name());

                const QStandardItem* config_item = FindFirstConfigOptionItem("Data series", item->plottable()->name(), 0, true);
                if (config_item)
                {
                    QModelIndex index = _configModel->indexFromItem(config_item);
                    _plotConfig->selectItem(index);
                }

                break;
            }
        }
    }
}

void PlotWindow::RecalculatePlotLayout()
{
    ui->plotwidget->setFixedSize(QSize(ui->centralwidget->width() - 18, ui->centralwidget->height() - 18));
    ui->horizontalScrollBar->setGeometry(0, ui->centralwidget->height() - 18, ui->centralwidget->width() - 18, 18);
    ui->verticalScrollBar->setGeometry(ui->centralwidget->width() - 18, 0, 18, ui->centralwidget->height() - 18);
}

void PlotWindow::dragEnterEvent(QDragEnterEvent* event)
{
    event->acceptProposedAction();
}

void PlotWindow::dragLeaveEvent(QDragLeaveEvent* event)
{
    event->accept();
}

void PlotWindow::dragMoveEvent(QDragEnterEvent* event)
{
    event->acceptProposedAction();
}

void PlotWindow::dropEvent(QDropEvent* event)
{
    if (_plotType == PlotType::RT_PLOT)
        return;

    const QMimeData* mimeData = event->mimeData();

    if (mimeData->hasUrls())
    {
        QStringList paths;
        QList<QUrl> urls = mimeData->urls();
        foreach(QUrl url, urls)
        {
            // qDebug() << "drag & drop : " << url.toLocalFile();
            paths.push_back(url.toLocalFile());
        }

        // reset(clear) all graph and data before importing new graph and data
        ResetPlot();

        // qDebug() << "import files: " << paths;
        ui->plotwidget->ImportData(paths);

        ExtendAll();
    }
}

void PlotWindow::ExtendAll()
{
    bool found;
    QCPRange range;
    range = ui->plotwidget->getKeyRange(found);
    if (found)
    {
        const QStandardItem *item = nullptr;
        // if (!_configOption.x_axis_auto_scroll) {
        item = FindFirstConfigOptionItem("x-Axis", "Begin(s)", 1);
        if (item)
        {
            const_cast<QStandardItem *>(item)->setData(range.lower, Qt::EditRole);
        }
        item = FindFirstConfigOptionItem("x-Axis", "End(s)", 1);
        if (item)
        {
            const_cast<QStandardItem *>(item)->setData(range.upper, Qt::EditRole);
        }
        //}

        // AdjustPlotXRange();
        ui->plotwidget->xAxis->setRange(range);
    }
    range = ui->plotwidget->getValueRange(found);
    if (found)
    {
        double extend = range.upper - range.lower;
        range.lower -= (extend * 0.05);
        range.upper += (extend * 0.05);

        const QStandardItem *item = nullptr;
        // if (!_configOption.y_axis_auto_scale) {
        item = FindFirstConfigOptionItem("y-Axis", "Lower bound", 1);
        if (item)
        {
            const_cast<QStandardItem *>(item)->setData(range.lower, Qt::EditRole);
        }
        item = FindFirstConfigOptionItem("y-Axis", "Upper bound", 1);
        if (item)
        {
            const_cast<QStandardItem *>(item)->setData(range.upper, Qt::EditRole);
        }
        //}

        // AdjustPlotYRange();
        ui->plotwidget->yAxis->setRange(range);
    }
}

void PlotWindow::AdjustPlotXRange()
{
    if (!_configOption.x_axis_auto_scroll)
    {
        ui->plotwidget->xAxis->setRange(
            _configOption.x_axis_begin_sec,
            _configOption.x_axis_end_sec);
    }
}

void PlotWindow::AdjustPlotYRange()
{
    if (!_configOption.y_axis_auto_scale)
    {
        if (_configOption.y_axis_lbound < _configOption.y_axis_ubound)
        {
            ui->plotwidget->yAxis->setRange(
                _configOption.y_axis_lbound,
                _configOption.y_axis_ubound);
        }
    }
}

const QStandardItem *PlotWindow::FindFirstConfigOptionItem(const QString &cat, const QString &item, int col, bool recursive)
{
    auto items = _configModel->findItems(item, Qt::MatchExactly | Qt::MatchRecursive, 0);
    const QStandardItem *parent;
    foreach (const QStandardItem *citem, items)
    {
        parent = citem->parent();
        while (parent)
        {
            if (parent->text() == cat)
                return citem->parent()->child(citem->row(), col);

            if (recursive)
                parent = parent->parent();
            else
                break;
        }
   }
    return nullptr;
}

const QStandardItem* PlotWindow::FindFirstConfigOptionGroup(const QString& cat, const QString& group, int col, bool recursive)
{
    auto items = _configModel->findItems(group, Qt::MatchExactly | Qt::MatchRecursive, 0);
    foreach (const QStandardItem *citem, items)
    {
        if (citem->parent() && citem->parent()->text() == cat && citem->whatsThis() == QString("Data series::Visible_Group"))
        {
            return citem->parent()->child(citem->row(), col);
        }
    }
    return nullptr;
}

/*!
 * \brief PlotWindow::OnRefreshPlot
 * 주기적으로 새로운 데이터가 있는지 확인한 후 그래프를 새로 그린다.
 */
void PlotWindow::OnRefreshPlot()
{
    // qDebug() << "PlotWindow::OnRefreshPlot";

    QMutexLocker locker(&_graphDataGuard);

    if (_isNewDataReceived)
    {
        _isNewDataReceived = false; // reset switch!

        double di =
            qMax(0.0, _lastRecvTime - _configOption.x_axis_auto_scroll_window);
        if (_configOption.x_axis_auto_scroll)
            ui->plotwidget->xAxis->setRange(di, _lastRecvTime);
        if (_configOption.y_axis_auto_scale)
            ui->plotwidget->yAxis->rescale(true);

        // update frame rate
        ui->statusbar->showMessage(
            QString("%1 FPS, Total Data received: %2")
                .arg(0.0, 0, 'f', 0)
                .arg(ui->plotwidget->graph(0)->data()->size()),
            0);

        // adjust scroll-bars
        // 새로운 데이터가 들어오면, 새로운 데이터 시간을 기준으로 scroll-bar의
        // range를 변경
        ui->horizontalScrollBar->setRange(
            0, (_lastRecvTime + 0.5 * ui->plotwidget->xAxis->range().size()) *
                   100);
    }

    ui->plotwidget->replot();
}

#ifdef USE_EMUL_DATA
void PlotWindow::OnRecvEmul(const DataSeriesEmul &data)
{
    // qDebug() << "PlotWindow::OnRecvEmul";

    ui->plotwidget->graph(0)->addData(data.time, data.data[0]);
    ui->plotwidget->graph(1)->addData(data.time, data.data[1]);
    ui->plotwidget->graph(2)->addData(data.time, data.data[2]);
    ui->plotwidget->graph(3)->addData(data.time, data.data[3]);
    _lastRecvTime = data.time;
    _isNewDataReceived = true;
}
#endif

void PlotWindow::OnClearTriggered()
{
    ResetData();
}

void PlotWindow::OnResetTriggered()
{
    ResetPlot();
}

void PlotWindow::OnImportTriggered()
{
    QString selectedFilters;
    QStringList filenames = QFileDialog::getOpenFileNames(this, "Select CSV/MCAP data files to import graphs and data from...", QDir::currentPath(), tr("Data Files (*.csv, *.mcap);;All Files (*.*)"), &selectedFilters, QFileDialog::ReadOnly);
    if (filenames.empty())
        return;

    // reset(clear) all graph and data before importing new graph and data
    ResetPlot();

    qDebug() << "import files: " << filenames;
    ui->plotwidget->ImportData(filenames);

    ExtendAll();
}

void PlotWindow::OnAppendTriggered()
{
    QString selectedFilters;
    QStringList filenames = QFileDialog::getOpenFileNames(this, "Select CSV/MCAP data files to import graphs and data from...", QDir::currentPath(), tr("Data Files (*.csv, *.mcap);;All Files (*.*)"), &selectedFilters, QFileDialog::ReadOnly);
    if (filenames.empty())
        return;

    qDebug() << "import files(append mode): " << filenames;
    ui->plotwidget->ImportData(filenames);

    // ExtendAll();
}

void PlotWindow::OnExportTriggered()
{
    QString export_dir = QFileDialog::getExistingDirectory(this, "Select a directory to export graphs and data into...", QDir::currentPath(), QFileDialog::ShowDirsOnly);
    if (export_dir.isNull())
        return;

    qDebug() << "export dir: " << export_dir;
    ui->plotwidget->ExportData(export_dir);
}

void PlotWindow::OnCloseTriggered()
{
    this->close();
}

void PlotWindow::OnExtendAllTriggered()
{
    ExtendAll();
}

void PlotWindow::OnMousePressed(QMouseEvent *event)
{
    Qt::KeyboardModifiers km = QGuiApplication::keyboardModifiers();
    if (event->button() == Qt::LeftButton && (km & Qt::ShiftModifier))
    {
        ui->plotwidget->setSelectionRectMode((QCP::srmSelect));
    }
    else if (event->button() == Qt::RightButton)
    {
        ui->plotwidget->setSelectionRectMode(QCP::srmZoom);
    }
    else
    {
        ui->plotwidget->setSelectionRectMode(QCP::srmNone);
    }
}

void PlotWindow::OnMouseReleased(QMouseEvent *event)
{
    Q_UNUSED(event);
}

// void PlotWindow::OnRangeChanged(const QCPRange &newRange, const QCPRange &oldRange)
//{
//     qDebug() << "OnRangeChanged" << oldRange.lower << "," << oldRange.upper << " -> " << newRange.lower << "," << newRange.upper;
//     Q_UNUSED(newRange);
//     Q_UNUSED(oldRange);
// }

void PlotWindow::OnRefreshRateFastTriggered()
{
    qDebug() << "PlotWindow::OnRefreshRateFastTriggered()";
}

void PlotWindow::OnRefreshRateNormalTriggered()
{
    qDebug() << "PlotWindow::OnRefreshRateNormalTriggered()";
}

void PlotWindow::OnRefreshRateSlowTriggered()
{
    qDebug() << "PlotWindow::OnRefreshRateSlowTriggered()";
}
