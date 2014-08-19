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

#include "tablemodel.h"
#include "tableviewmodel.h"
#include "debug.h"

//#include <QtGui>
#include <QMessageBox>

#include <iostream>
using namespace std;

class TimeTrackingModel;
TableModel::TableModel(QWidget *parent)
    : mTableViewModel(new TableViewModel(parent))

{

}

// =============================================================================
void TableModel::startTT() {

    QModelIndex cellIndex = mTableViewModel->getIndexOfToday(Col::START_TIME);
    mTableViewModel->setData(cellIndex, QTime::currentTime(),
                             Qt::UserRole + TableViewModel::USER_ROLE_FORCE_CHANGES);
    emit mTableViewModel->dataChanged(mTableViewModel->getIndexOfToday(Col::DAY),
                                      mTableViewModel->getIndexOfToday(Col::MAX_COL));
}

// =============================================================================
void TableModel::stopTT() {
    QModelIndex cellIndex = mTableViewModel->getIndexOfToday(Col::END_TIME);
    mTableViewModel->setData(cellIndex, QTime::currentTime(), Qt::EditRole);
    emit mTableViewModel->dataChanged(mTableViewModel->getIndexOfToday(Col::DAY),
                                      mTableViewModel->getIndexOfToday(Col::MAX_COL));
}

// =============================================================================
bool TableModel::isStarted() {
    return mTableViewModel->data(mTableViewModel->getIndexOfToday(Col::START_TIME),
                                 Qt::UserRole + USER_ROLE_START_TIME_IS_VALID).toBool();
}

// =============================================================================
bool TableModel::isStopped() {
    return mTableViewModel->data(mTableViewModel->getIndexOfToday(Col::END_TIME),
                                 Qt::UserRole + USER_ROLE_END_TIME_IS_VALID).toBool();
}

// =============================================================================
void TableModel::setStartTime(int day, const QTime& startTime) {
    QModelIndex cellIndex = mTableViewModel->index(day + 1, Col::START_TIME);
    mTableViewModel->setData(cellIndex, startTime, Qt::EditRole);
    emit mTableViewModel->dataChanged(mTableViewModel->getIndexOfToday(Col::DAY),
                                      mTableViewModel->getIndexOfToday(Col::MAX_COL));
}

// =============================================================================
void TableModel::clear(int daysInMonth)
{

}

// =============================================================================
void TableModel::lockUnlock(bool unlock)
{
 //   setShowGrid(unlock);
}

