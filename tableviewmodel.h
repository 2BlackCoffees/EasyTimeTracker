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

#ifndef SPREADSHEET_H
#define SPREADSHEET_H

#include "misc.h"
#include "iconnection.h"

#include <QTableView>
#include <QDateTime>
#include <QStringList>
#include <QMutex>
#include <QFile>

#include <memory.h>

class TimeTrackingModel;

namespace Col {
    enum Col {
        DAY, START_TIME, END_TIME, PAUSE_TIME, WORKED_TIME, REQUIRED_TIME, DUE_OVER_HOURS, COMMENT,
        MAX_COL /* Leave me last !*/
    };
}

class TableViewModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    enum TUserRole {
        USER_ROLE_START_TIME_IS_VALID,
        USER_ROLE_END_TIME_IS_VALID,
        USER_ROLE_FORCE_CHANGES,
        USER_ROLE_GET_MONTH_OF_SHEET
    };



    explicit TableViewModel(QWidget *parent);

    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant data(const QModelIndex &index,
                  int role=Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role=Qt::DisplayRole) const;
    int rowCount(const QModelIndex &) const;
    int columnCount(const QModelIndex &) const;

    bool setData(const QModelIndex &index, const QVariant &value,
                 int role=Qt::EditRole);
    bool setHeaderData(int, Qt::Orientation, const QVariant&,
                       int=Qt::EditRole) {
        return false;
    }

    QModelIndex getIndexOfToday(Col::Col column) {
        return index(QDate::currentDate().day() - 1, column);

    }
    bool sheetOfCurMonth() const;
    void setUserName(const QString & userName);
    void setNewBaseDate(const QDate & baseDate);
    TimeTrackingModel * getTimeTrackingModel() {
        return mTimeTrackingModel;
    }

public slots:
    //void lockUnlock(bool unlock);

private:
    mutable QMutex* mutex;
    enum { MagicNumber = 0x7F51C883, ColumnCount = Col::MAX_COL };

    const QString mColNameDay;
    const QString mColNameStartTime;
    const QString mColNameEndTime;
    const QString mColNamePauseTime;
    const QString mColNameWorkedTime;
    const QString mColNameRequiredTime;
    const QString mColNameDueOverHours;
    const QString mColNameComment;

    QStringList mColumnsName;
    QStringList mColumnsPattern;
    int mSignalActDeactSemaphore;
    IConnection* mParent;
    QWidget* mParentWidget;
    bool mDirty;
    TimeTrackingModel* mTimeTrackingModel;

    QString formatString(const QString &string) const;

};


#endif
