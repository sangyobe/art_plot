#ifndef PLOTCONFIG_H
#define PLOTCONFIG_H

#include <QDockWidget>
#include <QTreeWidgetItem>
#include <QAbstractItemModel>

namespace Ui {
class PlotConfig;
}

class PlotConfig : public QDockWidget
{
    Q_OBJECT

public:
    explicit PlotConfig(QWidget *parent = nullptr);
    ~PlotConfig();

    void setConfigModel(QAbstractItemModel* configModel);
    void expandAll();
    void expand(const QModelIndex& index, int depth = -1);
    void collapseAll();
    void collapse(const QModelIndex& index);
    void selectItem(const QModelIndex& index);
    void clearSelection();

private slots:
    void OnItemClicked(const QModelIndex& index);
    void OnItemDoubleClicked(const QModelIndex& index);

signals:
    void graphItemClicked(QString name, int index);
    void graphColorSelected(QString name, int index, QColor color);

private:
    Ui::PlotConfig *ui;
};

#endif // PLOTCONFIG_H
