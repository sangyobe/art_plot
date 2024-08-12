#include "datainfodlg.h"
#include "qcustomplot.h"
#include "ui_datainfodlg.h"
#include <QDebug>
#include <limits>

DataInfoDlg::DataInfoDlg(const QCPGraph *graph, QWidget *parent) : QDialog(parent),
                                                                   ui(new Ui::DataInfoDlg), _graph(graph)
{
    ui->setupUi(this);

    //connect(ui->btnRecal, SIGNAL(clicked()), this, SLOT(OnBtnRecalcClicked()));
    connect(ui->chkLimitX, SIGNAL(stateChanged(int)), this, SLOT(OnXRangeLimitEnabled(int)));
    connect(ui->chkLimitY, SIGNAL(stateChanged(int)), this, SLOT(OnYRangeLimitEnabled(int)));
    connect(ui->buttonBox->button(QDialogButtonBox::Apply), SIGNAL(clicked()), this, SLOT(OnBtnRecalcClicked()));

    ui->chkLimitX->setChecked(false);
    ui->editXRangeMin->setEnabled(false);
    ui->editXRangeMax->setEnabled(false);

    ui->chkLimitY->setChecked(false);
    ui->editYRangeMin->setEnabled(false);
    ui->editYRangeMax->setEnabled(false);

    ui->buttonBox->button(QDialogButtonBox::Apply)->setDefault(true);

    if (graph)
    {
        setWindowTitle(graph->name());

        Recalc(graph);

        // initialize x-range once.
        QCPRange xRange;
        bool foundXRange;
        xRange = graph->getKeyRange(foundXRange);
        if (foundXRange)
        {
            ui->editXRangeMin->setText(QString::number(xRange.lower));
            ui->editXRangeMax->setText(QString::number(xRange.upper));
        }
        else
        {
            ui->editXRangeMin->setText("0");
            ui->editXRangeMax->setText("0");
        }
        // initialize y-range once.
        QCPRange yRange;
        bool foundYRange;
        yRange = graph->getValueRange(foundYRange);
        if (foundYRange)
        {
            ui->editYRangeMin->setText(QString::number(yRange.lower));
            ui->editYRangeMax->setText(QString::number(yRange.upper));
        }
        else
        {
            ui->editYRangeMin->setText("0");
            ui->editYRangeMax->setText("0");
        }
    }
}

DataInfoDlg::~DataInfoDlg()
{
    delete ui;
}

void DataInfoDlg::Recalc(const QCPGraph *graph)
{
    int dnum = 0; //graph->dataCount();
    double x_range_min = -std::numeric_limits<double>::max();
    double x_range_max = std::numeric_limits<double>::max();
    double y_range_min = -std::numeric_limits<double>::max();
    double y_range_max = std::numeric_limits<double>::max();
    double sum = 0.0;
    double avg = 0.0;
    double std = 0.0;
    double ssum = 0.0;
    double min_key = 0;
    double max_key = 0;
    double min_val = std::numeric_limits<double>::max();
    double max_val = -std::numeric_limits<double>::max();

    if (ui->chkLimitX->checkState() == Qt::CheckState::Checked)
    {
        x_range_min = ui->editXRangeMin->text().toDouble();
        x_range_max = ui->editXRangeMax->text().toDouble();
    }
    if (ui->chkLimitY->checkState() == Qt::CheckState::Checked)
    {
        y_range_min = ui->editYRangeMin->text().toDouble();
        y_range_max = ui->editYRangeMax->text().toDouble();
    }

    std::for_each(graph->data()->constBegin(), graph->data()->constEnd(),
                  [&](const QCPGraphData &element) {
                      if (x_range_min > element.key) return;
                      if (x_range_max < element.key) return;
                      if (y_range_min > element.value) return;
                      if (y_range_max < element.value) return;

                      sum += element.value;
                      if (element.value < min_val)
                      {
                          min_key = element.key;
                          min_val = element.value;
                      }
                      if (element.value > max_val)
                      {
                          max_key = element.key;
                          max_val = element.value;
                      }
                      dnum++;
                  });

    if (dnum > 0)
    {
        avg = sum / dnum;
    }

    std::for_each(graph->data()->constBegin(), graph->data()->constEnd(),
                  [&](const QCPGraphData &element) {
                      if (x_range_min > element.key) return;
                      if (x_range_max < element.key) return;
                      if (y_range_min > element.value) return;
                      if (y_range_max < element.value) return;
                      ssum += (element.value - avg) * (element.value - avg);
                  });

    if (dnum > 0)
    {
        std = ssum / dnum;
        std = sqrt(std);
    }

    QString out = QString("%1 : %2\n%3 : %4 (@%5)\n%6 : %7 (@%8)\n%9 : %10\n%11 : %12\n%13 : %14")
                      .arg("Count")
                      .arg(QString::number(dnum, 10))
                      .arg("Min")
                      .arg(QString::number(min_val, 'f', 3))
                      .arg(QString::number(min_key, 'f', 3))
                      .arg("Max")
                      .arg(QString::number(max_val, 'f', 3))
                      .arg(QString::number(max_key, 'f', 3))
                      .arg("Sum.")
                      .arg(QString::number(sum, 'f', 3))
                      .arg("Avg.")
                      .arg(QString::number(avg, 'f', 3))
                      .arg("Std dev.")
                      .arg(QString::number(std, 'f', 3));

    ui->output->setText(out);
}

void DataInfoDlg::OnBtnRecalcClicked()
{
    // qDebug() << "OnBtnRecalcClicked";
    Recalc(this->_graph);
}

void DataInfoDlg::OnXRangeLimitEnabled(int state)
{
    ui->editXRangeMin->setEnabled(ui->chkLimitX->checkState() == Qt::CheckState::Checked);
    ui->editXRangeMax->setEnabled(ui->chkLimitX->checkState() == Qt::CheckState::Checked);
}

void DataInfoDlg::OnYRangeLimitEnabled(int state)
{
    ui->editYRangeMin->setEnabled(ui->chkLimitY->checkState() == Qt::CheckState::Checked);
    ui->editYRangeMax->setEnabled(ui->chkLimitY->checkState() == Qt::CheckState::Checked);
}