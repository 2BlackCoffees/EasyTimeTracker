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

#include "debug.h"
#include "tableviewdelegate.h"
#include "tableviewmodel.h"

#include <QComboBox>
#include <QTimeEdit>

TableViewDelegate::TableViewDelegate(QObject * parent) : QItemDelegate(parent)
{
}

// =============================================================================
void TableViewDelegate::paint(QPainter *painter,
                          const QStyleOptionViewItem &option,
                          const QModelIndex &index) const
{
    int column = index.column();

    if (column == Col::START_TIME   ||
        column == Col::END_TIME     ||
        column == Col::PAUSE_TIME   ||
        //column == Col::WORKED_TIME  ||
        column == Col::REQUIRED_TIME) {

        QString text = index.model()->data(index, Qt::DisplayRole).toTime().toString("hh:mm");

        QStyleOptionViewItem myOption = option;
        myOption.displayAlignment = Qt::AlignCenter | Qt::AlignVCenter;

        drawDisplay(painter, myOption, myOption.rect, text);
        drawFocus(painter, myOption, myOption.rect);

    } else {
        QItemDelegate::paint(painter, option, index);
    }
}

// =============================================================================
QWidget *TableViewDelegate::createEditor(QWidget *parent,
                                     const QStyleOptionViewItem &option,
                                     const QModelIndex &index) const
{

    // Day belongs to header
    int column = index.column();

    if (column == Col::START_TIME   ||
        column == Col::END_TIME     ||
        column == Col::PAUSE_TIME   ||
        column == Col::REQUIRED_TIME) {

        QTimeEdit *timeEdit = new QTimeEdit(parent);
        timeEdit->setDisplayFormat("hh:mm");
        connect(timeEdit, SIGNAL(editingFinished()),
                this, SLOT(commitAndCloseTimeEdit()));
        return timeEdit;

    } else {
        return QItemDelegate::createEditor(parent, option, index);
    }
}

// =============================================================================
void TableViewDelegate::setEditorData(QWidget *editor,
                                  const QModelIndex &index) const
{

    // Day belongs to header
    int column = index.column();

    if (column == Col::START_TIME   ||
        column == Col::END_TIME     ||
        column == Col::PAUSE_TIME   ||
        column == Col::REQUIRED_TIME) {

        QTime time = index.model()->data(index, Qt::DisplayRole).toTime();
        QTimeEdit *timeEdit = qobject_cast<QTimeEdit *>(editor);
        timeEdit->setTime(time);

    } else {
        QItemDelegate::setEditorData(editor, index);
    }
}

// =============================================================================
void TableViewDelegate::setModelData(QWidget *editor,
                                 QAbstractItemModel *model,
                                 const QModelIndex &index) const
{
    // Day belongs to header
    int column = index.column();

    if (column == Col::START_TIME   ||
        column == Col::END_TIME     ||
        column == Col::PAUSE_TIME   ||
        column == Col::REQUIRED_TIME) {

        QTimeEdit *timeEdit = qobject_cast<QTimeEdit *>(editor);
        QTime time = timeEdit->time();
        model->setData(index, time);

    } else {
        QItemDelegate::setModelData(editor, model, index);
    }
}

// =============================================================================
void TableViewDelegate::commitAndCloseTimeEdit()
{

    QTimeEdit *editor = qobject_cast<QTimeEdit *>(sender());
    emit commitData(editor);
    emit closeEditor(editor);
}

