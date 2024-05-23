#include "csvoptiondlg.h"
#include "ui_csvoptiondlg.h"
#include <QDebug>

CSVOptionDlg::CSVOptionDlg(QWidget *parent) : QDialog(parent),
                                              ui(new Ui::CSVOptionDlg)
{
    ui->setupUi(this);
}

CSVOptionDlg::~CSVOptionDlg()
{
    delete ui;
}
