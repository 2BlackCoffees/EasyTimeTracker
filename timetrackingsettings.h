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

#ifndef TIMETRACKINGSETTINGS_H
#define TIMETRACKINGSETTINGS_H

#include "ttexceptions.h"
#include "debug.h"

#include <QString>
#include <QMap>
#include <QDate>

#include <stddef.h>

class TimeTrackingSettings
{
private:
    static TimeTrackingSettings* mTimeTrackingSettings;
    static const int NB_DAYS_PER_WEEK = 7;

    bool mIsWorkingDay[NB_DAYS_PER_WEEK];
    QTime mRequiredWorkTime[NB_DAYS_PER_WEEK];
    QTime mPause[NB_DAYS_PER_WEEK];
    QTime mPeriodDefaultAction;
    QTime mPeriodPauseTime;
    QString mPauseString;
    bool mDefaultActionEnable;
    bool mPauseTimeEnable;

public:
    static TimeTrackingSettings* getInstance() {
        if(mTimeTrackingSettings == NULL) {
            mTimeTrackingSettings = new TimeTrackingSettings();
        }

        return mTimeTrackingSettings;
    }

    bool isWorkingDay(const QDate& date) const {
        return isWorkingDay(date.dayOfWeek());
    }
    bool isWorkingDay(int weekDay) const;
    QTime getRequiredWorkTime(const QDate& date) const;
    QTime getPause(const QDate& date) const;
    QTime getPeriodDefaultAction() const;
    QTime getPeriodPauseTime() const {
        return mPeriodPauseTime;
    }
    QString getPauseString() const {
        return mPauseString;
    }
    bool getEnablePeriodPauseTime() const {
        return mPauseTimeEnable;
    }

    void setWorkingDay(const QDate& date, bool isWorkingDay);
    void setRequiredWorkTime(const QDate& date, const QTime& time);
    void setPause(const QDate& date, const QTime& time);
    void setPeriodDefaultAction(const QTime& time);

    void setEnablePeriodDefaultAction(bool enable) {
        mDefaultActionEnable = enable;
    }
    bool getEnablePeriodDefaultAction() {
        return mDefaultActionEnable;
    }

    void setPeriodPauseTime(const QTime& time) {
        mPeriodPauseTime = time;
    }
    void setPauseString(const QString& string) {
        mPauseString = string;
    }
    void setEnablePeriodPauseTime(bool enable) {
        mPauseTimeEnable = enable;
    }
    void addAction(const QString & command, const QString & parameter) {
       addActionFromKey(actionToKey(command, parameter), parameter);
    }
    void addActionFromKey(const QString & key, const QString & parameter) {
        mActions[key] = parameter;
    }

    bool removeAction(const QString & command, const QString & parameter);

    const QMap<QString, QString> & getActionsWithKeys() const {
        return mActions;
    }

    QMultiHash<QString, QString> getActions() const {
        QMultiHash<QString, QString> tmpMultiHash;
        for(auto pair: mActions.toStdMap()) {
            DEBUG(QString("Extracting action %1: %2, %3").arg(keyToCommand(pair.first)).arg(pair.first).arg(pair.second));
            tmpMultiHash.insertMulti(keyToCommand(pair.first), pair.second);
        }
        return tmpMultiHash;
    }
    QString getDefaultBrowserCommand() {
        return mDefaultBrowser;
    }
    bool isCommandDefaultBrowser(const QString & command) {
        return command == mDefaultBrowser;
    }

    void informClients() {

    }

    void setDefaultValues();

    QString getPrintingRealName() const {
        return mPrintingRealName;
    }

    void setPrintingRealName(const QString & printingRealName) {
        mPrintingRealName = printingRealName;
    }

    QString getPrintingFooter() const {
        return mPrintingFooter;
    }

    void setPrintingFooter(const QString & printingFooter) {
        mPrintingFooter = printingFooter;
    }

    bool getPrintingShowPicture() const {
        return mPrintingShowPicture;
    }

    void setPrintingShowPicture(bool printingShowPicture) {
        mPrintingShowPicture = printingShowPicture;
    }

private:
    QMap<QString, QString> mActions;
    const QString mActionSeparator;
    const QString mDefaultBrowser;
    QString mPrintingRealName;
    QString mPrintingFooter;
    bool mPrintingShowPicture;

    TimeTrackingSettings();

    QString actionToKey(const QString & command, const QString & parameter) const {
        return command + mActionSeparator + parameter;
    }

    QString keyToCommand(const QString & key) const {
        int length = key.indexOf(mActionSeparator);
        if(length >= 0) {
            return key.mid(0, length);
        }
        return QString();
    }
};

#endif // DEFAULTTIMETRACKINGSETTINGS_H
