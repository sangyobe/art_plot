#ifndef CSVOPTIONDLG_H
#define CSVOPTIONDLG_H

#include <QDialog>

namespace Ui
{
class CSVOptionDlg;
}

class CSVOptionDlg : public QDialog
{
    Q_OBJECT

public:
    explicit CSVOptionDlg(QWidget *parent = nullptr);
    ~CSVOptionDlg();

private:
    Ui::CSVOptionDlg *ui;
};

#endif // CSVOPTIONDLG_H
