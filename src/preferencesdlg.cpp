#include "preferencesdlg.h"
#include "ui_preferencesdlg.h"
#include <QDebug>

PreferencesDlg::PreferencesDlg(QWidget *parent) : QDialog(parent),
                                                  ui(new Ui::PreferencesDlg)
{
    ui->setupUi(this);
}

PreferencesDlg::~PreferencesDlg()
{
    delete ui;
}
