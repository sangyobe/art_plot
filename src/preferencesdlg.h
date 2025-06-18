#ifndef PREFERENCESDLG_H
#define PREFERENCESDLG_H

#include <QDialog>

namespace Ui
{
class PreferencesDlg;
}

class PreferencesDlg : public QDialog
{
    Q_OBJECT

public:
    explicit PreferencesDlg(QWidget *parent = nullptr);
    ~PreferencesDlg();

private:
    Ui::PreferencesDlg *ui;
};

#endif // PREFERENCESDLG_H
