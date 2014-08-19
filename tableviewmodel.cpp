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

#include "tableviewmodel.h"
#include "debug.h"
#include "timetrackingmodel.h"

//#include <QtGui>
#include <QMessageBox>
#include <QTableWidgetSelectionRange>
#include <QDate>

#include <iostream>
#include <algorithm>
#include <assert.h>

using namespace std;

class TimeTrackingModel;
TableViewModel::TableViewModel(QWidget *parent)
    : QAbstractTableModel(parent),
      mColNameDay(tr("Day")),
      mColNameStartTime(tr("Start time")),
      mColNameEndTime(tr("End time")),
      mColNamePauseTime(tr("Pause")),
      mColNameWorkedTime(tr("Worked time")),
      mColNameRequiredTime(tr("Required time")),
      mColNameDueOverHours(tr("Difference")),
      mColNameComment(tr("Comment")),
      mSignalActDeactSemaphore(0),
      mParent(NULL),
      mParentWidget(parent),
      mDirty(false),
      mTimeTrackingModel(new TimeTrackingModel())
{
    mColumnsName << mColNameDay << mColNameStartTime << mColNameEndTime
                 << mColNamePauseTime << mColNameWorkedTime << mColNameRequiredTime
                 << mColNameDueOverHours << mColNameComment;

    mColumnsPattern << "ddd" << "HH:mm" << "HH:mm" << "HH:mm" << "HH:mm" << "HH:mm" << "HH:mm" << "%s";


     //setSelectionMode(ContiguousSelection);

}

// =============================================================================
Qt::ItemFlags TableViewModel::flags(const QModelIndex &index) const {
    Qt::ItemFlags theFlags = QAbstractTableModel::flags(index);
    // Day belongs to header
    int column = index.column();

    if(column == Col::WORKED_TIME   ||
       column == Col::DAY) {

        theFlags |= Qt::ItemIsSelectable | Qt::ItemIsEnabled;

    } else if(column == Col::START_TIME     ||
              column == Col::END_TIME       ||
              column == Col::PAUSE_TIME     ||
              column == Col::REQUIRED_TIME  ||
              column == Col::COMMENT) {

        theFlags |= Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable;
    }
    return theFlags;
}

// =============================================================================
QVariant TableViewModel::data(const QModelIndex &index,
              int role) const {
    int day = index.row() + 1;
    int column = index.column();
    try {
        if( role == Qt::DisplayRole || role == Qt::EditRole ) {
            // TODO: Replace try catch with error management
            if(column < Col::MAX_COL) {
                try {

                    switch(column) {
                    case Col::DAY:
                        return QString::number(day);

                    case Col::WORKED_TIME:
                        return mTimeTrackingModel->getWorkTime(day).
                                   toString(mColumnsPattern.value(Col::WORKED_TIME));

                    case Col::START_TIME:
                        return mTimeTrackingModel->getStartTime(day).
                                    toString(mColumnsPattern.value(Col::START_TIME));

                    case Col::PAUSE_TIME:
                        return mTimeTrackingModel->getPause(day).
                                toString(mColumnsPattern.value(Col::PAUSE_TIME));

                    case Col::REQUIRED_TIME:
                        return mTimeTrackingModel->getRequiredWorkTime(day).
                                toString(mColumnsPattern.value(Col::REQUIRED_TIME));

                    case Col::END_TIME:
                         return mTimeTrackingModel->getEndTime(day).
                                toString(mColumnsPattern.value(Col::END_TIME));

                    case Col::COMMENT:
                        return mTimeTrackingModel->getComment(day);

                    case Col::DUE_OVER_HOURS:
                        QString diff = mTimeTrackingModel->getTimeWorkedDiff(day).
                                toString(mColumnsPattern.value(Col::DUE_OVER_HOURS));
                        if(false == diff.isEmpty() && mTimeTrackingModel->timeWorkDiffNegativ(day)) {
                            diff = "-" + diff;
                        }
                        return diff;

                    }

                } catch (std::exception & e) {
                    QMessageBox::warning(mParentWidget, tr("Invalid time"),
                                         e.what());
                    return QVariant();
                }
            } else {
                QString name = mTimeTrackingModel->getDynamicFieldNameFromIndex(column);
                QDate date(mTimeTrackingModel->getBaseDate().year(),
                           mTimeTrackingModel->getBaseDate().month(),
                           day);
                return mTimeTrackingModel->getDynamicFieldSelection(name, date);
            }
        } else if(role == Qt::TextColorRole) {

            if(column == Col::DUE_OVER_HOURS) {
                if(mTimeTrackingModel->timeWorkDiffNegativ(day)) {
                    return QColor(200, 10, 10);
                } else {
                    return QColor(100, 100, 100);
                }
            } else if(column == Col::WORKED_TIME) {
                return QColor(100, 100, 100);
            }

        } else if(role == Qt::TextAlignmentRole) {

            return Qt::AlignCenter;

        } else if(role == Qt::UserRole + USER_ROLE_GET_MONTH_OF_SHEET) {
            return mTimeTrackingModel->getBaseDate().toString("MMMM");

        } else if(role == Qt::UserRole + USER_ROLE_START_TIME_IS_VALID) {
            return mTimeTrackingModel->startTimeIsValid();

        } else if(role == Qt::UserRole + USER_ROLE_END_TIME_IS_VALID) {
            return mTimeTrackingModel->endTimeIsValid();
        }
    } catch (std::exception & e) {
         QMessageBox::warning(mParentWidget, tr("Problem accessing tracking time"),
                             e.what());
         return QVariant();

    }


    return QVariant();
}

// =============================================================================
QVariant TableViewModel::headerData(int section, Qt::Orientation orientation,
                    int role) const {

    if(role == Qt::DisplayRole) {
        QString value;
        if(orientation == Qt::Horizontal) {
            if(section < Col::MAX_COL)  value = mColumnsName[section];
            else                        value = mTimeTrackingModel->getDynamicFieldNameFromIndex(section);
        } else {
            value = formatString(mTimeTrackingModel->getDayName(section + 1));
        }
        return value;

    } else if(role == Qt::FontRole && orientation == Qt::Vertical) {

        QFont tmpFont = QAbstractTableModel::headerData(section, orientation,role).value<QFont>();
        if(!mTimeTrackingModel->isWorkingDay(section + 1)) tmpFont.setBold(true);
        return tmpFont;

    } else if(role == Qt::TextColorRole && orientation == Qt::Vertical) {

        QColor color = QAbstractTableModel::headerData(section, orientation,role).value<QColor>();
        if(!mTimeTrackingModel->isWorkingDay(section + 1)) color = QColor(200, 10, 10);
        return color;
    }

    return QVariant();
}

// =============================================================================
int TableViewModel::rowCount(const QModelIndex &) const {
    return mTimeTrackingModel->daysInMonth();
}

// =============================================================================
int TableViewModel::columnCount(const QModelIndex &) const {
    return Col::MAX_COL + mTimeTrackingModel->getDynamicFieldCount();
}

// =============================================================================
bool TableViewModel::setData(const QModelIndex &index, const QVariant &value,
             int role) {
    int day = index.row() + 1;
    int column = index.column();

    if( role == Qt::EditRole ) {
        try {
            switch(column) {
            case Col::START_TIME:
                    mTimeTrackingModel->setStartTime(               day, value.toTime(), true /* changeStartTimeOnly */);
                    break;

            case Col::PAUSE_TIME:
                     mTimeTrackingModel->setPause(                  day, value.toTime());
                     break;

            case Col::REQUIRED_TIME:
                     mTimeTrackingModel->setRequiredWorkTime(       day, value.toTime());
                     break;

            case Col::END_TIME:
                      mTimeTrackingModel->setEndTime(               day, value.toTime());
                      break;

            case Col::COMMENT:
                      mTimeTrackingModel->setComment(               day, value.toString());
                      break;

            }
        } catch (std::exception& e) {
             QMessageBox::warning(mParentWidget, tr("Problem accessing tracking time"),
                                 e.what());

        }

    } else if(role == Qt::UserRole + USER_ROLE_FORCE_CHANGES) {
        switch(column) {
        case Col::START_TIME:
                mTimeTrackingModel->setStartTime(               day, value.toTime(), false /* changeStartTimeOnly */);
                break;
        default:
            assert(false);
        }

    }

    return false;

}

// =============================================================================
QString TableViewModel::formatString(const QString& string) const {
    return string.mid(0, 1).toUpper() + string.mid(1).toLower();
}

// =============================================================================
bool TableViewModel::sheetOfCurMonth() const {
    return mTimeTrackingModel->isModelOfCurMonth();
}

// =============================================================================
void TableViewModel::setUserName(const QString &userName) {
    mTimeTrackingModel->setUserName(Misc::getUserName());
}

// =============================================================================
void TableViewModel::setNewBaseDate(const QDate &baseDate) {
    mTimeTrackingModel->setNewBaseDate(baseDate);
}


