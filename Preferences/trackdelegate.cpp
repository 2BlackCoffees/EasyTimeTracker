#include <QtGui>

#include "trackdelegate.h"
#include "tablemodeladapter.h"

TrackDelegate::TrackDelegate(int durationColumn, QObject *parent)
    : QItemDelegate(parent)
{
    this->durationColumn = durationColumn;
}

void TrackDelegate::paint(QPainter *painter,
                          const QStyleOptionViewItem &option,
                          const QModelIndex &index) const
{
    DEBUG_OBJ("");

    if (index.column() == TableModelAdapter::COL_WORKING_HOURS ||
            index.column() == TableModelAdapter::COL_PAUSE) {
        int secs = index.model()->data(index, Qt::DisplayRole).toInt();
        QString text = QString("%1:%2")
                .arg(secs / 60, 2, 10, QChar('0'))
                .arg(secs % 60, 2, 10, QChar('0'));

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

QWidget *TrackDelegate::createEditor(QWidget *parent,
                                     const QStyleOptionViewItem &option,
                                     const QModelIndex &index) const
{
    DEBUG_OBJ("");
    if (index.column() == TableModelAdapter::COL_WORKING_HOURS ||
            index.column() == TableModelAdapter::COL_PAUSE) {

        QTimeEdit *timeEdit = new QTimeEdit(parent);
        timeEdit->setDisplayFormat("mm:ss");
        connect(timeEdit, SIGNAL(editingFinished()),
                this, SLOT(commitAndCloseEditor()));
        return timeEdit;

    } else if (index.column() == TableModelAdapter::COL_WORKING_DAY) {

        QComboBox *comboBox = new QComboBox(parent);
        comboBox->addItem("Yes");
        comboBox->addItem("No");
        connect(comboBox, SIGNAL(editingFinished()),
                this, SLOT(commitAndCloseEditor()));
        return comboBox;

    } else {
        return QItemDelegate::createEditor(parent, option, index);
    }
}

void TrackDelegate::setEditorData(QWidget *editor,
                                  const QModelIndex &index) const
{
    DEBUG_OBJ("");

    if (index.column() == TableModelAdapter::COL_WORKING_HOURS ||
            index.column() == TableModelAdapter::COL_PAUSE) {

        int secs = index.model()->data(index, Qt::DisplayRole).toInt();
        QTimeEdit *timeEdit = qobject_cast<QTimeEdit *>(editor);
        timeEdit->setTime(QTime(0, secs / 60, secs % 60));

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

void TrackDelegate::setModelData(QWidget *editor,
                                 QAbstractItemModel *model,
                                 const QModelIndex &index) const
{
    DEBUG_OBJ("");
    if (index.column() == TableModelAdapter::COL_WORKING_HOURS ||
            index.column() == TableModelAdapter::COL_PAUSE) {

        QTimeEdit *timeEdit = qobject_cast<QTimeEdit *>(editor);
        QTime time = timeEdit->time();
        int secs = (time.minute() * 60) + time.second();
        model->setData(index, secs);

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

void TrackDelegate::commitAndCloseEditor()
{
    DEBUG_OBJ("");
    QTimeEdit *editor = qobject_cast<QTimeEdit *>(sender());
    emit commitData(editor);
    emit closeEditor(editor);
}
