#include "plotconfig.h"
#include "plotwindow.h"
#include "ui_plotwindow.h"
#include "recvthreadecal.h"
#include "recvthreademul.h"
#include <cassert>
#include <iostream>
#include <memory>

PlotWindow::PlotWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::PlotWindow),
    _recvThread(new RecvThreadEmul(this))
{
    ui->setupUi(this);

    _plotConfig = new PlotConfig(this);
    _plotConfig->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    addDockWidget(Qt::LeftDockWidgetArea, _plotConfig);




    connect(_recvThread.get(), SIGNAL(add_data_refresh_plot(DataSeriesEmul*, uint32_t)), this, SLOT(OnNewDataArrived(DataSeriesEmul*, uint32_t)));


    this->AddGraph("Sin");
    this->AddGraph("Sin^2");
    this->AddGraph("Cos");
    this->AddGraph("Cos^2");


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
    //ui->plotwidget->xAxis->setRange(-1, 1);
    //ui->plotwidget->yAxis->setRange(0, 1);
    ui->plotwidget->xAxis->setRange(-0.1, 1.1);
    ui->plotwidget->yAxis->setRange(-1, 6);
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

int PlotWindow::AddGraph(const QString &name)
{
    QCPGraph *graph = ui->plotwidget->addGraph();
    graph->setName(name);
    // line style
    graph->setLineStyle(QCPGraph::lsLine/*lsStepRight*/);
    //graph->setScatterStyle(QCPScatterStyle::ssPlus);
    return ui->plotwidget->graphCount();
}

void PlotWindow::OnNewDataArrived(DataSeriesEmul* data, uint32_t size)
{
    qDebug() << "OnNewDataArrived";

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
    //di += dt;
    //df += dt;

    ui->plotwidget->xAxis->setRange(di, df);
    ui->plotwidget->replot();
}

void PlotWindow::resizeEvent(QResizeEvent* event)
{
    qDebug() << "Width : " << this->width() << ", Height : " <<  this->height();
    ui->plotwidget->setFixedSize(QSize(ui->centralwidget->width(), ui->centralwidget->height()));
    QMainWindow::resizeEvent(event);
}
