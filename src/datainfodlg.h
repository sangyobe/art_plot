#ifndef DATAINFODLG_H
#define DATAINFODLG_H

#include <QDialog>

class QCPGraph;

namespace Ui {
class DataInfoDlg;
}

class DataInfoDlg : public QDialog
{
    Q_OBJECT

public:
    explicit DataInfoDlg(const QCPGraph* graph, QWidget *parent = nullptr);
    ~DataInfoDlg();

public:
    void Recalc(const QCPGraph *graph);

protected slots:
    void OnBtnRecalcClicked();
    void OnXRangeLimitEnabled(int state);
    void OnYRangeLimitEnabled(int state);

private:
    Ui::DataInfoDlg *ui;
    const QCPGraph *_graph;
};

#endif // DATAINFODLG_H
