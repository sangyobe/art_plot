#ifndef PLOTCENTRALWIDGET_H
#define PLOTCENTRALWIDGET_H

#include <QWidget>

class PlotCentralWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PlotCentralWidget(QWidget *parent = nullptr);

signals:

protected:
    void resizeEvent(QResizeEvent* event);
};

#endif // PLOTCENTRALWIDGET_H
