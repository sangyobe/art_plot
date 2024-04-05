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
    void OnContextMenu(const QPoint&);
    void RestoreName();
    void RestoreNameAll();

signals:
    void graphItemClicked(QString name, int index);
    void graphColorSelected(QString name, int index, QColor color);
    void graphRestoreNameActionSelected(QString name, int index);

private:
    Ui::PlotConfig *ui;
};

#endif // PLOTCONFIG_H
