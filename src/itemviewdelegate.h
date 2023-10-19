#ifndef ITEMVIEWDELEGATE_H
#define ITEMVIEWDELEGATE_H

#include <QStyledItemDelegate>

class ItemViewDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    enum class Role {
        ComboBox = Qt::UserRole+12,
    };
    explicit ItemViewDelegate(QObject *parent = nullptr);
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

#endif // ITEMVIEWDELEGATE_H
