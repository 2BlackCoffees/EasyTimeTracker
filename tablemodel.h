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

#ifndef TABLE_MODEL_H
#define TABLE_MODEL_H

#include "tableviewmodel.h"
#include "misc.h"
#include "iconnection.h"

#include <QTableView>
#include <QDateTime>
#include <QStringList>
#include <QMutex>
#include <QFile>

#include <memory.h>
#include <boost/scoped_ptr.hpp>

class TimeTrackingModel;


class TableModel : public QObject
{
    Q_OBJECT

public:
    enum TUserRole {
        USER_ROLE_START_TIME_IS_VALID,
        USER_ROLE_END_TIME_IS_VALID
    };



    explicit TableModel(QWidget *parent);
    virtual ~TableModel() {}

    void load(const QString &filename=QString());
    void save(const QString &filename=QString());

    bool isStarted();
    bool isStopped();

    void updateContent();
    void setParent(IConnection* parent) {
        mParent = parent;
    }
    bool sheetOfCurMonth() const {
        return mTableViewModel->sheetOfCurMonth();
    }
    TableViewModel * getViewModel() const {
        return mTableViewModel.get();
    }
    void setUserName(const QString & userName) {
        Q_UNUSED(userName);
        mTableViewModel->setUserName(Misc::getUserName());
    }
    void setNewBaseDate(const QDate & baseDate) {
        mTableViewModel->setNewBaseDate(baseDate);
    }

signals:
    void modified();

public slots:
    void startTT();
    void stopTT();

public slots:
    void lockUnlock(bool unlock);

private:
    QWidget* mParentWidget;
    boost::scoped_ptr<TableViewModel> mTableViewModel;
    IConnection * mParent;

    void setStartTime(int day, const QTime& startTime);

    void clear(int daysInMonth);

    QString formatString(const QString &string) const;

};

#endif
