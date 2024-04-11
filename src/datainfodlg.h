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

private:
    Ui::DataInfoDlg *ui;
};

#endif // DATAINFODLG_H
