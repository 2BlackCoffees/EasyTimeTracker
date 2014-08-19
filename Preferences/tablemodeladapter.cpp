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

#include "tablemodeladapter.h"

TableModelAdapter::TableModelAdapter()
{
    mRowCount = 7;
    mColCount = 3;

    // Write day names using Qdate: Use urrent day and look for previous monday
    mDate = QDate::currentDate().addDays(-14);
    while(mDate.dayOfWeek() != 1){
        mDate = mDate.addDays(1);
    }

}

// =============================================================================
QVariant TableModelAdapter::horizontalHeaderData(int section) const {
    switch(section) {
    case 0:
        return QString("Working day");
    case 1:
        return QString("Expected work hours");
    case 2:
        return QString("Expected pause");

    }
    DEBUG(QString("Unexpected value of Section : %1").arg(section));
    return QString("Unexpected value of Section : %1").arg(section);
}

// =============================================================================
QVariant TableModelAdapter::verticalHeaderData(int section) const {
    return mDate.addDays(section).toString("ddd");
}
