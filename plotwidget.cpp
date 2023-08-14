#include "plotwidget.h"
#include <QDebug>

PlotWidget::PlotWidget(QWidget *parent) :
  QCustomPlot(parent)
{
    //connect(this, SIGNAL(mouseMove(QMouseEvent*)), this, SLOT(ShowPointToolTip(QMouseEvent*)));
    connect(this, &QCustomPlot::mouseMove, this, &PlotWidget::ShowPointToolTip);
}

PlotWidget::~PlotWidget()
{
}

void PlotWidget::ShowPointToolTip(QMouseEvent *event)
{
    double x = this->xAxis->pixelToCoord(event->x());
    double y = this->yAxis->pixelToCoord(event->y());
    setToolTip(QString("%1, %2").arg(x).arg(y));
}

void PlotWidget::resizeEvent(QResizeEvent *event)
{
    qDebug() << "PlotWidget::resizeEvent (Width : " << this->width() << ", Height : " <<  this->height() << ")";
    QCustomPlot::resizeEvent(event);

}
