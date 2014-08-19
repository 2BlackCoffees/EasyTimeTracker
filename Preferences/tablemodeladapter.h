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

#ifndef TABLEMODELADAPTER_H
#define TABLEMODELADAPTER_H
#include "timetrackingsettings.h"
#include "debug.h"

#include <QVariant>

class TableModelAdapter
{
public:
    enum {
        COL_WORKING_DAY = 0,
        COL_WORKING_HOURS,
        COL_PAUSE,

        COL_MAXCOL // This one should remain latest
    };
    TableModelAdapter();

    int rowCount() const {
        return mRowCount;
    }
    int columnCount() const {
        return mColCount;
    }
    QVariant horizontalHeaderData(int section) const;
    QVariant verticalHeaderData(int section) const;

    bool isWorkingDay(int weekDay) const {
        return TimeTrackingSettings::getInstance()->isWorkingDay(mDate.addDays(weekDay));
    }

    QTime getRequiredTime(int weekDay) {
        return TimeTrackingSettings::getInstance()->getRequiredWorkTime(
                    mDate.addDays(weekDay));
    }

    QTime getPause(int weekDay) {
        return TimeTrackingSettings::getInstance()->getPause(
                    mDate.addDays(weekDay));
    }

    void setWorkingDay(int weekDay, bool isWorkingDay) const {
        TimeTrackingSettings::getInstance()->setWorkingDay(
                    mDate.addDays(weekDay), isWorkingDay);
    }

    void setRequiredTime(int weekDay, const QTime& time) {
        TimeTrackingSettings::getInstance()->setRequiredWorkTime(
                    mDate.addDays(weekDay), time);
    }

    void setPause(int weekDay, const QTime& time) {
        TimeTrackingSettings::getInstance()->setPause(
                    mDate.addDays(weekDay), time);
    }

private:
    int mRowCount;
    int mColCount;
    QDate mDate;
};

#endif // TABLEMODELADAPTER_H
