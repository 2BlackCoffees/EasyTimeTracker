/*
 *  This file is part of EasyTimeTracker.
 *
 *  EasyTimeTracker is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  EasyTimeTracker is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *  You should have received a copy of the GNU General Public License
 *  along with EasyTimeTracker.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  EasyTimeTracker: First creation by 2BlackCoffees: http://www.twoblackcoffees.com/
 *
 **/

#include "combodelegate.h"
#include "debug.h"
#include "tablemodeladapter.h"

#include <QComboBox>
#include <QTimeEdit>

// =============================================================================
ComboDelegate::ComboDelegate(QObject * parent) : QItemDelegate(parent)
{
}

// =============================================================================
void ComboDelegate::paint(QPainter *painter,
                          const QStyleOptionViewItem &option,
                          const QModelIndex &index) const
{
    if (index.column() == TableModelAdapter::COL_WORKING_HOURS ||
            index.column() == TableModelAdapter::COL_PAUSE) {

        QString text = index.model()->data(index, Qt::DisplayRole).toTime().toString("hh:mm");

        QStyleOptionViewItem myOption = option;
        myOption.displayAlignment = Qt::AlignCenter | Qt::AlignVCenter;

        drawDisplay(painter, myOption, myOption.rect, text);
        drawFocus(painter, myOption, myOption.rect);

    } else if (index.column() == TableModelAdapter::COL_WORKING_DAY) {

        bool state = index.model()->data(index, Qt::DisplayRole).toBool();
        QString text = "No";
        if(state) {
            text = "Yes";
        }

        QStyleOptionViewItem curOption = option;
        curOption.displayAlignment = Qt::AlignCenter | Qt::AlignVCenter;
        drawDisplay(painter, curOption, curOption.rect, text);
        drawFocus(painter, curOption, curOption.rect);

    } else {
        QItemDelegate::paint(painter, option, index);
    }
}

// =============================================================================
QWidget *ComboDelegate::createEditor(QWidget *parent,
                                     const QStyleOptionViewItem &option,
                                     const QModelIndex &index) const
{

    if (index.column() == TableModelAdapter::COL_WORKING_HOURS ||
            index.column() == TableModelAdapter::COL_PAUSE) {

        QTimeEdit *timeEdit = new QTimeEdit(parent);
        timeEdit->setDisplayFormat("hh:mm");
        connect(timeEdit, SIGNAL(editingFinished()),
                this, SLOT(commitAndCloseTimeEdit()));
        return timeEdit;

    } else if (index.column() == TableModelAdapter::COL_WORKING_DAY) {

        QComboBox *comboBox = new QComboBox(parent);
        comboBox->addItem("Yes");
        comboBox->addItem("No");
        connect(comboBox, SIGNAL(editingFinished()),
                this, SLOT(commitAndCloseCombo()));
        return comboBox;

    } else {
        return QItemDelegate::createEditor(parent, option, index);
    }
}

// =============================================================================
void ComboDelegate::setEditorData(QWidget *editor,
                                  const QModelIndex &index) const
{

    if (index.column() == TableModelAdapter::COL_WORKING_HOURS ||
            index.column() == TableModelAdapter::COL_PAUSE) {

        QTime time = index.model()->data(index, Qt::DisplayRole).toTime();
        QTimeEdit *timeEdit = qobject_cast<QTimeEdit *>(editor);
        timeEdit->setTime(time);

    } else if (index.column() == TableModelAdapter::COL_WORKING_DAY) {

        bool state = index.model()->data(index, Qt::DisplayRole).toBool();
        QComboBox *comboBox = qobject_cast<QComboBox *>(editor);
        if(state) {
            comboBox->setCurrentIndex(0);
        } else {
            comboBox->setCurrentIndex(1);
        }

    } else {
        QItemDelegate::setEditorData(editor, index);
    }
}

// =============================================================================
void ComboDelegate::setModelData(QWidget *editor,
                                 QAbstractItemModel *model,
                                 const QModelIndex &index) const
{

    if (index.column() == TableModelAdapter::COL_WORKING_HOURS ||
            index.column() == TableModelAdapter::COL_PAUSE) {

        QTimeEdit *timeEdit = qobject_cast<QTimeEdit *>(editor);
        QTime time = timeEdit->time();
        model->setData(index, time);

    } else if (index.column() == TableModelAdapter::COL_WORKING_DAY) {

        QComboBox *comboBox = qobject_cast<QComboBox *>(editor);
        if(comboBox->currentText() == "Yes") {
            model->setData(index, true);
        } else {
            model->setData(index, false);
        }

    } else {
        QItemDelegate::setModelData(editor, model, index);
    }
}

// =============================================================================
void ComboDelegate::commitAndCloseTimeEdit()
{

    QTimeEdit *editor = qobject_cast<QTimeEdit *>(sender());
    emit commitData(editor);
    emit closeEditor(editor);
}

// =============================================================================
void ComboDelegate::commitAndCloseCombo()
{

    QComboBox *editor = qobject_cast<QComboBox *>(sender());
    emit commitData(editor);
    emit closeEditor(editor);
}

