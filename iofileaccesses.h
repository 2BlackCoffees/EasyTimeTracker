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

#ifndef IOFILEACCESSES_H
#define IOFILEACCESSES_H

#include <QtXml/QDomDocument>
#include <QHash>
#include <QIODevice>
#include "iiofileaccesses.h"
#include "misc.h"

class TimeTrackingModel;

class IOFileAccesses : public IIOFileAccesses
{
private:
    QDomDocument mDomDocument;
    TimeTrackingModel* mTimeTrackingModel;
    QString mTagMain;
    QString mTagVersion;
    QString mTagDay;
    QString mTagUserName;
    QString mTagYear;
    QString mTagMonth;
    QString mTagCompensatedSeconds;
    QString mTagPrevMonthlyOverDueSeconds;
    QString mTagDayNumber;
    QString mTagDayStarted;
    QString mTagDayEnded;
    QString mTagDayPause;
    QString mTagDayRequiredWorkTime;
    QString mTagDayComment;
    QDate mDate;
    int mDebug;
    static const int IDX_FILENAME_SHIFT_YEAR;
    static const int IDX_FILENAME_SHIFT_MONTH;

    enum DEBUG_STATE {
        NONE = 0,
        BASIC,
        DETAILED
    };

public:
    explicit IOFileAccesses(TimeTrackingModel* timeTrackingModel) :
        mTimeTrackingModel(timeTrackingModel),
        mTagMain("ett"),
        mTagVersion("version"),
        mTagDay("day"),
        mTagUserName("userName"),
        mTagYear("year"),
        mTagMonth("month"),
        mTagCompensatedSeconds("compensatedSeconds"),
        mTagPrevMonthlyOverDueSeconds("prevMonthlyOverDueHours"),
        mTagDayNumber("number"),
        mTagDayStarted("started"),
        mTagDayEnded("ended"),
        mTagDayPause("pause"),
        mTagDayRequiredWorkTime("requiredWorkTime"),
        mTagDayComment("comment"),
        mDebug(BASIC)

    {

    }
    virtual bool openSheetInformation(TimeTrackingModel *timeTrackingModel = NULL,
                                      bool silent = false);
    virtual bool saveSheetInformation();
    bool saveSheetInformation(const QStringList & fileList);
    virtual bool setPreviousBalance();
    bool setPreviousBalance(const QStringList & fileList);
    static void updateAllBalances(const QString & userName);
    bool saveSheetInformation(QIODevice* file, TimeTrackingModel *timeTrackingModel);



private:
    bool openSheetInformation(QIODevice* file,
                              TimeTrackingModel *timeTrackingModel = NULL);
    bool readHeader(const QDomElement& root, TimeTrackingModel *timeTrackingModel);
    bool readChild(const QDomElement& child, TimeTrackingModel *timeTrackingModel);
    static QStringList getFileList(const QString & userName, int fromYear = -1, int fromMonth = -1);
};

#endif // IOFILEACCESSES_H
