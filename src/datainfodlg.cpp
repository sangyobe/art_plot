#include "datainfodlg.h"
#include "ui_datainfodlg.h"
#include "qcustomplot.h"
#include <QDebug>

DataInfoDlg::DataInfoDlg(const QCPGraph* graph, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DataInfoDlg)
{
    ui->setupUi(this);

    if (graph) {
        setWindowTitle(graph->name());


        int dnum = graph->dataCount();
        double sum = 0.0;
        double avg = 0.0;
        double std = 0.0;
        double ssum = 0.0;

        std::for_each(graph->data()->constBegin(), graph->data()->constEnd(),
            [&sum, &avg] (const QCPGraphData& element)
            {
                sum += element.value;
            });

        if (dnum > 0) {
            avg = sum / dnum;
        }

        std::for_each(graph->data()->constBegin(), graph->data()->constEnd(),
            [&sum, &avg, &ssum, &std] (const QCPGraphData& element)
            {
                ssum += (element.value - avg) * (element.value - avg);
            });

        if (dnum > 0) {
            std = ssum / dnum;
            std = sqrt(std);
        }

        QString out= QString("%1 : %2\n%3 : %4\n%5 : %6\n%7 : %8")
                            .arg("Count").arg(QString::number(dnum, 10))
                            .arg("Sum.").arg(QString::number(sum, 'f', 3))
                            .arg("Avg.").arg(QString::number(avg, 'f', 3))
                            .arg("Std dev.").arg(QString::number(std, 'f', 3));

        ui->output->setText(out);
    }
}

DataInfoDlg::~DataInfoDlg()
{
    delete ui;
}
