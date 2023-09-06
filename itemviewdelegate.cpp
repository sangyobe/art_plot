#include "itemviewdelegate.h"

#include <QComboBox>
#include <QStylePainter>
#include <QApplication>
#include <QStyleOption>
#include <QStyle>
#include <QDebug>

ItemViewDelegate::ItemViewDelegate(QObject *parent) : QStyledItemDelegate(parent)
{
}

QWidget *ItemViewDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (!index.data(static_cast<int>(Role::ComboBox)).isNull()) {
        return new QComboBox(parent);
    }
    else
        return QStyledItemDelegate::createEditor(parent, option, index);
}

void ItemViewDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    if (!index.data(static_cast<int>(Role::ComboBox)).isNull()) {
        QComboBox* comboBox = qobject_cast<QComboBox*>(editor);
        if (comboBox) {
            QStringList options = index.data(static_cast<int>(Role::ComboBox)).toStringList();
            comboBox->addItems(options);
            QString value = index.data().toString();
            int current = options.indexOf(value);
            if (current > -1) {
                comboBox->setCurrentIndex(current);
            }
            comboBox->showPopup();
        }
        return;
    }

    QStyledItemDelegate::setEditorData(editor, index);
}

void ItemViewDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    if (!index.data(static_cast<int>(Role::ComboBox)).isNull()) {
        QComboBox* comboBox = qobject_cast<QComboBox*>(editor);
        if (comboBox) {
            model->setData(index, comboBox->currentText());
            return;
        }
    }

    QStyledItemDelegate::setModelData(editor, model, index);
}

void ItemViewDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (!index.data(static_cast<int>(Role::ComboBox)).isNull()) {
        QStyle* style = qApp->style();
        QStyleOptionComboBox opt;
        opt.rect = option.rect;
        opt.currentText = index.data().toString();
        opt.palette = option.palette;
        opt.state = option.state;
        opt.subControls = QStyle::SC_All;
        opt.activeSubControls = QStyle::SC_All;
        opt.editable = false;
        opt.frame = true;
        style->drawComplexControl(QStyle::CC_ComboBox, &opt, painter, 0);
        style->drawControl(QStyle::CE_ComboBoxLabel, &opt, painter, 0);
        return;
    }

    QStyledItemDelegate::paint(painter, option, index);
}
