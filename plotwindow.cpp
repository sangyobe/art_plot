#include "pconstants.h"
#include "plotconfig.h"
#include "plotwindow.h"
#include "ui_plotwindow.h"
#include <cassert>
#include <iostream>
#include <memory>

PlotWindow::ConfigOption::ConfigOption() :
    x_axis_auto_scroll(false),
    x_axis_auto_scroll_window(10.0),
    x_axis_begin_sec(0.0),
    x_axis_end_sec(1.0),
    y_axis_auto_scale(false),
    y_axis_lbound(-1.0),
    y_axis_ubound(1.0),
    legend_visible(true)
{}

PlotWindow::PlotWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::PlotWindow),
    _configModel(nullptr)
{
    ui->setupUi(this);
    
    //------------------------------------------------------------------
    // Creat and set-up plot configuration(properties) panel
    //
    _plotConfig = new PlotConfig(this);
    _plotConfig->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    addDockWidget(Qt::LeftDockWidgetArea, _plotConfig);

    BuildConfig();
    
    
    //------------------------------------------------------------------
    // Config plot window
    //
    
    // legend
    ui->plotwidget->legend->setVisible(_configOption.legend_visible);
    QFont legendFont = font();
    legendFont.setPointSize(9);
    ui->plotwidget->legend->setFont(legendFont);
    ui->plotwidget->legend->setBrush(QBrush(QColor(255, 255, 255, 230)));

    // axis
    ui->plotwidget->xAxis->setLabel("x");
    ui->plotwidget->yAxis->setLabel("y");
    // configure right and top axis to show ticks but no labels:
    // (see QCPAxisRect::setupFullAxesBox for a quicker method to do this)
    ui->plotwidget->xAxis2->setVisible(true);
    ui->plotwidget->xAxis2->setTickLabels(false);
    ui->plotwidget->yAxis2->setVisible(true);
    ui->plotwidget->yAxis2->setTickLabels(false);

    //
    // set user interaction options
    //
    // enable dragging using maouse
    ui->plotwidget->setInteraction(QCP::iRangeDrag, true);
    // sellect enabled dragging direction
    ui->plotwidget->axisRect(0)->setRangeDrag(Qt::Vertical | Qt::Horizontal);
    // enable zoom
    ui->plotwidget->setInteraction(QCP::iRangeZoom, true);
    ui->plotwidget->axisRect(0)->setRangeZoom(Qt::Vertical | Qt::Horizontal);
    ui->plotwidget->axisRect(0)->setRangeZoomFactor(1.1, 1.1);
    // enable clicking items
//    ui->plotwidget->setInteraction(QCP::iSelectPlottables);
//    ui->plotwidget->setInteraction(QCP::iMultiSelect, true);
//    ui->plotwidget->setMultiSelectModifier(Qt::ControlModifier);

    // scroll-bars
    ui->horizontalScrollBar->setRange(0,  100000);
    ui->verticalScrollBar->setRange(-10000, 10000);
    connect(ui->horizontalScrollBar, SIGNAL(valueChanged(int)), this, SLOT(OnHorzScrollBarChanged(int)));
    connect(ui->verticalScrollBar,   SIGNAL(valueChanged(int)), this, SLOT(OnVertScrollBarChanged(int)));
    connect(ui->plotwidget->xAxis, SIGNAL(rangeChanged(QCPRange)), this, SLOT(OnXAxisRangeChanged(QCPRange)));
    connect(ui->plotwidget->yAxis, SIGNAL(rangeChanged(QCPRange)), this, SLOT(OnYAxisRangeChanged(QCPRange)));


    ResetPlot();
    //ui->plotwidget->replot();


    //------------------------------------------------------------------
    // initialize the plot refresh timer
    //
    _refreshPlotTimer = std::unique_ptr<QTimer>(new QTimer(this));
    connect(_refreshPlotTimer.get(), SIGNAL(timeout()), this, SLOT(OnRefreshPlot()));
    _refreshPlotTimer->setInterval(100); // msec
    _refreshPlotTimer->start();


#ifdef USE_EMUL_DATA
    this->AddGraph("Sin", LineColor<0>());
    this->AddGraph("Sin^2", LineColor<1>());
    this->AddGraph("Cos", LineColor<2>());
    this->AddGraph("Cos^2", LineColor<3>());

    _dataSource = std::unique_ptr<DataSourceEmul>(new DataSourceEmul("Sample Data Series"));
    connect(_dataSource.get(), SIGNAL(new_data(const DataSeriesEmul&)), this, SLOT(OnRecvEmul(const DataSeriesEmul&)));
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

void PlotWindow::ResetPlot()
{
    for( int g=0; g<ui->plotwidget->graphCount(); g++ )
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
    ui->plotwidget->replot();
}

void PlotWindow::SetWindowTitle(const QString &title)
{
    setWindowTitle(title);
}

QString PlotWindow::GetWindowTitle() const {
    return windowTitle();
}

void PlotWindow::ShowLegend(bool show)
{
    const QStandardItem* item = FindFirstConfigOptionItem("Legend", "Visible");
    if (!item)
        return;

    const_cast<QStandardItem*>(item)->setCheckState(show? Qt::Checked : Qt::Unchecked);
}

void PlotWindow::AutoScroll(bool on)
{
    const QStandardItem* item = FindFirstConfigOptionItem("x-Axis", "Auto scroll");
    if (!item)
        return;

    const_cast<QStandardItem*>(item)->setCheckState(on? Qt::Checked : Qt::Unchecked);
}

void PlotWindow::AutoScrollWindow(double dt_sec)
{
    const QStandardItem* item = FindFirstConfigOptionItem("x-Axis", "Scroll Window(s)");
    if (!item)
        return;

    const_cast<QStandardItem*>(item)->setData(dt_sec, Qt::EditRole);
}

void PlotWindow::SetXRange(double ti, double tf)
{
    QStandardItem const * item;

    item = FindFirstConfigOptionItem("x-Axis", "Begin(s)");
    if (item) {
        const_cast<QStandardItem*>(item)->setData(ti, Qt::EditRole);
    }

    item = FindFirstConfigOptionItem("x-Axis", "End(s)");
    if (item) {
        const_cast<QStandardItem*>(item)->setData(tf, Qt::EditRole);
    }
}

void PlotWindow::AutoScale(bool on)
{
    const QStandardItem* item = FindFirstConfigOptionItem("y-Axis", "Auto scale");
    if (!item)
        return;

    const_cast<QStandardItem*>(item)->setCheckState(on ? Qt::Checked : Qt::Unchecked);
}

void PlotWindow::SetYRange(double lbound, double ubound)
{
    QStandardItem const * item;

    item = FindFirstConfigOptionItem("y-Axis", "Lower bound");
    if (item) {
        const_cast<QStandardItem*>(item)->setData(lbound, Qt::EditRole);
    }

    item = FindFirstConfigOptionItem("y-Axis", "Upper bound");
    if (item) {
        const_cast<QStandardItem*>(item)->setData(ubound, Qt::EditRole);
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

void PlotWindow::BuildConfig()
{
    QList<QStandardItem*> items;
    QStandardItem* item_title;
    QStandardItem* item;

    _configModel = new QStandardItemModel;
    _configModel->setColumnCount(2);

    QStandardItem* x_axis_root = new QStandardItem("x-Axis");
    QStandardItem* y_axis_root = new QStandardItem("y-Axis");
    QStandardItem* legend_root = new QStandardItem("Legend");
    QStandardItem* data_series_root = new QStandardItem("Data series");
    _configModel->appendRow(x_axis_root);
    _configModel->appendRow(y_axis_root);
    _configModel->appendRow(legend_root);
    _configModel->appendRow(data_series_root);

    // x-Axis
    items.clear();
    item_title = new QStandardItem("Auto scroll");
    item = new QStandardItem();
    item->setCheckable(true);
    item->setCheckState(_configOption.x_axis_auto_scroll ? Qt::Checked : Qt::Unchecked);
    item->setWhatsThis("x-Axis::AutoScroll");
    items.append(item_title);
    items.append(item);
    x_axis_root->appendRow(items);

    items.clear();
    item_title = new QStandardItem("Scroll Window(s)");
    item = new QStandardItem();
    item->setEditable(true);
    item->setData(_configOption.x_axis_auto_scroll_window, Qt::EditRole);
    item->setWhatsThis("x-Axis::AutoScrollWindow");
    items.append(item_title);
    items.append(item);
    x_axis_root->appendRow(items);

    items.clear();
    item_title = new QStandardItem("Begin(s)");
    item = new QStandardItem();
    item->setEditable(true);
    item->setData(_configOption.x_axis_begin_sec, Qt::EditRole);
    item->setWhatsThis("x-Axis::Begin");
    items.append(item_title);
    items.append(item);
    x_axis_root->appendRow(items);

    items.clear();
    item_title = new QStandardItem("End(s)");
    item = new QStandardItem();
    item->setEditable(true);
    item->setData(_configOption.x_axis_end_sec, Qt::EditRole);
    item->setWhatsThis("x-Axis::End");
    items.append(item_title);
    items.append(item);
    x_axis_root->appendRow(items);

    // y-Axis
    items.clear();
    item_title = new QStandardItem("Auto scale");
    item = new QStandardItem();
    item->setCheckable(true);
    item->setCheckState(_configOption.y_axis_auto_scale ? Qt::Checked : Qt::Unchecked);
    item->setWhatsThis("y-Axis::AutoScale");
    items.append(item_title);
    items.append(item);
    y_axis_root->appendRow(items);

    items.clear();
    item_title = new QStandardItem("Lower bound");
    item = new QStandardItem();
    item->setEditable(true);
    item->setData(_configOption.y_axis_lbound, Qt::EditRole);
    item->setWhatsThis("y-Axis::LBound");
    items.append(item_title);
    items.append(item);
    y_axis_root->appendRow(items);

    items.clear();
    item_title = new QStandardItem("Upper bound");
    item = new QStandardItem();
    item->setEditable(true);
    item->setData(_configOption.y_axis_ubound, Qt::EditRole);
    item->setWhatsThis("y-Axis::UBound");
    items.append(item_title);
    items.append(item);
    y_axis_root->appendRow(items);

    // legend
    items.clear();
    item_title = new QStandardItem("Visible");
    item = new QStandardItem();
    item->setCheckable(true);
    item->setCheckState(_configOption.legend_visible ? Qt::Checked : Qt::Unchecked);
    item->setWhatsThis("Legend::Visible");
    items.append(item_title);
    items.append(item);
    legend_root->appendRow(items);

    _plotConfig->setConfigModel(_configModel);

    // header
    QStringList header_labels;
    header_labels << "Property" << "Value";
    _configModel->setHeaderData(0, Qt::Horizontal, header_labels[0]);
    _configModel->setHeaderData(1, Qt::Horizontal, header_labels[1]);


    connect(_configModel, SIGNAL(itemChanged(QStandardItem*)), this, SLOT(OnConfigChanged(QStandardItem*)));
}

int PlotWindow::AddGraph(const QString &name, const QColor &color)
{
    QCPGraph *graph = ui->plotwidget->addGraph();
    graph->setName(name);
    // line style
    graph->setLineStyle(QCPGraph::lsLine/*lsStepRight*/);
    graph->setPen(QPen(color));
    //graph->setScatterStyle(QCPScatterStyle::ssPlus);

    auto items = _configModel->findItems("Data series", Qt::MatchExactly | Qt::MatchRecursive, 0);
    QList<QStandardItem*> item_graph_visible;
    foreach (QStandardItem* data_series_root, items) {
        if (data_series_root->parent())
            continue;

        QStandardItem* item_title = new QStandardItem(name);
        QStandardItem* item_option = new QStandardItem();

        item_title->setSelectable(false);
        item_option->setCheckable(true);
        item_option->setCheckState(Qt::Checked);
        //item_option->setWhatsThis(QString("Data series::%s").arg(name));
        item_option->setWhatsThis(QString("Data series::Visible"));
        item_option->setSelectable(false);
        item_option->setData(QVariant::fromValue((void*)graph));
        item_option->setData(color, Qt::BackgroundColorRole);
        item_graph_visible.append(item_title);
        item_graph_visible.append(item_option);
        data_series_root->appendRow(item_graph_visible);
        break;
    }

    return ui->plotwidget->graphCount();
}

void PlotWindow::OnConfigChanged(QStandardItem *item)
{
    //qDebug() << "PlotWindow::OnConfigChanged(" << item->whatsThis() << ")";

    if (item->whatsThis() == "x-Axis::AutoScroll") {
        _configOption.x_axis_auto_scroll = (item->checkState() == Qt::Checked ? true : false);

        const QStandardItem* item = nullptr;
        if (_configOption.x_axis_auto_scroll) {
            item = FindFirstConfigOptionItem("x-Axis", "Begin(s)");
            if (item) {
                const_cast<QStandardItem*>(item)->setEnabled(false);
            }
            item = FindFirstConfigOptionItem("x-Axis", "End(s)");
            if (item) {
                const_cast<QStandardItem*>(item)->setEnabled(false);
            }

            // AutoScroll on 시 여기서는 플래그만 셋하고, 실제 새로운 데이터 수신시에 그래프를 새로 그린다.
        }
        else {
            double ti = ui->plotwidget->xAxis->range().lower;
            double tf = ui->plotwidget->xAxis->range().upper;

            item = FindFirstConfigOptionItem("x-Axis", "Begin(s)");
            if (item) {
                const_cast<QStandardItem*>(item)->setEnabled(true);
                const_cast<QStandardItem*>(item)->setData(ti, Qt::EditRole);
            }
            item = FindFirstConfigOptionItem("x-Axis", "End(s)");
            if (item) {
                const_cast<QStandardItem*>(item)->setEnabled(true);
                const_cast<QStandardItem*>(item)->setData(tf, Qt::EditRole);
            }
        }
    }
    else if (item->whatsThis() == "x-Axis::AutoScrollWindow") {
        _configOption.x_axis_auto_scroll_window = item->data(Qt::EditRole).toDouble();
        // 여기서는 데이터만 셋하고, 실제 새로운 데이터 수신시에 그래프를 새로 그린다.
    }
    else if (item->whatsThis() == "x-Axis::Begin") {
        _configOption.x_axis_begin_sec = item->data(Qt::EditRole).toDouble();
        AdjustPlotXRange();
    }
    else if (item->whatsThis() == "x-Axis::End") {
        _configOption.x_axis_end_sec = item->data(Qt::EditRole).toDouble();
        AdjustPlotXRange();
    }
    else if (item->whatsThis() == "y-Axis::AutoScale") {
        _configOption.y_axis_auto_scale = (item->checkState() == Qt::Checked ? true : false);

        const QStandardItem* item = nullptr;
        if (_configOption.y_axis_auto_scale) {
            item = FindFirstConfigOptionItem("y-Axis", "Lower bound");
            if (item) {
                const_cast<QStandardItem*>(item)->setEnabled(false);
            }
            item = FindFirstConfigOptionItem("y-Axis", "Upper bound");
            if (item) {
                const_cast<QStandardItem*>(item)->setEnabled(false);
            }
            ui->plotwidget->yAxis->rescale(false);
            ui->plotwidget->replot();
        }
        else {
            double lb = ui->plotwidget->yAxis->range().lower;
            double ub = ui->plotwidget->yAxis->range().upper;

            item = FindFirstConfigOptionItem("y-Axis", "Lower bound");
            if (item) {
                const_cast<QStandardItem*>(item)->setEnabled(true);
                const_cast<QStandardItem*>(item)->setData(lb, Qt::EditRole);
            }
            item = FindFirstConfigOptionItem("y-Axis", "Upper bound");
            if (item) {
                const_cast<QStandardItem*>(item)->setEnabled(true);
                const_cast<QStandardItem*>(item)->setData(ub, Qt::EditRole);
            }
        }
    }
    else if (item->whatsThis() == "y-Axis::LBound") {
        _configOption.y_axis_lbound = item->data(Qt::EditRole).toDouble();
        AdjustPlotYRange();
    }
    else if (item->whatsThis() == "y-Axis::UBound") {
        _configOption.y_axis_ubound = item->data(Qt::EditRole).toDouble();
        AdjustPlotYRange();
    }
    else if (item->whatsThis() == "Legend::Visible") {
        _configOption.legend_visible = (item->checkState() == Qt::Checked ? true : false);
        ui->plotwidget->legend->setVisible(_configOption.legend_visible);
        ui->plotwidget->replot();
    }
    else if (item->whatsThis() == "Data series::Visible") {
        QCPGraph *graph = (QCPGraph*)item->data().value<void*>();
        if (graph) {
            graph->setVisible((item->checkState() == Qt::Checked ? true : false));
            ui->plotwidget->replot();
        }
    }
}

void PlotWindow::OnHorzScrollBarChanged(int value)
{
    //qDebug() << "OnHorzScrollBarChanged: value=" << value;
    if (qAbs(ui->plotwidget->xAxis->range().center()-value/100.0) > 0.01) // if user is dragging plot, we don't want to replot twice
    {
        ui->plotwidget->xAxis->setRange(value/100.0, ui->plotwidget->xAxis->range().size(), Qt::AlignCenter);
        ui->plotwidget->replot();
    }
}

void PlotWindow::OnVertScrollBarChanged(int value)
{
    //qDebug() << "OnVertScrollBarChanged: value=" << value;
    if (qAbs(ui->plotwidget->yAxis->range().center()+value/100.0) > 0.01) // if user is dragging plot, we don't want to replot twice
    {
        ui->plotwidget->yAxis->setRange(-value/100.0, ui->plotwidget->yAxis->range().size(), Qt::AlignCenter);
        ui->plotwidget->replot();
    }
}

void PlotWindow::OnXAxisRangeChanged(QCPRange range)
{
    //qDebug() << "OnXAxisRangeChanged: center=" << range.center() << ", size=" << range.size();
    ui->horizontalScrollBar->setValue(qRound(range.center()*100.0)); // adjust position of scroll bar slider
    ui->horizontalScrollBar->setPageStep(qRound(range.size()*100.0)); // adjust size of scroll bar slider
}

void PlotWindow::OnYAxisRangeChanged(QCPRange range)
{
    //qDebug() << "OnYAxisRangeChanged: center=" << range.center() << ", size=" << range.size();
    ui->verticalScrollBar->setValue(qRound(-range.center()*100.0)); // adjust position of scroll bar slider
    ui->verticalScrollBar->setPageStep(qRound(range.size()*100.0)); // adjust size of scroll bar slider
}

void PlotWindow::resizeEvent(QResizeEvent* event)
{
    //qDebug() << "Width : " << this->width() << ", Height : " <<  this->height();
    ui->plotwidget->setFixedSize(QSize(ui->centralwidget->width()-18, ui->centralwidget->height()-18));
    ui->horizontalScrollBar->setGeometry(0, ui->centralwidget->height()-18, ui->centralwidget->width()-18, 18);
    ui->verticalScrollBar->setGeometry(ui->centralwidget->width()-18, 0, 18, ui->centralwidget->height()-18);
    QMainWindow::resizeEvent(event);
}

void PlotWindow::AdjustPlotXRange(bool replot)
{
    if (!_configOption.x_axis_auto_scroll)
    {
        ui->plotwidget->xAxis->setRange(
            _configOption.x_axis_begin_sec,
            _configOption.x_axis_end_sec
            );
    }
    if (replot)
        ui->plotwidget->replot();
}

void PlotWindow::AdjustPlotYRange(bool replot)
{
    if (!_configOption.y_axis_auto_scale)
    {
        ui->plotwidget->yAxis->setRange(
            _configOption.y_axis_lbound,
            _configOption.y_axis_ubound
            );
    }
    if (replot)
        ui->plotwidget->replot();
}

const QStandardItem *PlotWindow::FindFirstConfigOptionItem(const QString &cat, const QString &item)
{
    auto items = _configModel->findItems(item, Qt::MatchExactly | Qt::MatchRecursive, 0);
    foreach (const QStandardItem* item, items) {
        if (item->parent() && item->parent()->text() == cat) {
            return item->parent()->child(item->row(), 1);
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
    //qDebug() << "PlotWindow::OnRefreshPlot";

    if (!_isNewDataReceived)
        return;

    _isNewDataReceived = false; // reset switch!

    double di = qMax(0.0, _lastRecvTime - _configOption.x_axis_auto_scroll_window);
    if (_configOption.x_axis_auto_scroll)
        ui->plotwidget->xAxis->setRange(di, _lastRecvTime);
    if (_configOption.y_axis_auto_scale)
        ui->plotwidget->yAxis->rescale(true);

    ui->plotwidget->replot();

    // update frame rate
    ui->statusbar->showMessage(
        QString("%1 FPS, Total Data received: %2")
            .arg(0.0, 0, 'f', 0)
            .arg(ui->plotwidget->graph(0)->data()->size()), 0);

    // adjust scroll-bars
    // 새로운 데이터가 들어오면, 새로운 데이터 시간을 기준으로 scroll-bar의 range를 변경
    ui->horizontalScrollBar->setRange(0,  (_lastRecvTime + 0.5 * ui->plotwidget->xAxis->range().size()) * 100);
}

#ifdef USE_EMUL_DATA
void PlotWindow::OnRecvEmul(const DataSeriesEmul& data)
{
    //qDebug() << "PlotWindow::OnRecvEmul";

    ui->plotwidget->graph(0)->addData(data.time, data.data[0]);
    ui->plotwidget->graph(1)->addData(data.time, data.data[1]);
    ui->plotwidget->graph(2)->addData(data.time, data.data[2]);
    ui->plotwidget->graph(3)->addData(data.time, data.data[3]);
    _lastRecvTime = data.time;
    _isNewDataReceived = true;
}
#endif

void PlotWindow::on_actionNew_triggered()
{
    ResetPlot();
}

