#include "plotcentralwidget.h"
#include "plotwindow.h"

PlotCentralWidget::PlotCentralWidget(QWidget *parent)
    : QWidget{parent}
{

}

void PlotCentralWidget::resizeEvent(QResizeEvent* event)
{
    static_cast<PlotWindow*>(parent())->RecalculatePlotLayout();
    QWidget::resizeEvent(event);
}
