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

    _plotConfig = new PlotConfig(this);
    _plotConfig->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    addDockWidget(Qt::LeftDockWidgetArea, _plotConfig);

    BuildConfig();


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
    ui->plotwidget->axisRect(0)->setRangeDrag(/*Qt::Vertical | */Qt::Horizontal);
    // enable zoom
    ui->plotwidget->setInteraction(QCP::iRangeZoom, true);
    ui->plotwidget->axisRect(0)->setRangeZoom(Qt::Vertical | Qt::Horizontal);
    ui->plotwidget->axisRect(0)->setRangeZoomFactor(1.1, 1.1);
    // enable clicking items
//    ui->plotwidget->setInteraction(QCP::iSelectPlottables);
//    ui->plotwidget->setInteraction(QCP::iMultiSelect, true);
//    ui->plotwidget->setMultiSelectModifier(Qt::ControlModifier);

    ResetPlot();
    //ui->plotwidget->replot();


    // initialize the plot refresh timer
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

void PlotWindow::SetWindowTitle(const QString &title)
{
    setWindowTitle(title);
}

void PlotWindow::AutoScroll(bool on)
{
    _configOption.x_axis_auto_scroll = on;
}

void PlotWindow::AutoScrollWindow(double dt_sec)
{
    _configOption.x_axis_auto_scroll_window = dt_sec;
}

void PlotWindow::AutoScale(bool on)
{
    _configOption.y_axis_auto_scale = on;
}

void PlotWindow::DataUpdated(double recv_time)
{
    _lastRecvTime = qMax(_lastRecvTime.load(), recv_time);
    _isNewDataReceived = true;
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
    return ui->plotwidget->graphCount();
}

void PlotWindow::OnConfigChanged(QStandardItem *item)
{
    qDebug() << "PlotWindow::OnConfigChanged(" << item->whatsThis() << ")";

    if (item->whatsThis() == "x-Axis::AutoScroll") {
        _configOption.x_axis_auto_scroll = (item->checkState() == Qt::Checked ? true : false);
    }
    else if (item->whatsThis() == "x-Axis::AutoScrollWindow") {
        _configOption.x_axis_auto_scroll_window = item->text().toDouble();
    }
    else if (item->whatsThis() == "x-Axis::Begin") {
        _configOption.x_axis_begin_sec = item->text().toDouble();
        AdjustPlotXRange();
    }
    else if (item->whatsThis() == "x-Axis::End") {
        _configOption.x_axis_end_sec = item->text().toDouble();
        AdjustPlotXRange();
    }
    else if (item->whatsThis() == "y-Axis::AutoScale") {
        _configOption.y_axis_auto_scale = (item->checkState() == Qt::Checked ? true : false);
    }
    else if (item->whatsThis() == "y-Axis::LBound") {
        _configOption.y_axis_lbound = item->text().toDouble();
        AdjustPlotYRange();
    }
    else if (item->whatsThis() == "y-Axis::UBound") {
        _configOption.y_axis_ubound = item->text().toDouble();
        AdjustPlotYRange();
    }
    else if (item->whatsThis() == "Legend::Visible") {
        _configOption.legend_visible = (item->checkState() == Qt::Checked ? true : false);
        ui->plotwidget->legend->setVisible(_configOption.legend_visible);
    }
}

void PlotWindow::resizeEvent(QResizeEvent* event)
{
    qDebug() << "Width : " << this->width() << ", Height : " <<  this->height();
    ui->plotwidget->setFixedSize(QSize(ui->centralwidget->width(), ui->centralwidget->height()));
    QMainWindow::resizeEvent(event);
}

void PlotWindow::AdjustPlotXRange()
{
    if (!_configOption.x_axis_auto_scroll)
    {
        ui->plotwidget->xAxis->setRange(
            _configOption.x_axis_begin_sec,
            _configOption.x_axis_end_sec
            );
    }
}

void PlotWindow::AdjustPlotYRange()
{
    if (!_configOption.y_axis_auto_scale)
    {
        ui->plotwidget->yAxis->setRange(
            _configOption.y_axis_lbound,
            _configOption.y_axis_ubound
            );
    }
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
        ui->plotwidget->yAxis->rescale();

    ui->plotwidget->replot();
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

