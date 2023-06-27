#include "plotconfig.h"
#include "plotwindow.h"
#include "ui_plotwindow.h"
#include "recvthreadecal.h"
#include "recvthreademul.h"
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
    y_axis_ubound(1.0)
{}

PlotWindow::PlotWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::PlotWindow),
    _configModel(nullptr),
    _recvThread(new RecvThreadEmul(this))

{
    ui->setupUi(this);

    _plotConfig = new PlotConfig(this);
    _plotConfig->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    addDockWidget(Qt::LeftDockWidgetArea, _plotConfig);

    BuildConfig();




    connect(_recvThread.get(), SIGNAL(add_data_refresh_plot(DataSeriesEmul*, uint32_t)), this, SLOT(OnNewDataArrived(DataSeriesEmul*, uint32_t)));


    this->AddGraph("Sin", QColorConstants::DarkCyan);
    this->AddGraph("Sin^2", QColorConstants::DarkYellow);
    this->AddGraph("Cos", QColorConstants::Yellow);
    this->AddGraph("Cos^2", QColorConstants::Magenta);


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
    // set range
    //
    ui->plotwidget->xAxis->setRange(
        _configOption.x_axis_begin_sec,
        _configOption.x_axis_end_sec);
    ui->plotwidget->yAxis->setRange(
        _configOption.y_axis_lbound,
        _configOption.y_axis_ubound);
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
    ui->plotwidget->setInteraction(QCP::iSelectPlottables);
    ui->plotwidget->setInteraction(QCP::iMultiSelect, true);
    ui->plotwidget->setMultiSelectModifier(Qt::ControlModifier);

    ui->plotwidget->replot();



    //_recvThread = std::make_unique<RecvThread>(this);
    //_recvThread = new RecvThreadECal(this);
    _recvThread->start();
}

PlotWindow::~PlotWindow()
{
    //_recvThread->exit(0);
    _recvThread->terminate();
    _recvThread->wait();

    delete ui;
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
    QStandardItem* data_series_root = new QStandardItem("Data series");
    _configModel->appendRow(x_axis_root);
    _configModel->appendRow(y_axis_root);
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

void PlotWindow::OnNewDataArrived(DataSeriesEmul* data, uint32_t size)
{
    //qDebug() << "OnNewDataArrived";

    static double di = 0.0;
    static double df = 1.1;
    //static const double dt = 0.1;

    for (size_t i=0; i<size; i++) {
        ui->plotwidget->graph(0)->addData(data[i].time, data[i].data[0]);
        ui->plotwidget->graph(1)->addData(data[i].time, data[i].data[1]);
        ui->plotwidget->graph(2)->addData(data[i].time, data[i].data[2]);
        ui->plotwidget->graph(3)->addData(data[i].time, data[i].data[3]);
        df = data[i].time;
    }
    di = qMax(0.0, df - _configOption.x_axis_auto_scroll_window);
    if (_configOption.x_axis_auto_scroll)
        ui->plotwidget->xAxis->setRange(di, df);
    if (_configOption.y_axis_auto_scale)
        ui->plotwidget->yAxis->rescale();

    ui->plotwidget->replot();
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
